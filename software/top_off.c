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

#include "aquamat.h"
#include "commonDefs.h"
#include "top_off.h"
#include "outputs.h"
#include "alarms.h"
#include "log.h"
#include "hd44780.h"
#include "strings.h"
#include "ui.h"

/** @var struct TOP_OFF top_off
	Przechowuje struktur automatycznej dolewki.
*/
struct TOP_OFF top_off;

/** Sprawdza warunki włączenia pompy automatycznej dolewki
*/
void top_off_process() {

	top_off.flags |= TOP_OFF_FLAGS_MASK;

	if (TOP_OFF_MAXSUMP_PORTIN & _BV(TOP_OFF_MAXSUMP_PIN)) {
		TOPCSF(TOP_OFF_MAX_SUMP);
	}

	if (TOP_OFF_MINSUMP_PORTIN & _BV(TOP_OFF_MINSUMP_PIN)) {
		TOPCSF(TOP_OFF_MIN_SUMP);
	}

	if (TOP_OFF_MINRO_PORTIN & _BV(TOP_OFF_MINRO_PIN)) {
		TOPCSF(TOP_OFF_MIN_RO);
	}

	if (TOP_OFF_BLOCK_PORTIN & _BV(TOP_OFF_BLOCK_PIN)) {
		TOPCSF(TOP_OFF_BLOCK_BY_PIN);
	}

	if (TOPSF(TOP_OFF_MIN_RO) && !(TOPSF(TOP_OFF_BLOCK_MIN_RO))) {
        alarmLog(ALARM_FLAG_TOP_OFF_MIN_RO,ON, LOG_EVENT_TOP_OFF_MIN_RO,0);
    } else {
        alarmLog(ALARM_FLAG_TOP_OFF_MIN_RO,OFF, LOG_EVENT_TOP_OFF_MIN_RO,0);
    }

	if (top_off.flags & TOP_OFF_OFF_MASK) {
		top_off_off();
	} else {
        if (TOPSF(TOP_OFF_MIN_SUMP)) {
            top_off_on();
        }
	}
}

/** Sprawdza czy nie został przekroczony czas pracy automatycznej dolewki
*/
void top_off_check_time(void) {
	if (TOP_OFF_IS_RUNNING()) {
		top_off.run_time++;
		if (top_off.run_time >= top_off.max_run) {
			TOPSSF(TOP_OFF_MAX_RUN_REACHED);
			top_off_off();
			alarmLog(ALARM_FLAG_TOP_OFF_MAX_RUN,ON,LOG_EVENT_TOP_OFF_MAX_RUN,0);
		}
	}
}

/** Włącza wyjście skojarzone z automatyczną dolewką
*/
void top_off_on(void) {
	if (TOP_OFF_IS_NOT_RUNNING()) {
		if (output_switch(top_off.out,ON)) {
            top_off.run_time = 0;
		}
	}
}

/** Wyłącza wyjście skojarzone z automatyczną dolewką
*/
void top_off_off(void) {
    if (TOP_OFF_IS_RUNNING()) {
        output_switch(top_off.out,OFF);
    }
}

/** Usuwa bit maksymalnego czasu pracy pompy i usuwa alarm
*/
void top_off_unblock_run(void) {
	TOPCSF(TOP_OFF_MAX_RUN_REACHED);
	alarmLog(ALARM_FLAG_TOP_OFF_MAX_RUN,OFF,LOG_EVENT_TOP_OFF_MAX_RUN,0);
}

/** Inicjalizuje ustawienia dla top_off
*/
void top_off_init(void) {
	TOP_OFF_MAXSUMP_DDR &= ~_BV(TOP_OFF_MAXSUMP_PIN);
	TOP_OFF_MINSUMP_DDR &= ~_BV(TOP_OFF_MINSUMP_PIN);
	TOP_OFF_MINRO_DDR &= ~_BV(TOP_OFF_MINRO_PIN);
	TOP_OFF_BLOCK_DDR &= ~_BV(TOP_OFF_BLOCK_PIN);
	TOP_OFF_MAXSUMP_PORTOUT |= _BV(TOP_OFF_MAXSUMP_PIN);
	TOP_OFF_MINSUMP_PORTOUT |= _BV(TOP_OFF_MINSUMP_PIN);
	TOP_OFF_MINRO_PORTOUT |= _BV(TOP_OFF_MINRO_PIN);
	TOP_OFF_BLOCK_PORTOUT |= _BV(TOP_OFF_BLOCK_PIN);
}

