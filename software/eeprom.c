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

/** @file eeprom.c
	@brief Obsługa pamięci EEPROM.
*/

#include "aquamat.h"
#include "eeprom.h"
#include "temp.h"
#include "outputs.h"
#include "timers.h"
#include "timersv.h"
#include "top_off.h"
#include "hd44780.h"

char output_name[EEPROM_OUTS_NAME_LENGTH+1];

void eeprom_write_byte(uint16_t addr,uint8_t byte) {
	while(EECR & _BV(EEWE));
	EEAR = addr;
	EEDR = byte;
	EECR |= _BV(EEMWE);
	EECR |= _BV(EEWE);
}

uint8_t eeprom_read_byte(uint16_t addr) {
	while(EECR & _BV(EEWE));
	EEAR = addr;
	EECR |= _BV(EERE);
	return EEDR;
}

void eeprom_write_word(uint16_t addr,uint16_t word) {
	eeprom_write_byte(addr,word);
	eeprom_write_byte(addr+1,word>>8);
}

uint16_t eeprom_read_word(uint16_t addr) {
	uint16_t word;
	word = (int16_t)eeprom_read_byte(addr);
	word |= ((int16_t)eeprom_read_byte(addr+1)) << 8;
	return word;
}

void eeprom_read_block(uint16_t addr,uint8_t *dst,uint8_t n) {
	uint8_t i;
	for (i=0;i<n;i++) {
		*dst++ = eeprom_read_byte(addr+i);
	}
}

void eeprom_write_block(uint16_t addr,uint8_t *src,uint8_t n) {
	uint8_t i;
	for (i=0;i<n;i++) {
		eeprom_write_byte(addr+i,*src++);
	}
}

void load_temp_sensor_settings(uint16_t addr,uint8_t sens) {
	eeprom_read_block(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_ROM_OFFSET,&(temp_sensors[sens].rom[0]),8);
 	temp_sensors[sens].target = eeprom_read_word(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_TARGET_OFFSET);
	if (temp_sensors[sens].target > TEMP_MAXIMUM_VALUE) {
		temp_sensors[sens].target = TEMP_DEFAULT_TARGET;
	}
 	temp_sensors[sens].target_night = eeprom_read_word(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_TARGET_NIGHT_OFFSET);
	if (temp_sensors[sens].target_night > TEMP_MAXIMUM_VALUE) {
		temp_sensors[sens].target_night = TEMP_DEFAULT_TARGET_NIGHT;
	}
 	temp_sensors[sens].max = eeprom_read_word(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_MAX);
	if (temp_sensors[sens].max > TEMP_MAXIMUM_VALUE) {
		temp_sensors[sens].max = TEMP_DEFAULT_MAX;
	}
 	temp_sensors[sens].min = eeprom_read_word(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_MIN);
	if (temp_sensors[sens].min > TEMP_MAXIMUM_VALUE) {
		temp_sensors[sens].min = TEMP_DEFAULT_MIN;
	}
	temp_sensors[sens].histeresis = eeprom_read_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_HISTERESIS);
	if (temp_sensors[sens].histeresis > TEMP_MAXIMUM_HISTERESIS) {
		temp_sensors[sens].histeresis = TEMP_DEFAULT_HISTERESIS;
	}
	temp_sensors[sens].flags = eeprom_read_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_FLAGS) & TEMP_SENSOR_ROLE_MASK;
	outputs_assign(&temp_sensors[sens].out_heating,eeprom_read_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_OUT_HEATING));
	outputs_assign(&temp_sensors[sens].out_cooling,eeprom_read_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_OUT_COOLING));
	//temp_sensors[sens].out_heating = eeprom_read_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_OUT_HEATING);
	//temp_sensors[sens].out_cooling = eeprom_read_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_OUT_COOLING);
}
 
void load_temp_sensors_settings (uint16_t addr) {
 	uint8_t i;
 	for (i=0;i<TEMP_SENSORS_NUM;i++) {
 		load_temp_sensor_settings(addr,i);
 	}
}

void save_temp_sensor_settings(uint16_t addr,uint8_t sens) {
	eeprom_write_block(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_ROM_OFFSET,&(temp_sensors[sens].rom[0]),8);
 	eeprom_write_word(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_TARGET_OFFSET,temp_sensors[sens].target);
	eeprom_write_word(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_TARGET_NIGHT_OFFSET,temp_sensors[sens].target_night);
	eeprom_write_word(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_MAX,temp_sensors[sens].max);
	eeprom_write_word(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_MIN,temp_sensors[sens].min);
	eeprom_write_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_HISTERESIS,temp_sensors[sens].histeresis);
	eeprom_write_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_FLAGS,temp_sensors[sens].flags & TEMP_SENSOR_ROLE_MASK);
	eeprom_write_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_OUT_HEATING,temp_sensors[sens].out_heating);
	eeprom_write_byte(sens*EEPROM_TEMP_SIZE+addr+EEPROM_TEMP_OUT_COOLING,temp_sensors[sens].out_cooling);
}

