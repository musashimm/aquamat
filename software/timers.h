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

/** @file timers.h
	@brief Plik nagłówkowy - Implementacja timerów.
*/

/** @struct TIMER
	@brief Struktura przechowywująca dane o timerze.
*/
struct TIMER {
        struct MIT from; /**< Czas, od którego timer jest włączony. Struktura \ref MIT "MIT".*/
        struct MIT to;   /**< Czas, do którego timer jest włączony. Struktura \ref MIT "MIT".*/
        uint8_t out;     /**< Przypisane wejscie.*/
/** Flagi.
	@verbatim
7654|3210
    |   X - blokada timera
    |   0 - odblokowany
    |   1 - zablokowany
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
#define TIMERS_FLAG_BLOCKED 0

/** Flaga zajetości.
*/
#define TIMERS_FLAG_BUSY 4

/** Maska flag konfiguracyjnych.
*/
#define TIMERS_FLAG_CONFIG_MASK 0x0F

/** Maska dni tygodnia
*/
#define TIMERS_FLAG_WDAY_MASK 0x0E

/** Liczba minut w godzinie
*/
#define TIMERS_HOUR_MINUTES 60

/** Maksymalna wartość dla WDAY
*/
#define TIMERS_MAX_WDAY 7

uint8_t just_in_time(struct MIT*,struct MIT*);
void timers_process(void);

extern struct TIMER timers[TIMERS_NUM];

