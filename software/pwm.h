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


#ifndef PWM_H
#define PWM_H

#include "aquamat.h"

/** @file pwm.h
	@brief Plik nagłówkowy - Implementacja wyjść PWM.
*/

#define PWM_PORT A
#define PWM_PINS (_BV(PORT0)|_BV(PORT1)|_BV(PORT2)|_BV(PORT3))
#define PWM_PORTOUT PORT(PWM_PORT)
#define PWM_DDR DDR(PWM_PORT)
#define PWM_NUM 4
#define PWM_MAX_WIDTH 100

#define PWM_TIMER_TAU 25
#define PWM_TIMER_TICKS 4

#define EEPROM_PWM_WIDTH_OFFSET 0
#define EEPROM_PWM_FLAGS_OFFSET 1
#define EEPROM_PWM_NAME_OFFSET 2
#define EEPROM_PWM_SIZE (EEPROM_PWM_NAME_OFFSET + NAME_LENGTH)

/** @struct PWM
	@brief Struktura przechowywująca dane o wyjściu PWM.
*/
struct PWM {
    uint8_t width;

/** Flagi.
	@verbatim
7654|3210
    |   X - blokada wyjścia
    |   0 - odblokowane/aktywne
    |   1 - zablokowane/nieaktywne
	@endverbatim
*/
	uint8_t flags;
};

/** Flaga blokady.
*/
#define PWM_FLAG_BLOCKED 0

/** Maska flag konfiguracyjnych.
*/
#define PWM_FLAG_CONFIG_MASK 0x0F

void pwm_init(void);
void pwm_to_port(void);
void ui_pwm_settings_display(void);
void load_pwm_settings(uint16_t,uint8_t);
void save_pwm_settings(uint16_t,uint8_t);
void load_pwms_settings(uint16_t);
void save_pwms_settings(uint16_t);
uint8_t pwm_check_flag(uint8_t,uint8_t);

#endif
