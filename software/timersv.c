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

/** @file timersv.c
	@brief Implementacja timerów interwałowych.
*/

#include "aquamat.h"
#include "strings.h"
#include "timersv.h"
#include "outputs.h"
#include "usart.h"



struct TIMERV timersv[TIMERSV_NUM];

/** Zwraca status timera interwałowego.
	@param t numer timera, od 0 do \ref TIMERSV_NUM - 1
*/
uint8_t timerv_status (uint8_t t) {
	
	if (timersv[t].flags & _BV(TIMERSV_FLAG_BLOCKED)) {
		return TIM_STATUS_BLOCKED;
	}
	
	if (timersv[t].out == UINT8_T_DISABLED) {
		return TIM_STATUS_BAD_OUT;
	}

	if (!(timersv_is_valid_interval(timersv[t].off))) {
		return TIM_STATUS_BAD_TIME_1;
	}

	if (!(timersv_is_valid_interval(timersv[t].on))) {
		return TIM_STATUS_BAD_TIME_2;
	}

// 	if (output_is_blocked(timersv[t].out)) {
// 		return TIM_STATUS_OUT_BLOCKED;
// 	}
	
// 	if (!(timersv[t].flags & _BV(TIMERSV_FLAG_BUSY)) && output_is_on(timersv[t].out)) {
// 		return TIM_STATUS_ALREADY_BUSY;
// 	}

	return TIM_STATUS_OK;	
}

/** Sprawdza poprawność ustawień interwałów.
	@param interval wartość interwału 
    @return \ref TRUE - wartość prawidłowa, \ref FALSE - wartość nieprawidłowa
*/
uint8_t timersv_is_valid_interval(uint8_t interval) {
	if (interval < TIMERSV_MIN_SET_TIME || interval > TIMERSV_MAX_SET_TIME) {
		return FALSE;
	} else {
		return TRUE;
	}
}

/** Przełącza przypisane wyjście na podstawie stanu licznika.
	@param t numer timera, od 0 do \ref TIMERSV_NUM - 1
*/
void timerv_set_out(uint8_t t) {	
	if(timersv[t].flags & _BV(TIMERSV_FLAG_DIRECTION)) {
		if (++timersv[t].counter > timersv[t].on) {
			output_switch(timersv[t].out,OFF);
			timersv[t].flags &= ~_BV(TIMERSV_FLAG_BUSY);
			timersv[t].flags &= ~_BV(TIMERSV_FLAG_DIRECTION);
			timersv[t].counter=0;
		}
	} else {
		if (++timersv[t].counter > timersv[t].off) {
			output_switch(timersv[t].out,ON);
			timersv[t].flags |= _BV(TIMERSV_FLAG_BUSY);
			timersv[t].flags |= _BV(TIMERSV_FLAG_DIRECTION);
			timersv[t].counter=0;
		}
	}
}

/** Przetwarza po kolei wszystkie timery.
*/
void timersv_process(void) {
	
	uint8_t t;

	for (t=0;t<TIMERSV_NUM;t++) {
		if (!timerv_status(t)) {
			timerv_set_out(t);
		}
	}
}
