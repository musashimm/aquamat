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

	$Id:$
*/

#ifndef TIMERSSEC_H
#define TIMERSSEC_H

#include "time.h"

/** @file timerssec.h
	@brief Plik nagłówkowy - Implementacja timerów sekundowych.
*/

/** @struct TIMERSEC
	@brief Struktura przechowywująca dane o timerze sekundowym.
*/
struct TIMERSEC {
	struct MIT when; 	/**< Czas, od którego timer jest włączony. Struktura \ref MIT "MIT".*/
	uint8_t duration; 	/**< Czas w sekundach, przez jaki będzie timer.*/
	uint8_t on; 		/**< Czas w sekundach, przez jaki jest aktywny timer.*/
	uint8_t out;     	/**< Przypisane wyjście.*/

/** Flagi.
	@verbatim
7654|3210
    |   X - blokada timera
    |   0 - odblokowany/włączony
    |   1 - zablokowany/wyłączony
    |XXX  - dzien tygodnia działania timera
    |000  - Nd
    |001  - Pn
    |010  - Wt
    |011  - Sr
    |100  - Cz
    |101  - Pt
    |110  - So
    |111  - Każdego dnia
   X|     - flaga zajętości timera
   0|     - timer wyłączony
   1|     - timer włączony
	@endverbatim
	*/
	uint8_t flags;
};

/** Flaga blokady.
*/
#define TIMERSSEC_FLAG_BLOCKED 0

/** Flaga zajetości.
*/
#define TIMERSSEC_FLAG_BUSY 4

/** Maska dni tygodnia
*/
#define TIMERSSEC_FLAG_WDAY_MASK 0x0E

/** Maska flag konfiguracyjnych.
*/
#define TIMERSSEC_FLAG_CONFIG_MASK 0x0F

/** Minimalny czas włączenia/wyłączenia.
*/
#define TIMERSSEC_MIN_SET_TIME 1

/** Maksymalny czas włączenia/wyłączenia.
*/
#define TIMERSSEC_MAX_SET_TIME 99

/** Czas domyślny włączenia/wyłączenia.
*/
#define TIMERSSEC_DEFAULT_TIME 10

/** Liczba tiemrów sekundowych w sterowniku.
*/
#define TIMERSSEC_NUM 4

/** Maksymalna wartość dni tygodnia
*/
#define TIMERSSEC_MAX_WDAY 7

/** Adresacja EEPROM
*/
#define EEPROM_TIMERSEC_WHEN_HOURS_OFFSET 0
#define EEPROM_TIMERSEC_WHEN_MINUTES_OFFSET 1
#define EEPROM_TIMERSEC_DURATION_OFFSET 2
#define EEPROM_TIMERSEC_OUT_OFFSET 3
#define EEPROM_TIMERSEC_FLAGS_OFFSET 4
#define EEPROM_TIMERSEC_SIZE (EEPROM_TIMERSEC_FLAGS_OFFSET + 1)

void timerssec_switch_out (uint8_t,uint8_t);
void timerssec_when_active(void);
void timerssec_when_notactive(void);
void ui_timerssec_settings_display(void);
void save_timersec_settings(uint16_t,uint8_t);
void load_timersec_settings(uint16_t,uint8_t);
void save_timerssec_settings(uint16_t);
void load_timerssec_settings(uint16_t);

#endif