void save_temp_sensors_settings (uint16_t addr) {
 	uint8_t i;
 	for (i=0;i<TEMP_SENSORS_NUM;i++) {
 		save_temp_sensor_settings(addr,i);
 	}
}

void load_outputs_settings (uint16_t addr) {
	uint8_t i;
	for (i=0;i<OUTPUTS_NUM;i++) {
		outputs[i].flags = (eeprom_read_byte(i*EEPROM_OUTS_SIZE+addr) & OUTPUT_CONFIG_MASK) | _BV(OUTPUT_ACTIVE_FLAG);
	}
}

void save_output_settings(uint16_t addr,uint8_t id) {
	eeprom_write_byte(id*EEPROM_OUTS_SIZE+addr,((outputs[id].flags & OUTPUT_CONFIG_MASK) | _BV(OUTPUT_ACTIVE_FLAG)));
}

void save_outputs_settings (uint16_t addr) {
	uint8_t i;
	for (i=0;i<OUTPUTS_NUM;i++) {
		save_output_settings(addr,i);
	}
}

void save_timer_settings(uint16_t base_addr, uint8_t t) {
	uint16_t addr = t*EEPROM_TIMERS_SIZE+base_addr;
	eeprom_write_byte(addr+EEPROM_TIMERS_FROM_HOURS_OFFSET,timers[t].from.hours);
	eeprom_write_byte(addr+EEPROM_TIMERS_FROM_MINUTES_OFFSET,timers[t].from.minutes);
	eeprom_write_byte(addr+EEPROM_TIMERS_TO_HOURS_OFFSET,timers[t].to.hours);
	eeprom_write_byte(addr+EEPROM_TIMERS_TO_MINUTES_OFFSET,timers[t].to.minutes);
	eeprom_write_byte(addr+EEPROM_TIMERS_OUT_OFFSET,timers[t].out);
	eeprom_write_byte(addr+EEPROM_TIMERS_FLAGS_OFFSET,timers[t].flags & TIMERS_FLAG_CONFIG_MASK);
}

void load_timer_settings(uint16_t addr, uint8_t t) {
	set_MIT(&timers[t].from,eeprom_read_word(addr+EEPROM_TIMERS_FROM_HOURS_OFFSET),eeprom_read_word(addr+EEPROM_TIMERS_FROM_MINUTES_OFFSET),MIT_DEFAULT_HOURS,MIT_DEFAULT_MINUTES);
	set_MIT(&timers[t].to,eeprom_read_word(addr+EEPROM_TIMERS_TO_HOURS_OFFSET),eeprom_read_word(addr+EEPROM_TIMERS_TO_MINUTES_OFFSET),MIT_DEFAULT_HOURS,MIT_DEFAULT_MINUTES);

	//timers[t].from.hours = eeprom_read_word(addr+EEPROM_TIMERS_FROM_HOURS_OFFSET);
	//timers[t].from.minutes = eeprom_read_word(addr+EEPROM_TIMERS_FROM_MINUTES_OFFSET);
	//timers[t].to.hours = eeprom_read_word(addr+EEPROM_TIMERS_TO_HOURS_OFFSET);
	//timers[t].to.minutes = eeprom_read_word(addr+EEPROM_TIMERS_TO_MINUTES_OFFSET);

	outputs_assign(&timers[t].out,eeprom_read_word(addr+EEPROM_TIMERS_OUT_OFFSET));
	//timers[t].out = eeprom_read_word(addr+EEPROM_TIMERS_OUT_OFFSET);

	
	timers[t].flags = eeprom_read_word(addr+EEPROM_TIMERS_FLAGS_OFFSET);
}

void save_timerv_settings(uint16_t base_addr, uint8_t t) {
	uint16_t addr = t*EEPROM_TIMERSV_SIZE+base_addr;
	eeprom_write_byte(addr+EEPROM_TIMERSV_OFF_OFFSET,timersv[t].off);
	eeprom_write_byte(addr+EEPROM_TIMERSV_ON_OFFSET,timersv[t].on);
	eeprom_write_byte(addr+EEPROM_TIMERSV_OUT_OFFSET,timersv[t].out);
	eeprom_write_byte(addr+EEPROM_TIMERSV_FLAGS_OFFSET,timersv[t].flags & TIMERSV_FLAG_CONFIG_MASK);
}

void load_timerv_settings(uint16_t addr, uint8_t t) {
	timersv[t].off = eeprom_read_byte(addr+EEPROM_TIMERSV_OFF_OFFSET);
	if (timersv[t].off > TIMERSV_MAX_SET_TIME || timersv[t].off < TIMERSV_MIN_SET_TIME) {
		timersv[t].off = TIMERSV_DEFAULT_TIME;
	}
	timersv[t].on = eeprom_read_byte(addr+EEPROM_TIMERSV_ON_OFFSET);
	if (timersv[t].on > TIMERSV_MAX_SET_TIME || timersv[t].on < TIMERSV_MIN_SET_TIME) {
		timersv[t].on = TIMERSV_DEFAULT_TIME;
	}
	outputs_assign(&timersv[t].out,eeprom_read_byte(addr+EEPROM_TIMERSV_OUT_OFFSET));
	//timersv[t].out = eeprom_read_byte(addr+EEPROM_TIMERSV_OUT_OFFSET);
	timersv[t].flags = eeprom_read_byte(addr+EEPROM_TIMERSV_FLAGS_OFFSET) & TIMERS_FLAG_CONFIG_MASK;
}

