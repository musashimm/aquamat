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

/** @file alarms.c
	@brief Implementacja interfejsu alarmowego.

	Czy dany alarm jest aktywny określa wartość flagi w zmiennej \ref alarms_flags. Jeżeli zmienna \ref alarms_disable_notification_counter jest większa od \ref ALARM_DISABLE_NOTIFICATION_COUNTER_VALUE powiadomienie o alarmach jest odblokowane. Ta funkcjonalność umożliwia tymczasową blokadę powiadomień, np. wyłączając mruganie wyświetlacza, bądź wyłaczenie sygnalizacji dźwiękowej.
*/

#include "aquamat.h"
#include "alarms.h"
#include "commonDefs.h"
#include "temp.h"
#include "usart.h"
#include "strings.h"
#include "log.h"
#include "hd44780.h"

/** @var uint8_t alarms_flags
	Zmienna przechowująca flagi alarmów.
	@verbatim
    76543210   - numer bitu
           X   - ALARM_FLAG_TEMP_MAX
          X    - ALARM_FLAG_TEMP_MIN
         X     - ALARM_FLAG_TEMP_SENSOR_FAILURE
        X      - ALARM_FLAG_MIN_RO
       X       - ALARM_FLAG_TOP_OFF_MAX_RUN
      X        -
     X         -
    X          - ALARM_FLAG_BLOCK
	@endverbatim
*/
uint8_t alarms_flags;

/** @var uint8_t alarms_disable_notification_counter
	Zmienna przechowuje licznik zablokowania powiadomienia o alarmie.
*/
uint8_t alarms_block_notification_counter;

/** Sprawdza czy aktywny jest jakikolwiek alarm.
* @return TRUE - alarm aktywny, FALSE - alarmy nieaktywny
*/
uint8_t alarms_is_active(void) {
	if (alarms_flags & ALARM_FLAG_MASK) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/** Sprawdza czy odblokowane są powiadomienia o alarmach.
* @return TRUE - powiadomiena odblokowane, FALSE - powiadomienia zablokowane
*/
uint8_t alarms_notification_enabled(void) {
	if ((alarms_block_notification_counter >= ALARM_BLOCK_NOTIFICATION_COUNTER_VALUE) && !(AF(ALARM_FLAG_BLOCK)) ) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/** Blokuje czasowo powiadomienia.
*/
void alarms_block_notification(void) {
	alarms_block_notification_counter = FALSE;
}

/** Ustawia bądź czyści alarm. Dodatkowo dokonuje odpowiedniego wpisu w logu systemowym.
	@param flag wskazuje jaki alarm ustawić
	@param new_state określa czy zdarzenie się kończy czy rozpoczyna ON - rozpoczyna, OFF - kończy
	@param event_type typ zdarzenia, jaki trzeba wpisać do logu.
	@param id identyfikator urządzenia wywołujacego alarm
*/
void alarmLog(uint8_t flag,uint8_t new_state, uint8_t event_type, uint8_t id) {
	if (alarms_flags & _BV(flag)) {
		if (!new_state) {
			CAF(flag);
			if (event_type != UINT8_T_DISABLED) {
				log_write_record(event_type,(id & LOG_EVENT_DEVICE_MASK),0,0);
			}
		}
	} else {
		if (new_state) {
			SAF(flag);
			if (event_type != UINT8_T_DISABLED) {
				log_write_record(event_type,(_BV(LOG_EVENT_START_STOP_FLAG) | (id & LOG_EVENT_DEVICE_MASK)),0,0);
			}
		}
	}
}

/** Czyści flagi alarmów
*/
void alarms_clear(void) {
	alarms_flags &= ~ALARM_FLAG_MASK;
}
