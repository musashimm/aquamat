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

/** @file eeprom.h
	@brief Plik nagłówkowy - Obsługa pamięci EEPROM.
*/

#define 	EEMEM   __attribute__((section(".eeprom")))

#define EEPROM_TEMP_SETTINGS_BEGIN 10

#define EEPROM_TEMP_ROM_OFFSET 0
#define EEPROM_TEMP_TARGET_OFFSET (EEPROM_TEMP_ROM_OFFSET + 8)
#define EEPROM_TEMP_TARGET_NIGHT_OFFSET (EEPROM_TEMP_TARGET_OFFSET + 2)
#define EEPROM_TEMP_MAX (EEPROM_TEMP_TARGET_NIGHT_OFFSET + 2)
#define EEPROM_TEMP_MIN (EEPROM_TEMP_MAX + 2)
#define EEPROM_TEMP_HISTERESIS (EEPROM_TEMP_MIN + 2)
#define EEPROM_TEMP_FLAGS (EEPROM_TEMP_HISTERESIS + 1)
#define EEPROM_TEMP_OUT_HEATING (EEPROM_TEMP_FLAGS + 1)
#define EEPROM_TEMP_OUT_COOLING (EEPROM_TEMP_OUT_HEATING + 1)
#define EEPROM_TEMP_SIZE (EEPROM_TEMP_OUT_COOLING + 1)

#define EEPROM_OUTS_SETTING_BEGIN ((EEPROM_TEMP_SIZE*TEMP_SENSORS_NUM)+EEPROM_TEMP_SETTINGS_BEGIN)
#define EEPROM_OUTS_SETS_OFF 0
#define EEPROM_OUTS_NAME_OFF 1
#define EEPROM_OUTS_NAME_LENGTH 5
#define EEPROM_OUTS_SIZE (EEPROM_OUTS_NAME_OFF + EEPROM_OUTS_NAME_LENGTH)

#define EEPROM_TIMERS_SETTING_BEGIN ((OUTPUTS_NUM * EEPROM_OUTS_SIZE) + EEPROM_OUTS_SETTING_BEGIN)
#define EEPROM_TIMERS_FROM_HOURS_OFFSET 0
#define EEPROM_TIMERS_FROM_MINUTES_OFFSET 1
#define EEPROM_TIMERS_TO_HOURS_OFFSET 2
#define EEPROM_TIMERS_TO_MINUTES_OFFSET 3
#define EEPROM_TIMERS_OUT_OFFSET 4
#define EEPROM_TIMERS_FLAGS_OFFSET 5
#define EEPROM_TIMERS_SIZE (EEPROM_TIMERS_FLAGS_OFFSET + 1)

#define EEPROM_TIMERSV_SETTING_BEGIN (EEPROM_TIMERS_SETTING_BEGIN + EEPROM_TIMERS_SIZE * TIMERS_NUM)
#define EEPROM_TIMERSV_OFF_OFFSET 0
#define EEPROM_TIMERSV_ON_OFFSET 1
#define EEPROM_TIMERSV_OUT_OFFSET 2
#define EEPROM_TIMERSV_FLAGS_OFFSET 3
#define EEPROM_TIMERSV_SIZE (EEPROM_TIMERSV_FLAGS_OFFSET + 1)

#define EEPROM_BASIC_SETTINGS_BEGIN (EEPROM_TIMERSV_SETTING_BEGIN + EEPROM_TIMERSV_SIZE * TIMERSV_NUM)
#define EEPROM_BASIC_SETTINGS_DTF_H_O 0
#define EEPROM_BASIC_SETTINGS_DTF_M_O 1
#define EEPROM_BASIC_SETTINGS_DTT_H_O 2
#define EEPROM_BASIC_SETTINGS_DTT_M_O 3
#define EEPROM_BASIC_SIZE (EEPROM_BASIC_SETTINGS_DTT_M_O + 1)

#define EEPROM_TOPOFF_SETTINGS_BEGIN (EEPROM_BASIC_SETTINGS_BEGIN + EEPROM_BASIC_SIZE)
#define EEPROM_TOPOFF_FLAGS_OFFSET 0
#define EEPROM_TOPOFF_OUT_OFFSET 1
#define EEPROM_TOPOFF_MAX_RUN_OFFSET 2
#define EEPROM_TOPOFF_SIZE 3

void eeprom_write_byte(uint16_t,uint8_t);
uint8_t eeprom_read_byte(uint16_t);
void eeprom_write_word(uint16_t,uint16_t);
uint16_t eeprom_read_word(uint16_t);
void eeprom_read_block(uint16_t,uint8_t*,uint8_t);
void eeprom_write_block(uint16_t,uint8_t*,uint8_t);

void load_temp_sensor_settings(uint16_t,uint8_t);
void save_temp_sensor_settings(uint16_t,uint8_t);
void load_temp_sensors_settings (uint16_t);
void save_temp_sensors_settings (uint16_t);

void load_topoff_settings (uint16_t);
void save_topoff_settings (uint16_t);

void load_outputs_settings (uint16_t);
void save_outputs_settings (uint16_t);
void save_output_settings(uint16_t,uint8_t);

void load_basic_settings (uint16_t);
void save_basic_settings (uint16_t);

void load_timers_settings (uint16_t);
void save_timers_settings (uint16_t);
void save_timer_settings(uint16_t, uint8_t);
void save_timerv_settings(uint16_t, uint8_t);
void load_timersv_settings (uint16_t);
void save_timersv_settings (uint16_t);

void load_output_name(uint16_t ,uint8_t);
void save_output_name(uint16_t ,uint8_t);

extern char output_name[EEPROM_OUTS_NAME_LENGTH+1];
