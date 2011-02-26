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

	$Id: aquamat.h 48 2009-03-27 21:37:50Z musashi@todryk.pl $
*/

#ifndef AQUAMAT_H
#define AQUAMAT_H

/** @file aquamat.h
	@brief Plik nagłówkowy - Główna pętla programowa.
*/

/** Niektóre ustawienia oraz niektóry sprzęt są inne w wersji prototypowej, np. niektóre adresy układów I2C,
	więc poniższą definicją przełączam się na wersję prototypową.
	Normalnie poniższa definicja powinna pozostać zakomentowana.
*/

#define PROTOTYPE yes
#ifdef PROTOTYPE
#define BUZZER_ON_HIGH
#warning ################################################
#warning PROTOTYPE mode enabled, disable it before commit
#warning ################################################
#endif

/** Częstotliwość pracy kwarcu
*/
#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "commonDefs.h"

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

typedef uint16_t TEMP;

#define GLUE(a, b)     a##b
#define PORT(x)        GLUE(PORT, x)
#define PIN(x)         GLUE(PIN, x)
#define DDR(x)         GLUE(DDR, x)
#define pin(x)         GLUE(pin, x)

/** Definicja prawdy.
*/
#define TRUE 1

/** Definicja fałszu.
*/
#define FALSE 0

/** Definicja włączenia.
*/
#define ON 1

/** Definicja wyłączenia.
*/
#define OFF 0

/** Definicja wartości zablokowanej / nieaktywnej dla zmiennych 16-bitowych.
*/
#define UINT16_T_DISABLED 0xFFFF
/** Definicja wartości zablokowanej / nieaktywnej dla zmiennych 8-bitowych.
*/
#define UINT8_T_DISABLED 0xFF

#define PROGRAM_NAME "Aquamat"
#define PROGRAM_VERSION_MAIN 1
#define PROGRAM_VERSION_MAJOR 3
#define PROGRAM_VERSION_MINOR 0
#define PROGRAM_FULL_NAME  "    " PROGRAM_NAME "\n  wersja " STRINGIFY(PROGRAM_VERSION_MAIN) "." STRINGIFY(PROGRAM_VERSION_MAJOR) "." STRINGIFY(PROGRAM_VERSION_MINOR)

/** ustawienia timera */
#define TAU1 10000
#define TICKS 50

/** sterowanie podswietlaniem LCD */
#define HD44780_BL_PORT A
#define HD44780_BL_SWITCH PORT7

//#define HD44780_BL_ON_LOW YES

/** obsluga wyswietlacza LCD */
#define HD44780_DATA_PORT C
#define HD44780_CONTROL_PORT A
#define HD44780_RS PORT4
#define HD44780_RW PORT5
#define HD44780_E  PORT6
#define HD44780_D4 PORT4
#define HD44780_D5 PORT5
#define HD44780_D6 PORT6
#define HD44780_D7 PORT7

/** Domyślna pora dnia
*/
#define DAYTIME_FROM_DEFAULT_HOURS 6
#define DAYTIME_FROM_DEFAULT_MINUTES 0
#define DAYTIME_TO_DEFAULT_HOURS 18
#define DAYTIME_TO_DEFAULT_MINUTES 0

/** dodatkowe znaki */
#define LCD_CHAR_DEGREE 0
#define LCD_CHAR_FAILURE 1
#define LCD_CHAR_COOLING 2
#define LCD_CHAR_HEATING 3
#define LCD_CHAR_OFF 4
#define LCD_CHAR_OFF_BLOCKED 5
#define LCD_CHAR_ON 6
#define LCD_CHAR_ON_BLOCKED 7

#define LCD_BL_SHUTOFF_TIME 10

/** buzzer */
#define BUZZER_PORT C
#define BUZZER_SWITCH PORT3
#define BUZZER_PORTOUT PORT(BUZZER_PORT)
#define BUZZER_DDR DDR(BUZZER_PORT)

/** W zależności od użytego tranzystora NPN lub PNP sterującego buzzer'em.
*/
#ifdef BUZZER_ON_HIGH
#define buzzer_on() BUZZER_PORTOUT |= _BV(BUZZER_SWITCH)
#define buzzer_off() BUZZER_PORTOUT &= ~_BV(BUZZER_SWITCH)
#else
#define buzzer_on() BUZZER_PORTOUT &= ~_BV(BUZZER_SWITCH)
#define buzzer_off() BUZZER_PORTOUT |= _BV(BUZZER_SWITCH)
#endif

#define buzzer_toggle() BUZZER_PORTOUT ^= _BV(BUZZER_SWITCH)

/** definicje flag
	SSF - set system flag
	CSF - clear system flag
*/
#define SSF(x) (system_flags |= _BV(x))
#define CSF(x) (system_flags &= ~_BV(x))
#define SF(x) (system_flags & _BV(x))

#define FLAG_COMMAND_PROCESSING 0
#define FLAG_HOUR_PAST 1
#define FLAG_FORCE_LCD_SEMI_REFRESH 2
#define FLAG_DAY_PAST 3
#define FLAG_FORCE_LCD_REFRESH 4
#define FLAG_HALF_SECOND_PAST 5
#define FLAG_SECOND_PAST 6
#define FLAG_MINUTE_PAST 7

#define LCD_REFRESH_MASK (_BV(FLAG_FORCE_LCD_SEMI_REFRESH)|_BV(FLAG_FORCE_LCD_REFRESH))

/** externy */
typedef void (*FuncPtr)(void);
extern volatile uint8_t seconds;

extern struct MIT daytime_from;
extern struct MIT daytime_to;
extern volatile uint8_t system_flags;

extern volatile uint8_t pinB;
extern volatile uint8_t pinC;
extern volatile uint8_t pinD;

extern volatile uint8_t temp_counter;

void(*restart)(void);

#endif

