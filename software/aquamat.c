/*
    Aquamat - Sterownik akwariowy Open Source
    Copyright (C) 2009 Wojciech Todryk (wojciech@todryk.pl)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	$Id: aquamat.c 49 2009-05-05 21:26:37Z musashi@todryk.pl $
*/

/** @file aquamat.c
	@brief Główna pętla programowa
*/

#include "aquamat.h"
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include "hd44780.h"
#include "usart.h"
#include "comm_gui.h"
#include "rtc.h"
#include "ui.h"
#include "one_wire.h"
#include "eeprom.h"
#include "temp.h"
#include "log.h"
#include "outputs.h"
#include "twi.h"
#include "timers.h"
#include "timersv.h"
#include "alarms.h"
#include "strings.h"
#include "top_off.h"
#include "commandsDefs.h"
#include "qbuttons.h"
#include "menu.h"
#include "timerssec.h"
#include "pwm.h"

/** Wskaźnik do funkcji restart.
*/
void(*restart)(void) = 0x0000;

volatile uint8_t clock10ms = TICKS;
volatile uint8_t clock05s = 2;

volatile uint8_t seconds; /**< Aktualny czas - sekundy.*/
volatile uint8_t minutes; /**< Aktualny czas - minuty.*/
volatile uint8_t hours; /**< Aktualny czas - godziny.*/

volatile uint8_t year = 9;
volatile uint8_t month = 1;
volatile uint8_t day = 1;
volatile uint8_t wday = 1;
volatile uint8_t pinB;
volatile uint8_t pinC;
volatile uint8_t pinD;
volatile uint8_t back_light_counter;

volatile uint8_t tmp_var;
volatile uint8_t tmp_var2;

extern uint8_t pcmbuff;
extern uint8_t cm_buffer[CM_BUFFER_LENGTH];

/** @var volatile uint8_t system_flags
	@brief Zmienna przechowująca flagi związane ze zdarzeniami systemowymi.
	@verbatim
    7654|3210   - numer bitu
        |   X   -
        |  X    - FLAG_HOUR_PAST
        | X     - FLAG_FORCE_LCD_SEMI_REFRESH
        |X      - FLAG_DAY_PAST
       X|       - FLAG_FORCE_LCD_REFRESH
      X |       - FLAG_HALF_SECOND_PAST
     X  |       - FLAG_SECOND_PAST
    X   |       - FLAG_MINUTE_PAST
	@endverbatim
*/
volatile uint8_t system_flags;

volatile uint8_t temp_counter;
struct MIT daytime_from;
struct MIT daytime_to;

/** @var static const PROGMEM unsigned char extraChar[]
	@brief Definicja dodatkowych znaków LCD.
*/
static const PROGMEM unsigned char extraChar[] =
{
	0x1c, 0x14, 0x1c, 0x0, 0x3, 0x4, 0x4,0x3,	//0 LCD_CHAR_DEGREE
 	0xa, 0xa, 0xe, 0x4, 0x4, 0xe, 0xa, 0xa,		//1 LCD_CHAR_FAILURE
	0x11, 0xa, 0x4, 0x11, 0xa, 0x4, 0x0, 0x0,	//2 LCD_CHAR_COOLING
	0x4, 0xa, 0x11, 0x4, 0xa, 0x11, 0x0, 0x0,	//3 LCD_CHAR_HEATING
	0xe, 0xa, 0xe, 0x0, 0x0, 0x0, 0x0, 0x0,     //4 LCD_CHAR_OFF
	0xe, 0xa, 0xe, 0x0, 0x8, 0xe, 0xa, 0xe,     //5 LCD_CHAR_OFF_BLOCKED
	0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0xe, 0xe, 	//6 LCD_CHAR_ON
	0x8, 0xe, 0xa, 0xe, 0x0, 0xe, 0xe, 0xe		//7 LCD_CHAR_ON_BLOCKED
};

ISR (TIMER1_COMPA_vect) {

	pinB = PINB;
	pinC = PINC;
	pinD = PIND;						    // co 10ms sprwawdzenie stanu klawiszy
	if (counterKey < UINT8_T_DISABLED) { counterKey++; }

	if (!(--clock10ms)) {					//############### minęło pół sekundy
    	clock10ms=TICKS;
		SSF(FLAG_HALF_SECOND_PAST);
		if (!(--clock05s)) {				//############### minęła sekunda
			clock05s=2;
			SSF(FLAG_SECOND_PAST);
			if (++seconds == 60) {          //############### minęła minuta
				SSF(FLAG_MINUTE_PAST);
				seconds=0;
				if (++minutes == 60) {      //############### minęła godzina
					SSF(FLAG_HOUR_PAST);
					minutes=0;
					if (++hours == 24) {    //############### mineła północ
						SSF(FLAG_DAY_PAST);
						hours=0;
					}
				}
			}
		}
	}
}

