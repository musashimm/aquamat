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

	$Id$
*/

#include <avr/interrupt.h>
#include "pwm.h"
#include "menu.h"
#include "eeprom.h"
#include "timerssec.h"
#include "strings.h"
#include "hd44780.h"

/** @file pwm.c
	@brief Implementacja wyjść PWM.

	Definicja \ref PWM_PORT określa, który port będzie portem PWM. Definicja \ref PWM_PINS określa, które porty będą PWM.
*/

extern uint8_t clock10ms;

/** @struct PWM
	@brief Struktura przechowywująca dane o wyjściach PWM.

	Index wyjścia PWM odpowiada pinowi w porcie \ref PWM_PORT.
*/
struct PWM pwms[PWM_NUM];

/** Inicjalizacja wyjść PWM
*/
void pwm_init(void) {

	PWM_DDR |= PWM_PINS;

	TCCR0 = _BV(WGM01) |_BV(CS01);	// clock / 8 - wyserowanie na porównanie 1/8000000*25*8*4*100
	OCR0 = PWM_TIMER_TAU;			// wartość licznika CTC
	TIMSK |= _BV(OCIE0);			// przerwanie na porównanie
}

/** Procedura przerwania ustawiająca pwm na porcie.
*/
ISR (TIMER0_COMP_vect) {

	static volatile uint8_t clock1_4_1_10ms;
	static volatile uint8_t clock1_10ms;
	uint8_t i;

	if(!(--clock1_4_1_10ms)) {
		clock1_4_1_10ms = 4;
		if(!(--clock1_10ms)) {
			clock1_10ms = 100;
		}
		for (i=0;i < PWM_NUM;i++) {
			if (clock1_10ms <= pwms[i].width) {
				PWM_PORTOUT |= _BV(i);
			} else {
				PWM_PORTOUT &= ~_BV(i);
			}
		}
	}
}

/** Ustawia żądaną wartość PWM na wyjście.
	@param uint8_t oidx index wyjścia,
	@param uint8_t pwm wartość PWM.
*/
void pwm_set(uint8_t oidx,uint8_t pwm) {
	if (oidx < PWM_NUM) {
		pwms[oidx].width = pwm;
	}
}

/** Wyświetlanie i konfiguracja wyjść PWM
*/
void ui_pwm_settings_display(void) {

	if (!MENU_F(MENU_VARS_LOADED_FLAG)) {
		load_name(menu_unit() * EEPROM_PWM_SIZE + EEPROM_PWM_SETTINGS_BEGIN + EEPROM_PWM_NAME_OFFSET);
		MENU_SF(MENU_VARS_LOADED_FLAG);
	}

	hd44780_label(OUTPWM_S,FALSE,FALSE);
	hd44780_out4hex(menu_unit()+1);
	hd44780_outstrn_P(SPACE_S);
	hd44780_outstrn_P(PSTR("P="));
	hd44780_out8dec3(pwms[menu_unit()].width);
	hd44780_outstrn_P(PSTR("%"));
	hd44780_next_line();

	hd44780_outstrn_P(PAR_S);
	hd44780_label(NZW_S,TRUE,FALSE);
	hd44780_outstrn(name);
	hd44780_outstrn_P(PAR_S);

	switch (menu_phase()) {
	case 0:
		menu_standard_unit(PWM_NUM);
		break;
	case 1:
		menu_set_phase();
		break;
	case 2:
		menu_mod_uint8(1,13,&pwms[menu_unit()].width,0,PWM_MAX_WIDTH,FALSE);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		menu_mod_char(2,10,name,3);
		break;
	case 8:
		save_pwm_settings(menu_unit() * EEPROM_PWM_SIZE + EEPROM_PWM_SETTINGS_BEGIN,menu_unit());
		save_name(menu_unit() * EEPROM_PWM_SIZE + EEPROM_PWM_SETTINGS_BEGIN + EEPROM_PWM_NAME_OFFSET);
		menu_reset_phase();
	}
}

/** Wczytuje konfigurację wszystkich wyjść PWM.
*/
void load_pwms_settings(uint16_t addr) {
	uint8_t i;
 	for (i=0;i < PWM_NUM;i++) {
 		load_pwm_settings(i * EEPROM_PWM_SIZE + addr,i);
 	}
}

/** Zapisuje konfigurację wszystkich wyjść PWM.
*/
void save_pwms_settings (uint16_t addr) {
	uint8_t i;
 	for (i=0;i < PWM_NUM;i++) {
 		save_pwm_settings(i * EEPROM_PWM_SIZE + addr,i);
 	}
}

/** Zapisuje konfigurację jednego wyjścia PWM.
	@param uint16_t base_addr adres bazowy, pod który będzie zapisana konfiguracja
	@param uint8_t o index wyjścia PWM.
*/
void save_pwm_settings(uint16_t base_addr,uint8_t o) {
	eeprom_write_byte(base_addr + EEPROM_PWM_WIDTH_OFFSET,pwms[o].width);
}

/** Wczytuje konfigurację wszystkich wyjść PWM.
	@param uint16_t addr adres, pod którym zostanie zapisana konfiguracja
	@param uint8_t o index wyjścia PWM.
*/
void load_pwm_settings(uint16_t base_addr, uint8_t o) {
	pwms[o].width = eeprom_read_byte(base_addr + EEPROM_PWM_WIDTH_OFFSET);
	if (pwms[o].width > PWM_MAX_WIDTH) {
		pwms[o].width = 0;
	}
}