void load_timers_settings (uint16_t addr) {
	uint8_t i;
 	for (i=0;i<TIMERS_NUM;i++) {
 		load_timer_settings(i*EEPROM_TIMERS_SIZE+addr,i);
 	}
}

void save_timers_settings (uint16_t base_addr) {
	uint8_t i;
 	for (i=0;i<TIMERS_NUM;i++) {
 		save_timer_settings(base_addr,i);
 	}
}

void load_timersv_settings (uint16_t addr) {
	uint8_t i;
 	for (i=0;i<TIMERSV_NUM;i++) {
 		load_timerv_settings(i*EEPROM_TIMERSV_SIZE+addr,i);
 	}
}

void save_timersv_settings (uint16_t base_addr) {
	uint8_t i;
 	for (i=0;i<TIMERSV_NUM;i++) {
 		save_timerv_settings(base_addr,i);
 	}
}

void load_basic_settings(uint16_t addr) {
	set_MIT(&daytime_from,eeprom_read_byte(addr+EEPROM_BASIC_SETTINGS_DTF_H_O),eeprom_read_word(addr+EEPROM_BASIC_SETTINGS_DTF_M_O),DAYTIME_FROM_DEFAULT_HOURS,DAYTIME_FROM_DEFAULT_MINUTES);
	set_MIT(&daytime_to,eeprom_read_word(addr+EEPROM_BASIC_SETTINGS_DTT_H_O),eeprom_read_word(addr+EEPROM_BASIC_SETTINGS_DTT_M_O),DAYTIME_TO_DEFAULT_HOURS,DAYTIME_TO_DEFAULT_MINUTES);
	//daytime_from.hours = eeprom_read_byte(addr+EEPROM_BASIC_SETTINGS_DTF_H_O);
	//daytime_from.minutes = eeprom_read_word(addr+EEPROM_BASIC_SETTINGS_DTF_M_O);
	//daytime_to.hours = eeprom_read_word(addr+EEPROM_BASIC_SETTINGS_DTT_H_O);
	//daytime_to.minutes = eeprom_read_word(addr+EEPROM_BASIC_SETTINGS_DTT_M_O);
}

void save_basic_settings(uint16_t addr) {
	eeprom_write_byte(addr+EEPROM_BASIC_SETTINGS_DTF_H_O,daytime_from.hours);
	eeprom_write_byte(addr+EEPROM_BASIC_SETTINGS_DTF_M_O,daytime_from.minutes);
	eeprom_write_byte(addr+EEPROM_BASIC_SETTINGS_DTT_H_O,daytime_to.hours);
	eeprom_write_byte(addr+EEPROM_BASIC_SETTINGS_DTT_M_O,daytime_to.minutes);
}

void load_output_name(uint16_t addr,uint8_t o) {
	uint8_t i,c;
	if (o < OUTPUTS_NUM) {
		for (i=0;i<EEPROM_OUTS_NAME_LENGTH;i++) {
			c = eeprom_read_byte(o * EEPROM_OUTS_SIZE + i + EEPROM_OUTS_NAME_OFF + addr);
			if (c < OUTPUTS_NAME_MIN_CHAR || c > OUTPUTS_NAME_MAX_CHAR ) {
				output_name[i] = OUTPUTS_NAME_DEF_CHAR;
			} else {
				output_name[i] = c;
			}
		}
	}
}

void save_output_name(uint16_t addr,uint8_t o) {
	uint8_t i;
	for (i=0;i<EEPROM_OUTS_NAME_LENGTH;i++) {
 		eeprom_write_byte(o * EEPROM_OUTS_SIZE + i + EEPROM_OUTS_NAME_OFF + addr,output_name[i]);
 	}
}

void load_topoff_settings (uint16_t addr) {
	top_off.flags = eeprom_read_byte(addr + EEPROM_TOPOFF_FLAGS_OFFSET) & TOP_OFF_CONFIG_MASK;
	outputs_assign(&top_off.out,eeprom_read_word(addr + EEPROM_TOPOFF_OUT_OFFSET));
	//top_off.out = eeprom_read_word(addr + EEPROM_TOPOFF_OUT_OFFSET);
	top_off.max_run = eeprom_read_word(addr + EEPROM_TOPOFF_MAX_RUN_OFFSET);
	if (top_off.max_run == UINT8_T_DISABLED) {
		top_off.max_run = TOP_OFF_DEFAULT_RUN_TIME;
	}
}

void save_topoff_settings (uint16_t addr) {
	eeprom_write_byte(addr + EEPROM_TOPOFF_FLAGS_OFFSET, top_off.flags & TOP_OFF_CONFIG_MASK);
	eeprom_write_byte(addr + EEPROM_TOPOFF_OUT_OFFSET, top_off.out);
	eeprom_write_byte(addr + EEPROM_TOPOFF_MAX_RUN_OFFSET, top_off.max_run);
}