ISR (USART_RXC_vect) {
    //if (SF(FLAG_COMMAND_PROCESSING)) {
        cm_buffer[pcmbuff++] = UDR;
    //} else {
        //gui_cm_return_status(GUI_STATUS_COMMAND,GUI_PROCESSING_COMMAND,UINT8_T_DISABLED);
    //}
}

/** Inicjalizacja urządzeń wejścia / wyjścia.
*/
void ioinit(void)
{
	uint8_t i;
	hd44780_init();
	hd44780_outcmd(HD44780_CLR);
	hd44780_wait_ready();
	hd44780_outcmd(HD44780_ENTMODE(1, 0));
	hd44780_wait_ready();
	hd44780_outcmd(HD44780_DISPCTL(1, 1, 1));
	hd44780_wait_ready();
	hd44780_outcmd(HD44780_CGADDR(0));
	for(i=0; i<64; i++) {
		hd44780_outdata(pgm_read_byte(&extraChar[i]));
	}

	TCCR1B = _BV(WGM12) | _BV(CS11);                        // licznik / 8 - wyzerwoanie na porownanie
	TIMSK = _BV(OCIE1A);                                    // przerwanie na porownanie wartosci
	OCR1A = TAU1;                                           // warto licznika porownania

	usart_init(UBRR_VALUE);

	onewire_init();

	twi_init();

	outputs_init();

	top_off_init();

	qbuttons_init();

	ui_init();

	BUZZER_DDR |= _BV(BUZZER_SWITCH);
	buzzer_off();

	pwm_init();

	wdt_enable(WDTO_2S);
}

/** Główna funkcja programu
*/
int main(void)
{
	FuncPtr FPtr;

	ioinit();

	hd44780_clear();
	hd44780_bl_on();
	hd44780_cursor_off();

	load_basic_settings(EEPROM_BASIC_SETTINGS_BEGIN);
	load_temp_sensors_settings(EEPROM_TEMP_SETTINGS_BEGIN);
	load_outputs_settings(EEPROM_OUTS_SETTING_BEGIN);
	load_timers_settings(EEPROM_TIMERS_SETTING_BEGIN);
	load_timersv_settings(EEPROM_TIMERSV_SETTING_BEGIN);
	load_topoff_settings(EEPROM_TOPOFF_SETTINGS_BEGIN);
	load_timerssec_settings(EEPROM_TIMERSSEC_SETTINGS_BEGIN);
	load_pwms_settings(EEPROM_PWM_SETTINGS_BEGIN);

	hd44780_printOnLcdDelay(PSTR(PROGRAM_FULL_NAME));
	hd44780_clear();
	sei();

	rtc_get_time();
	rtc_get_date();

	gui_client_present();

	log_write_record(LOG_EVENT_START,0,0,0);

	for (;;) {

		wdt_reset();

		qbuttons_process();

		ui_key_pressed();

		if (key_pressed && alarms_notification_enabled() && alarms_is_active() ){
			alarms_block_notification();
			menu_set(1,1,0,0);
			key_pressed = NONE;
		}

		//########################### akcje wykonywane kiedy minie sekunda
		if (SF(FLAG_SECOND_PAST)) {
			SSF(FLAG_FORCE_LCD_REFRESH);
			temp_counter++;
			top_off_check_time();
			top_off_process();
			timerssec_when_active();
			CSF(FLAG_SECOND_PAST);
		}

 		//########################### akcje wykonywane kiedy minie pół sekundy
		if (SF(FLAG_HALF_SECOND_PAST)) {
			SSF(FLAG_FORCE_LCD_SEMI_REFRESH);
			if (alarms_is_active() && alarms_notification_enabled()) {
				buzzer_toggle();
			} else {
				buzzer_off();
			}
			CSF(FLAG_HALF_SECOND_PAST);
		}

		//######################################## odświeżenie wyswietlacza LCD
		if (system_flags & LCD_REFRESH_MASK) {
			//if (back_light_counter < LCD_BL_SHUTOFF_TIME) { hd44780_bl_on(); }
			FPtr=(FuncPtr)pgm_read_word(&FuncPtrTable[menu_func_index()]);
			hd44780_outcmd(HD44780_HOME);
			FPtr();
			menu_dummy();
			CSF(FLAG_FORCE_LCD_REFRESH);
			CSF(FLAG_FORCE_LCD_SEMI_REFRESH);
		}

		//########################### akcje wykonywane kiedy minie minuta
		if (SF(FLAG_MINUTE_PAST)) {
			//if (back_light_counter < 0xFF) { back_light_counter++; }
			//if (back_light_counter >= LCD_BL_SHUTOFF_TIME) { hd44780_bl_off(); }
			timers_process();
			timersv_process();
			if (alarms_block_notification_counter < ALARM_MAX_BLOCK_VALUE) {
				alarms_block_notification_counter++;
			}
			timerssec_when_notactive();
			CSF(FLAG_MINUTE_PAST);
		}

		//########################### akcje wykonywane kiedy minie godzina
		if (SF(FLAG_HOUR_PAST)) {
			temp_register();
			CSF(FLAG_HOUR_PAST);
		}

		//########################### akcje wykonywane jak minie dzień
		if (SF(FLAG_DAY_PAST)) {
			rtc_get_time();
			rtc_get_date();
			CSF(FLAG_DAY_PAST);
		}

		//########################### pomiar temeratury
		if (temp_counter > TEMP_SENSOR_READ_INTERVAL) {
			temp_read_temperature();
			temp_counter = 0;
		}

		//######################################## obsługa interfejsu szeregowego
        gui_cm_process_char();

        //######################################## aktualizacja stanu wyjść
		outputs_update();
    }
  	return 0;
}

