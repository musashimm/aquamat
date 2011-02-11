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

/** @file alarms.h
	@brief Plik nagłówkowy - Implementacja interfejsu alarmowego
*/

#define ALARM_MAX_BLOCK_VALUE 0xFF

/** Po ilu minutach odblokować powiadomienia o alarmach.
*/
#define ALARM_BLOCK_NOTIFICATION_COUNTER_VALUE 5

#define SAF(x) (alarms_flags |= _BV(x))
#define CAF(x) (alarms_flags &= ~_BV(x))
#define AF(x) (alarms_flags & _BV(x))
#define TAF(x) (alarms_flags ^= _BV(x))

uint8_t alarms_is_active(void);
uint8_t alarms_notification_enabled(void);
//void alarm(uint8_t,uint8_t);
void alarmLog(uint8_t flag,uint8_t new_state, uint8_t event_type, uint8_t id);
void alarms_block_notification(void);
void alarms_clear(void);

extern uint8_t alarms_block_notification_counter;
extern uint8_t alarms_flags;
