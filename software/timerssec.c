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

/** @file timerssec.c
	@brief Implementacja timerów sekundowych.

	Warunkiem włączenia timera jest zgodność czasów: aktualnego i czasu ustawionego w timerze. Warunkiem wyłączenia jest kiedy licznik <em>on</em> osiągnie wartość <em>duration</em>.
*/

#include "aquamat.h"
#include "timerssec.h"
#include "outputs.h"

extern uint8_t wday;

struct TIMERSEC timerssec[TIMERSSEC_NUM];

/** Zwraca status timera.
	@param t numer timera, od 0 do \ref TIMERSSEC_NUM - 1
*/
uint8_t timerssec_status (uint8_t t) {

	if (timerssec[t].flags & _BV(TIMERS_FLAG_BLOCKED)) {
		return TIM_STATUS_BLOCKED;
	}

	if (timerssec[t].out >= OUTPUTS_NUM) {
		return TIM_STATUS_BAD_OUT;
	}

	if (check_is_wday(timerssec[t].flags,TIMERSSEC_FLAG_WDAY_MASK)) {
		return TIM_STATUS_NOT_WDAY;
	}

	if (output_check_flag(timerssec[t].out,OUTPUT_BLOCK_FLAG)) {
		return TIM_STATUS_OUT_BLOCKED;
	}

	if (is_not_valid_MIT(&timerssec[t].when)) {
		return TIM_STATUS_BAD_TIME_1;
	}

	if (timerssec[t].flags & _BV(TIMERSSEC_FLAG_BUSY)) {
		return TIM_STATUS_ALREADY_BUSY;
	}

	if (check_if_its_time(&timerssec[t].when)) {
		return TIM_STATUS_ACTIVE;
	}

	return TIM_STATUS_NOTACTIVE;
}

/** Przełącza skojarzone wyjście.
	@param t numer timera, od 0 do \ref TIMERSSEC_NUM - 1
	@param action \ref ON włączenie, \ref OFF wyłączenie
*/
void timerssec_switch_out (uint8_t t,uint8_t action) {
	if (action) {
		output_switch(timerssec[t].out,ON);
		timerssec[t].flags |= _BV(TIMERSSEC_FLAG_BUSY);
	} else {
		output_switch(timerssec[t].out,OFF);
		timerssec[t].flags &= ~_BV(TIMERSSEC_FLAG_BUSY);
	}
}

/** Sprawdza warunek wyłączenia timera. Procedura wywoływana co sekundę.
*/
void timerssec_when_active(void) {
	uint8_t t;
	for (t=0;t<TIMERSSEC_NUM;t++) {
		if (timerssec_status(t) == TIM_STATUS_ALREADY_BUSY) {
			if (++timerssec[t].on >= timerssec[t].duration) {
				timerssec_switch_out(t,OFF);
			}
		}
	}
}

/** Sprawdza warunek włączenia timera. Procedura wywoływana co minutę.
*/
void timerssec_when_notactive(void) {
	uint8_t t;
	for (t=0;t<TIMERSSEC_NUM;t++) {
		if (timerssec_status(t) == TIM_STATUS_ACTIVE) {
			timerssec[t].on=0;
			timerssec_switch_out(t,ON);
		}
	}
}

