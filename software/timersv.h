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

/** @file timersv.h
	@brief Plik nagłówkowy - Implementacja timerów interwałowych.
*/

/** @struct TIMERV
	@brief Struktura przechowywująca dane o timerze interwałowym.
*/
struct TIMERV {
        uint8_t off;       /**< Czas włączenia.*/
		uint8_t on;        /**< Czas wyączenia.*/
		uint8_t out;       /**< Przypisane wejscie.*/
		uint8_t counter;   /**< Aktualny licznik minutowy.*/
/** Flagi.
@verbatim
7654|3210
    |   X - blokada timera
    |   0 - odblokowany
    |   1 - zablokowany
   X|     - flaga zajętości timera
   0|     - timer wyłączony
   1|     - timer włączony
  X |     - flaga porównywania licznika
  0 |     - licznik porównywany jest z wartością TIMERV::off
  1 |     - licznik porównywany jest z wartością TIMERV::on
@endverbatim
*/
		uint8_t flags;   
};

/** Flaga blokady.
*/
#define TIMERSV_FLAG_BLOCKED 0

/** Flaga zajetości.
*/
#define TIMERSV_FLAG_BUSY 4

/** Flaga porównania licznika.
*/
#define TIMERSV_FLAG_DIRECTION 5

/** Maska flag konfiguracyjnych.
*/
#define TIMERSV_FLAG_CONFIG_MASK 0x0F

/** Minimalny czas włączenia/wyłączenia.
*/
#define TIMERSV_MIN_SET_TIME 1

/** Maksymalny czas włączenia/wyłączenia.
*/
#define TIMERSV_MAX_SET_TIME 99

/** Czas domyślny włączenia/wyłączenia.
*/
#define TIMERSV_DEFAULT_TIME 5

uint8_t timersv_is_valid_interval(uint8_t);
void timersv_process(void);

extern struct TIMERV timersv[TIMERSV_NUM];
