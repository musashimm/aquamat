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

/** @file time.c
	@brief Funkcje związane z obsługą czasu.
*/

#include "aquamat.h"
#include "time.h"

extern uint8_t wday;
extern uint8_t hours;
extern uint8_t minutes;

/** Sprawdza warunek dla dnia tygodnia.
	@param flags miejsce przechowywania konfiguracji dni tygodnia
	@param mask maska bitowa dla dni tygodnia
*/
uint8_t check_is_wday(uint8_t flags,uint8_t mask) {
	if ((flags & mask) >> 1 == wday ||	// warunek dla dnia tygodnia !!! Uwaga przesunięcie bez parametryzacji
		(flags & mask) == mask ) {	// warunek dla każdego dnia !!! konfiguracja dnia musi być w określonym miejscu
		return TRUE;
	}
	return FALSE;
}

/** Porównuje strukturę MIT z aktualnym czasem.
	@param *time wskażnik do struktury MIT
*/
uint8_t check_if_its_time(struct MIT *time) {
	if (time->hours == hours && time->minutes == minutes) {
		return TRUE;
	}
	return FALSE;
}

/** Ustawia strukturę MIT.
	@param *mit wskaźnik do struktury
	@param hours wartość godziny
	@param minutes wartość minut
	@param default_hours wartość godziny
	@param default_minutes wartość minut
*/
void set_MIT(struct MIT *mit,uint8_t hours,uint8_t minutes,uint8_t default_hours,uint8_t default_minutes) {

	if (hours > MIT_MAX_HOURS) {
		mit->hours = default_hours;
	} else {
		mit->hours = hours;
	}

	if (minutes > MIT_MAX_MINUTES) {
		mit->minutes = default_minutes;
	} else {
		mit->minutes = minutes;
	}
}

/** Sprawdza czy struktura MIT zawiera dopuszczalne wartości.
	@param *mit wskaźnik do struktury
    @return \ref TRUE - zawiera niedopuszczalne wartości, \ref FALSE - zawiera dopuszczalne wartości
*/
uint8_t is_not_valid_MIT(struct MIT *mit) {
	if (mit->hours > MIT_MAX_HOURS || mit->minutes > MIT_MAX_MINUTES) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/** Dodaje minuty do struktury \ref MIT.
    @param struct MIT* time wskaźnik na strukturę
    @param uint8_t minutes liczba minut
    @warning Brak sprawdzenia przekroczenia zakresu zmiennych.
*/
void add_minutes (struct MIT* time, uint8_t minutes) {
    time->minutes += minutes;
    while(time->minutes >= 60) {
        time->minutes -= 60;
        if (++time->hours >= 24) {
            time->hours=0;
        }
    }
}