/** Wyświetla stan automatycznej dolewki
*/
void top_off_status_display() {

	hd44780_label(PSTR("AD"),FALSE,FALSE);
	hd44780_switch_state(TOP_OFF_IS_RUNNING(),(top_off.flags & (_BV(TOP_OFF_BLOCK)|_BV(TOP_OFF_BLOCK_BY_PIN))) || (top_off.out >= OUTPUTS_NUM));
	hd44780_switch_state(TOPSF(TOP_OFF_MAX_RUN_REACHED),FALSE);
	hd44780_outstrn_P(SPACE_S);

	hd44780_label(MAX_S,FALSE,FALSE);
	hd44780_switch_state(TOPSF(TOP_OFF_MAX_SUMP),FALSE);
	hd44780_outstrn_P(SPACE_S);

	hd44780_label(MIN_S,FALSE,FALSE);
	hd44780_switch_state(TOPSF(TOP_OFF_MIN_SUMP),FALSE);
	hd44780_outstrn_P(SPACE_S);

	display_break_line(2,0);

	hd44780_label(RO_S,FALSE,FALSE);
	hd44780_switch_state(TOPSF(TOP_OFF_MIN_RO),TOPSF(TOP_OFF_BLOCK_MIN_RO));
	hd44780_outstrn_P(SPACE_S);

	if (top_off.out < OUTPUTS_NUM) {
		hd44780_out8dec(top_off.out+1);
		hd44780_switch_state(!output_check_flag(top_off.out,OUTPUT_ACTIVE_FLAG),output_check_flag(top_off.out,OUTPUT_BLOCK_FLAG));
	} else {
		hd44780_outstrn_P(MINUSMINUSMINUS_S);
		hd44780_outstrn_P(SPACE_S);
	}

	hd44780_outstrn_P(SPACE_S);

	if (top_off.run_time < UINT8_T_DISABLED) {
		hd44780_out8dec3(top_off.run_time);
	} else {
		hd44780_outstrn_P(MINUSMINUSMINUS_S);
	}
	hd44780_outstrn_P(SLASH_S);

	if (top_off.max_run < UINT8_T_DISABLED) {
		hd44780_out8dec3(top_off.max_run);
	} else {
		hd44780_outstrn_P(MINUSMINUSMINUS_S);
	}
	hd44780_outstrn_P(SPACE_S);
}

/** @file top_off.c
        @brief Implementacja automatycznej dolewki

Automatyczna dolewka domyślnie wykorzystuje 3 czujniki poziomu wody: 2 w
sumpie (\ref top_off_max_sump , \ref top_off_min_sump) i jeden w zbiorniku
na wodę RO (\ref top_off_min_ro ) . Ten ostatni służy do zabezpieczenia
pompy przed pracą na sucho (wystawiony jest alarm), dlatego trzeba
przewidzieć odpowiedni margines ilości wody w zbiorniku RO.

Dodatkowym zabezpieczeniem jest licznik pracy pompy. Przekroczony czas
pompy skutkuje zatrzymaniem pracy pompy (ustawiany jest bit blokady),
niezależnie od stanu czujników. Dodatkowo wystawiany jest alarm. Ponowna
praca pompy możliwa jest po skasowaniu alarmu i blokady pracy pompy.

 @image html top_off.png

 @section topoff_run Algorytm działania automatycznej jest następujący:

@verbatim
TOP_OFF_MAX_SUMP TOP_OFF_MIN_SUMP TOP_OFF_MIN_RO TOP_OFF_BLOCK_MIN_RO TOP_OFF_MAX_RUN_REACHED TOP_OFF_RUNNING
     X                X                 X                  X              aktywny              wy³.(0)
  aktywny             X                 X                  X                X                  wy³.(0)
     X                X              aktywny           niekatywny           X                  alarm
     X             aktywny              X                  X                X                  w³.(1)
@endverbatim

@section topoff_ro Warunki alarmowania dla czujnika wody RO
@verbatim
TOPSF(TOP_OFF_MIN_RO) TOPSF(TOP_OFF_BLOCK_MIN_RO) Akcja
0                     0                           Alarm Wył.
0                     1                           Alarm Wył.
1                     0                           Alarm Wł.
1                     1                           Alarm Wył.
@endverbatim

@section adsensors Podłączenie czujników
Automatyczna dolewka obsługuje 3 czujniki. Podłaczenie czujnika TOP_OFF_MAX_SUMP (NO), TOP_OFF_MIN_SUMP (NC) i TOP_OFF_BLOCK_MIN_RO (NO) pokazano na poniższych
zdjęciach.

@image html jack_ad.jpg

@image html jack_ad2.jpg

*/