/** @mainpage Aquamat - Sterownik akwariowy Open Source
 	@image html logo.png
@verbatim
    Aquamat - Sterownik akwariowy Open Source
    Copyright (C) 2009 Wojciech Todryk (wojciech@todryk.pl)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 @endverbatim

	@section intro_sec Wstęp
 	Celem projektu jest zaprojektowanie, implementacja i uruchomienie sterownika akwariowego o otwartej architekturze, łatwego do budowy i rozbudowy.
 	@section func_sec Funkcjonalność
	Poniżej został przedstawiony diagram planowanej funkcjonalności w docelowej wersji sterownika.

	@image html functions.png

	@subsection func_current_sec Funkcjonalność wersji 1.X.X

- Zegar RTC
	- obsługa dni tygodnia
- Wyjścia
	- 16 wyjść (4 typu LowVoltage i 12 HighVoltage)
	- możliwość niezależnej blokady każdego wyjścia
- Timery godzinowe
	- 8 timerów
	- przypisanie niezależnie do dowolnego wyjścia
	- możliwość blokady
	- obsługa dni tygodnia
	- rozdzielczość minutowa
- Timery interwałowe
	- 8 timerów
	- czas włączenia / wyłączenia regulowany od 1-99 minut
	- rozdzielczość minutowa
- Kontrola temperatury
	- 4 czujniki DS18B20 na jednym przwodzie (ROM_SEARCH)
	- niezależne przypisanie wyjść COOL i HEAT do każdego czujnika
	- rozdzielczość pomiarowa 0.1 st. C
	- temperatura dzienna i nocna
- Interfejs komunikacyjny
	- RS-232
	- pełne sterowanie i konfiguracja za pomocą interfejsu graficznego \ref AquamatControl
- Wyświetlacz LCD
	- wyświetlanie statusu sterownika
	- stan poszczególnych wyjść
	- stan czujników tepmeratury
- Modul alarmowy
    - alarmowanie o przkroczeniu progowych temperatur
    - alarm w przypadku uszkodzenia czujnika temperatury
- Log
	- dziennik systemowy umożliwiajacy zapis do 999 zdarzeń systemowych (start sterownika, alarmy max temp,min temp, uszkodzenie czujnika)
- Automatyczna dolewka
- Przyciski szybkiego wysterowania wyjść (Quick Buttons)

@section modules Moduły sterownika
Celem łatwiejszej implementacji projekt został podzielony na moduły funkcjonalne. Poniższy rysunek przedstawia podstawowe zależności między modułami.

@image html aquamat_moduly.png

Staram się aby, aby funkcjonalności stetrownika miały budowę modułową. Dzięki temu, przy odpowiednich ustawieniach warunkowej kompilacji będzie można, pewne funkcję włączać lub wyłączać.

@image html aquamat_module.png

- \ref temp.c Kontrola temperatury
- \ref top_off.c Automatyczna dolewka
- \ref outputs.c Sterowanie wyjściami


@section micro Mikrokontroler
Sterownik implementowany jest w z wykorzystaniem mikrokontrolera z rodziny AVR 8-bit. W tej chwili prace opierają sie o model ATmega32. Poniżej zamieszczono przyporządkowanie poszczególnych pinów.
@image html pins_assign.png

@section hardware Sprzęt
Ze względu na określone miejsce w szafce i wielkość projektu, część sprzętowa została podzielona na 3 moduły:
- avr_core - główna płytka projektu
- outputs board 16 - płytka z wyjściami
- relays board 6 - płytka listwy sterującej z przekaźnikami

Zależność między modułami pokazano na poniższym schemacie:

@image html hardware_1_X_X.png


@page changes Historia zmian
@section release_1_0_12 Wersja 1.0.12
- poprawiony błąd z indeksowaniemi tablicy nazw wyjść
 */
