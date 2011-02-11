	/*
    AquamatControl - Interfejs graficzny do sterownika akwariowego Aquamat
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

#ifndef COMMONDEFS_H
#define COMMONDEFS_H

/** Bit Value
*/
#define _BV(bit) (1 << (bit))

/** Definicja włączenia.
*/
#define ON 1

/** Definicja wyłączenia.
*/
#define OFF 0

/** Definicja wartości zablokowanej / nieaktywnej dla zmiennych 16-bitowych.
*/
#define UINT16_T_DISABLED 0xFFFF

/** Definicja wartości zablokowanej / nieaktywnej dla zmiennych 8-bitowych.
*/
#define UINT8_T_DISABLED 0xFF


#define OUTPUT_ACTIVE_FLAG 7
#define OUTPUT_BLOCK_FLAG 3
#define OUTPUTS_DEVICES_NUM 2
#define OUTPUTS_NUM_PER_DEVICE 8
#define OUTPUTS_NUM (OUTPUTS_DEVICES_NUM * OUTPUTS_NUM_PER_DEVICE)
#define OUTPUTS_NAME_LENGTH 5
#define OUTPUTS_QBUTTONS_MASK 0x07
#define OUTPUTS_QBUTTONS_NUM 6
#define OUTPUTS_NAME_DEF_CHAR 0x3F
#define OUTPUTS_NAME_MIN_CHAR 0x20
#define OUTPUTS_NAME_MAX_CHAR 0x7A

#define ALARMS_TYPES_NUM 5
#define ALARM_FLAG_TEMP_MAX 0
#define ALARM_FLAG_TEMP_MIN 1
#define ALARM_FLAG_TEMP_SENSOR_FAILURE 2
#define ALARM_FLAG_TOP_OFF_MIN_RO 3
#define ALARM_FLAG_TOP_OFF_MAX_RUN 4
#define ALARM_FLAG_BLOCK 7
#define ALARM_FLAG_MASK (_BV(ALARM_FLAG_TEMP_MAX)| _BV(ALARM_FLAG_TEMP_MIN)| _BV(ALARM_FLAG_TEMP_SENSOR_FAILURE)| _BV(ALARM_FLAG_TOP_OFF_MIN_RO)| _BV(ALARM_FLAG_TOP_OFF_MAX_RUN))

#define TEMP_SENSORS_NUM 4
#define TEMP_SENSOR_ROLE_0 0
#define TEMP_SENSOR_ROLE_1 1
#define TEMP_SENSOR_STATUS_OK 2
#define TEMP_SENSOR_CONVERT 3
#define TEMP_SENSOR_HEATING 4
#define TEMP_SENSOR_COOLING 5
#define TEMP_ALARM_MIN 6
#define TEMP_ALARM_MAX 7
#define TEMP_ALARM_MASK (_BV(TEMP_ALARM_MIN)|_BV(TEMP_ALARM_MAX))
#define TEMP_SENSOR_ROLE_MASK (_BV(TEMP_SENSOR_ROLE_0)|_BV(TEMP_SENSOR_ROLE_1))
#define TEMP_SENSOR_ROLE_INACTIVE 3

/** Domyślna temperatura dzienna (23 stopnie)
*/
#define TEMP_DEFAULT_TARGET 0x0170

/** Domyślna temperatura nocna (22 stopnie)
*/
#define TEMP_DEFAULT_TARGET_NIGHT 0x0160

/** Domyślna minimalna temperatura (10 stopni)
*/
#define TEMP_DEFAULT_MIN 0x00A0

/** Domyślna maksymalna temperatura (30 stopni)
*/
#define TEMP_DEFAULT_MAX 0x01E0

/** Domyślna histereza (0.5 stopni)
*/
#define TEMP_DEFAULT_HISTERESIS 0x08

/** Maksymalnie możliwa temperatura (99 stopni)
*/
#define TEMP_MAXIMUM_VALUE 0x0630

/** Maksymalna możliwa histereza (10 stopni)
*/
#define TEMP_MAXIMUM_HISTERESIS 0xA0

#define LOG_EVENT_START 0
#define LOG_EVENT_ALARM_MAX 1
#define LOG_EVENT_ALARM_MIN 2
#define LOG_EVENT_ALARM_SENSOR_FAILURE 3
#define LOG_EVENT_TOP_OFF_MAX_RUN 4
#define LOG_EVENT_TEMP_READ 5
#define LOG_EVENT_OUT_CHANGE 6
#define LOG_EVENT_START_STOP_FLAG 7
#define LOG_EVENT_TOP_OFF_MIN_RO 8

#define LOG_EVENT_DEVICE_MASK 0x0F

#define TIM_STATUS_OK 0
#define TIM_STATUS_NOTACTIVE 1
#define TIM_STATUS_ACTIVE 2
#define TIM_STATUS_BLOCKED 3
#define TIM_STATUS_BAD_OUT 4
#define TIM_STATUS_SAME_SETTINGS 5
#define TIM_STATUS_OUT_BLOCKED 6
#define TIM_STATUS_ALREADY_BUSY 7
#define TIM_STATUS_NULL_SETTINGS 8
#define TIM_STATUS_BAD_TIME_1 9
#define TIM_STATUS_BAD_TIME_2 10
#define TIM_STATUS_NOT_WDAY 11

#define TIMERS_NUM 8
#define TIMERS_FLAG_WDAY_MASK 0x0E
#define TIMERS_FLAG_BLOCKED 0

#define TIMERSV_NUM 8
#define TIMERSV_FLAG_BLOCKED 0

#define TOP_OFF_BLOCK 0
#define TOP_OFF_BLOCK_BY_PIN 1
#define TOP_OFF_BLOCK_MIN_RO 2
#define TOP_OFF_MAX_RUN_REACHED 3
#define TOP_OFF_MAX_SUMP 4
#define TOP_OFF_MIN_SUMP 5
#define TOP_OFF_MIN_RO 6
//#define TOP_OFF_RUNNING 7
#define TOP_OFF_CONFIG_MASK 0x07
#define TOP_OFF_MIN_RUN_TIME 5
#define TOP_OFF_DEFAULT_RUN_TIME 25

#endif

/** @file commonDefs.h
	@brief Plik nagłówkowy - wspólne definicje dla interfejsu i sterownika
*/
