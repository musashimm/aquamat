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

/** @file timers.c
	@brief Implementacja timerów.
*/

#include "aquamat.h"
#include "timers.h"
#include "outputs.h"
#include "strings.h"
#include "usart.h"

extern uint8_t minutes;
extern uint8_t hours;
extern uint8_t wday;

struct TIMER timers[TIMERS_NUM];


/** Zwraca status timera.
	@param t numer timera, od 0 do \ref TIMERSV_NUM - 1
*/
uint8_t timer_status (uint8_t t) {

	if (timers[t].flags & _BV(TIMERS_FLAG_BLOCKED)) {
		return TIM_STATUS_BLOCKED;
	}

	if (timers[t].out >= OUTPUTS_NUM) {
		return TIM_STATUS_BAD_OUT;
	}

	//if (output_is_blocked(timers[t].out)) {
	//	return TIM_STATUS_OUT_BLOCKED;
	//}

// 	if (!(timers[t].flags & _BV(TIMERS_FLAG_BUSY)) && output_is_on(timers[t].out)) {
// 		return TIM_STATUS_ALREADY_BUSY;
// 	}
	
	/*if (is_not_valid_MIT(&timers[t].from) || is_not_valid_MIT(&timers[t].to) ) {
		return TIM_STATUS_BAD_TIME_1;
	}*/

	/* if (timers[t].from.hours > TIMERS_MAX_HOURS || timers[t].to.hours > TIMERS_MAX_HOURS || timers[t].from.minutes > TIMERS_MAX_MINUTES || timers[t].to.minutes > TIMERS_MAX_MINUTES) {
		return TIM_STATUS_BAD_TIME_1;
	} */

	if ((timers[t].flags & TIMERS_FLAG_WDAY_MASK) != wday && (timers[t].flags & TIMERS_FLAG_WDAY_MASK) != TIMERS_FLAG_WDAY_MASK ) {
		return TIM_STATUS_NOT_WDAY;
	}

	if (just_in_time(&timers[t].from,&timers[t].to)) {
		return TIM_STATUS_ACTIVE;
	} else {
		return TIM_STATUS_NOTACTIVE;
	}
}

/** Przełącza skojarzone wyjście.
	@param t numer timera, od 0 do \ref TIMERS_NUM - 1
	@param action \ref ON włączenie, \ref OFF wyłączenie
*/
void timer_switch_out (uint8_t t,uint8_t action) {
	if (action) {
		output_switch(timers[t].out,ON);
		timers[t].flags |= _BV(TIMERS_FLAG_BUSY);
	} else {
		output_switch(timers[t].out,OFF);
		timers[t].flags &= ~_BV(TIMERS_FLAG_BUSY);
	}
}

/** Przetwarza po kolei wszystkie timery.
*/
void timers_process(void) {
	uint8_t t;
	for (t=0;t<TIMERS_NUM;t++) {
		if (timer_status(t) == TIM_STATUS_ACTIVE) {
			timer_switch_out(t,ON);
		} else if (timer_status(t) == TIM_STATUS_NOTACTIVE || timer_status(t) == TIM_STATUS_NOT_WDAY || timer_status(t) == TIM_STATUS_BLOCKED) {
			timer_switch_out(t,OFF);
		}
	}
}

// Wyświetla status timera na terminalu.
//	@param t numer timera, od 0 do \ref TIMERS_NUM - 1
//*/
// void timer_show_status(uint8_t t) {
// 	PGM_P p;
// 	usart_outstrn_P(TIMER_S);
// 	usart_out4dec(t+1);
// 	usart_outstrn_P(N_S);
// 	usart_outstrn_P_par(FROM_S);
// 	usart_format_time(timers[t].from);
// 	usart_outstrn_P(SPACE_S);
// 	usart_outstrn_P(TO_S);
// 	usart_format_time(timers[t].to);
// 	usart_outstrn_P(N_S);
// 	usart_outstrn_P_par(WDAY_S);
// 	memcpy_P(&p, &RTC_WDAYS_STRS[(timers[t].flags & TIMERS_FLAG_WDAY_MASK) >> 1], sizeof(PGM_P));
// 	usart_outstrn_P(p);
// 	usart_outstrn_P(N_S);
// 	usart_outstrn_P_par(OUT_S);
// 	//output_show_status(timers[t].out);
// 	usart_outstrn_P(N_S);
// 	usart_outstrn_P_par(STATUS_S);
// 	tim_status(timer_status(t));
// 	usart_outstrn_P(N_S);
// }

/** Sprawdza warunek włączenia timera.
	@param from czas początkowy w formacie \ref MIT "MIT"
	@param to czas końcowy w formacie \ref MIT "MIT"
    @return  \ref TRUE - timer powinien być włączony, \ref FALSE - timer powinien być wyłączony
*/
uint8_t just_in_time(struct MIT *from,struct MIT *to) {	
	
	uint16_t mincur=hours * TIMERS_HOUR_MINUTES + minutes;
    uint16_t minfrom=from->hours * TIMERS_HOUR_MINUTES + from->minutes;
    uint16_t minto=to->hours * TIMERS_HOUR_MINUTES + to->minutes;

	if (minto > minfrom) {
		if ((mincur >= minfrom) && (mincur < minto)) {
			return TRUE;
		} else {
			return FALSE;
		}
	} else if (minto < minfrom) {
		if ((mincur >= minfrom) || (mincur < minto)) {
			return TRUE;
		} else {
			return FALSE;
		}
	} else {
		if (minto == mincur) {
			return TRUE;
		} else {
			return FALSE;
		}
	}
}



