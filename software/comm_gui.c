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

/** @file comm_gui.c
@brief Implementacja komunikacji z interfejsem graficznym.
*/

#include <avr/wdt.h>
#include "aquamat.h"
#include "usart.h"
#include "comm_gui.h"
#include "commandsDefs.h"
#include "rtc.h"
#include "hd44780.h"
#include "eeprom.h"
#include "outputs.h"
#include "alarms.h"
#include "top_off.h"
#include "temp.h"
#include "timers.h"
#include "timersv.h"
#include "log.h"
#include "pwm.h"

extern uint8_t minutes;
extern uint8_t hours;
extern uint8_t temp;
extern uint8_t year;
extern uint8_t month;
extern uint8_t day;
extern uint8_t wday;

/** @var uint8_t cm_buffer[CM_BUFFER_LENGTH]
	Bufor przechowujący znaki odebrane z USARTa.
*/
uint8_t cm_buffer[CM_BUFFER_LENGTH+1];

/** @var uint8_t pcmbuff
	Wskaźnik bufora znakowego.
*/
uint8_t pcmbuff;

/** @var uint8_t pcmbuff_crc
	Crc bufora
*/
uint8_t pcmbuff_crc;

uint8_t transmit_crc;

/** @var uint8_t pcmbuff_read
	Wskaźnik bufora znakowego do odczytu.
*/
uint8_t pcmbuff_read;

/** Odbiera znaki z interfejsu szeregowego i umieszcza je w buforze
*/
void gui_cm_process_char(void) {
	if (pcmbuff) {
		if (pcmbuff >= CM_BUFFER_LENGTH) {
			gui_cm_return_status(GUI_BUFFER_FULL,cm_buffer[0],cm_buffer[1]);
			pcmbuff=0;
		} else if (cm_buffer[pcmbuff-1] == GUI_END_OF_COMMAND) {
		    //SSF(FLAG_COMMAND_PROCESSING);
			gui_cm_execute();
			pcmbuff=0;
			//CSF(FLAG_COMMAND_PROCESSING);
		}
	}
}

/** Pobiera pierwszy daną z bufora.
*/
uint8_t gui_get_first(void) {
	pcmbuff_read = 2;
	return gui_get_next();
}

/** Ustawia wskaźnik do odczytu na początek danych w buforze.
*/
void gui_buffer_reset(void) {
	pcmbuff_read = 2;
}

/** Pobiera kolejną daną z bufora.
*/
uint8_t gui_get_next(void) {
	uint8_t first;
	if (pcmbuff_read >= pcmbuff) {
		return UINT8_T_DISABLED;
	}
	first = cm_buffer[pcmbuff_read];
	pcmbuff_read++;
	if (first == GUI_EXTEND_BYTE) {
		first = cm_buffer[pcmbuff_read];
		pcmbuff_read++;
		return (first | GUI_EXTEND_BYTE);
	} else {
		return first;
	}
}

void gui_command_start(uint8_t command,uint8_t subcommand) {
	transmit_crc = 0;
	usart_transmit(command);
	transmit_crc = transmit_crc ^ command;
	usart_transmit(subcommand);
	transmit_crc = transmit_crc ^ subcommand;
}

void gui_command_data(uint8_t data) {
	if (data >= GUI_EXTEND_BYTE) {
		usart_transmit(GUI_EXTEND_BYTE);
		transmit_crc = transmit_crc ^ GUI_EXTEND_BYTE;
		usart_transmit(data & 0x7F);
		transmit_crc = transmit_crc ^ (data & 0x7F);
	} else {
		transmit_crc = transmit_crc ^ data;
		usart_transmit(data);
	}
}

void gui_command_end() {
	usart_transmit(transmit_crc);
	usart_transmit(GUI_END_OF_COMMAND);
}

/** Zwraca błąd aplikacji zarządzjącej.
*/
void gui_cm_return_status(uint8_t status,uint8_t command,uint8_t subcommand) {
  	gui_command_start(GUI_STATUS_COMMAND,status);
	gui_command_data(command);
	gui_command_data(subcommand);
	gui_command_end();
}

void gui_cm_return_status_unit(uint8_t status,uint8_t command,uint8_t subcommand,uint8_t unit) {
  	gui_command_start(GUI_STATUS_COMMAND,status);
	gui_command_data(command);
	gui_command_data(subcommand);
	gui_command_data(unit);
	gui_command_end();
}

uint8_t calculateCrc() {
	uint8_t i,crc;
	crc=0;
	for (i=0;i<pcmbuff-2;i++) {
		crc = crc ^ cm_buffer[i];
	}
	return crc;
}

/** analizuje komendę przysłaną przez aplikację zarządzającą i zwraca stosowną informację.
*/
void gui_cm_execute(void) {
	uint8_t command,subcommand;
	command = cm_buffer[0];
	subcommand = cm_buffer[1];
	if (calculateCrc() != cm_buffer[pcmbuff-2]) {
		gui_cm_return_status(GUI_COMMAND_CKSUM_ERROR,command,subcommand);
		return;
	}
	switch (command) {
	//------------------------------------------------------------------alive
	case GUI_STATUS_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_STATUS: gui_client_present();
				break;
			case GUI_SUBCOMMAND_CLEAR: gui_restart();
				break;
		}
		break;
	//------------------------------------------------------------------datetime
	case GUI_DATETIME_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_SETTINGS: gui_datetime_get_settings();
				break;
		  	case GUI_SUBCOMMAND_SET: gui_datetime_set();
				break;
		}
		break;
	//-------------------------------------------------------------------daytime
	case GUI_DAYTIME_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_SETTINGS: gui_daytime_get_settings();
				break;
			case GUI_SUBCOMMAND_SET: gui_daytime_set();
				break;
		}
		break;
	//------------------------------------------------------------------- outs
	case GUI_OUT_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_STATUS: gui_out_get_status();
				break;
			case GUI_SUBCOMMAND_GET_SETTINGS: gui_out_get_settings();
				break;
			case GUI_SUBCOMMAND_SET: gui_out_set();
				break;
			case GUI_SUBCOMMAND_SET_STATE: gui_out_set_state();
				break;
			case GUI_SUBCOMMAND_SET_BLOCK: gui_out_set_block();
				break;
			case GUI_SUBCOMMAND_SET_STATE_ALL: gui_out_set_state_all();
				break;
			case GUI_SUBCOMMAND_SET_BLOCK_ALL: gui_out_set_block_all();
				break;
		}
		break;
	//------------------------------------------------------------------- pwm
	case GUI_PWM_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_STATUS: pwm_gui_get_status();
				break;
			case GUI_SUBCOMMAND_GET_SETTINGS: pwm_gui_get_settings();
				break;
			case GUI_SUBCOMMAND_SET: pwm_gui_set();
				break;
			case GUI_SUBCOMMAND_SET_STATE: pwm_gui_set_state();
				break;
		}
		break;
	//----------------------------------------------------------------- alarms
	case GUI_ALARM_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_STATUS: gui_alarm_get_status();
				break;
			case GUI_SUBCOMMAND_CLEAR: gui_alarm_clear();
				break;
			case GUI_SUBCOMMAND_SET_BLOCK: gui_alarm_block();
				break;
		}
		break;
	//----------------------------------------------------------------- AD
	case GUI_TOP_OFF_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_STATUS: gui_topoff_get_status();
				break;
			case GUI_SUBCOMMAND_SET: gui_topoff_set();
				break;
			case GUI_SUBCOMMAND_CLEAR: gui_topoff_clear();
				break;
		}
		break;
	//----------------------------------------------------------------- temp
	case GUI_TEMP_COMMAND:
		switch(subcommand) {
			case GUI_SUBCOMMAND_GET_STATUS: gui_temp_get_status();
				break;
			case GUI_SUBCOMMAND_GET_SETTINGS: gui_temp_get_settings();
				break;
			case GUI_SUBCOMMAND_DISCOVER: gui_temp_discover();
				break;
			case GUI_SUBCOMMAND_SET: gui_temp_set();
				break;
		}
		break;
	//--------------------------------------------------------------- timers
	case GUI_TIMERS_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_SETTINGS: gui_timers_get_settings();
				break;
			case GUI_SUBCOMMAND_SET: gui_timers_set();
				break;
		}
		break;
	//--------------------------------------------------------------- timersv
	case GUI_TIMERSV_COMMAND:
		switch(subcommand) {
			case GUI_SUBCOMMAND_GET_SETTINGS: gui_timersv_get_settings();
				break;
			case GUI_SUBCOMMAND_SET: gui_timersv_set();
				break;
		}
	//---------------------------------------------------------------- log
	case GUI_LOG_COMMAND:
		switch (subcommand) {
			case GUI_SUBCOMMAND_GET_STATUS: gui_log_get();
				break;
			case GUI_SUBCOMMAND_CLEAR: gui_log_clear();
				break;
		}
		break;
		//---------------------------------------------------------------default
	default: gui_cm_return_status(GUI_UNKONWN_COMMAND,command,subcommand);
		break;
	}
}

void gui_restart (void) {
		restart();
}

void gui_client_present (void) {
	gui_command_start(GUI_STATUS_COMMAND,GUI_CLIENT_PRESENT);
	gui_command_data(PROGRAM_VERSION_MAIN);
	gui_command_data(PROGRAM_VERSION_MAJOR);
	gui_command_data(PROGRAM_VERSION_MINOR);
	gui_command_end();
}
//==================================================================================== timers
void gui_timers_get_settings(void) {
	uint8_t i;
	for (i=0;i<TIMERS_NUM;i++) {
		gui_command_start(GUI_TIMERS_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE);
		gui_command_data(i);
		gui_command_data(timers[i].from.hours);
		gui_command_data(timers[i].from.minutes);
		gui_command_data(timers[i].to.hours);
		gui_command_data(timers[i].to.minutes);
		gui_command_data(timers[i].out);
		gui_command_data(timers[i].flags);
		gui_command_end();
	}
	gui_cm_return_status(GUI_COMMAND_OK,GUI_TIMERS_COMMAND,GUI_SUBCOMMAND_GET_STATUS);
}

void gui_timers_set(void) {
	int id;
	id = gui_get_first();
	timers[id].from.hours = gui_get_next();
	timers[id].from.minutes = gui_get_next();
	timers[id].to.hours = gui_get_next();
	timers[id].to.minutes = gui_get_next();
	timers[id].out = gui_get_next();
	timers[id].flags = gui_get_next();
	save_timer_settings(EEPROM_TIMERS_SETTING_BEGIN,id);
	gui_cm_return_status_unit(GUI_COMMAND_OK,GUI_TIMERS_COMMAND,GUI_SUBCOMMAND_SET,id);
}
//==================================================================================== timersv
void gui_timersv_get_settings(void) {
	uint8_t i;
	for (i=0;i<TIMERSV_NUM;i++) {
		gui_command_start(GUI_TIMERSV_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE);
		gui_command_data(i);
		gui_command_data(timersv[i].off);
		gui_command_data(timersv[i].on);
		gui_command_data(timersv[i].out);
		gui_command_data(timersv[i].flags);
		gui_command_data(timersv[i].counter);
		gui_command_end();
	}
	gui_cm_return_status(GUI_COMMAND_OK,GUI_TIMERSV_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS);
}

void gui_timersv_set(void) {
	int id;
	id = gui_get_first();
	timersv[id].off = gui_get_next();
	timersv[id].on = gui_get_next();
	timersv[id].out = gui_get_next();
	timersv[id].flags = gui_get_next();
	timersv[id].counter=0;
	save_timerv_settings(EEPROM_TIMERSV_SETTING_BEGIN,id);
	gui_cm_return_status_unit(GUI_COMMAND_OK,GUI_TIMERSV_COMMAND,GUI_SUBCOMMAND_SET,id);
}
//==================================================================================== log
void gui_log_get(void) {
	uint16_t logp,i;
	uint8_t j=0,k;

	logp = log_read_pointer();
	logp--;

	if (logp < LOG_FIRST_RECORD_NUMBER || logp > LOG_LAST_RECORD_NUMBER) {				// log pusty
		gui_cm_return_status(GUI_EMPTY_RES,GUI_LOG_COMMAND,GUI_SUBCOMMAND_GET_STATUS);
		return;
	}

	for (i=1;i<=LOG_LAST_RECORD_NUMBER;i++) {

		if (++j > LOG_RECORD_CLEAR_MAX) {
				j=0;
				wdt_reset();
		}

		if (!logp) {
			logp = LOG_LAST_RECORD_NUMBER;
		}
		log_read_record_at_pointer(logp,&logbuff[0]);
		if (logbuff[0] == UINT8_T_DISABLED) {
			break;
		} else {
			gui_command_start(GUI_LOG_COMMAND,GUI_SUBCOMMAND_GET_STATUS_RESPONSE);
			for(k=0;k<LOG_BYTES_PER_RECORD;k++) {
				gui_command_data(logbuff[k]);
			}
			gui_command_end();
		}
		logp--;
	}
	gui_cm_return_status(GUI_COMMAND_OK,GUI_LOG_COMMAND,GUI_SUBCOMMAND_GET_STATUS);
}

void gui_log_clear(void) {
	log_clear();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_LOG_COMMAND,GUI_SUBCOMMAND_CLEAR);
}
//==================================================================================== Temp
void gui_temp_get_status(void) {
	uint8_t i;
	for (i=0;i<TEMP_SENSORS_NUM;i++) {
		gui_command_start(GUI_TEMP_COMMAND,GUI_SUBCOMMAND_GET_STATUS_RESPONSE);
		gui_command_data(i);
		gui_command_data((uint8_t)(temp_sensors[i].temp >> 8));
		gui_command_data((uint8_t)temp_sensors[i].temp);
		gui_command_data(temp_sensors[i].flags);
		gui_command_data(temp_sensors[i].bad_reads);
		gui_command_end();
	}
	gui_cm_return_status(GUI_COMMAND_OK,GUI_TEMP_COMMAND,GUI_SUBCOMMAND_GET_STATUS);
}

void gui_temp_get_settings(void) {
	uint8_t i;
	for (i=0;i<TEMP_SENSORS_NUM;i++) {
		gui_command_start(GUI_TEMP_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE);
		gui_command_data(i);
		gui_command_data((uint8_t)(temp_sensors[i].temp >> 8));
		gui_command_data((uint8_t)temp_sensors[i].temp);
		gui_command_data((uint8_t)(temp_sensors[i].target >> 8));
		gui_command_data((uint8_t)temp_sensors[i].target);
		gui_command_data((uint8_t)(temp_sensors[i].target_night >> 8));
		gui_command_data((uint8_t)temp_sensors[i].target_night);
		gui_command_data((uint8_t)(temp_sensors[i].max >> 8));
		gui_command_data((uint8_t)temp_sensors[i].max);
		gui_command_data((uint8_t)(temp_sensors[i].min >> 8));
		gui_command_data((uint8_t)temp_sensors[i].min);
		gui_command_data((uint8_t)temp_sensors[i].histeresis);
		gui_command_data(temp_sensors[i].flags);
		gui_command_data(temp_sensors[i].bad_reads);
		gui_command_data(temp_sensors[i].out_cooling);
		gui_command_data(temp_sensors[i].out_heating);
		gui_command_end();
		gui_temp_rom(i);
	}
	gui_cm_return_status(GUI_COMMAND_OK,GUI_TEMP_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS);
}

void gui_temp_rom(uint8_t id) {
	uint8_t j;
	gui_command_start(GUI_TEMP_COMMAND,GUI_SUBCOMMAND_DISCOVER_RESPONSE);
	gui_command_data(id);
	for (j=0;j<8;j++) {
		gui_command_data(temp_sensors[id].rom[j]);
	}
	gui_command_end();
}

void gui_temp_discover(void) {
	uint8_t i;
	temp_discover_sensors();
	for (i=0;i<TEMP_SENSORS_NUM;i++) {
		gui_temp_rom(i);
	}
	gui_cm_return_status(GUI_COMMAND_OK,GUI_TEMP_COMMAND,GUI_SUBCOMMAND_DISCOVER);
}

void gui_temp_set(void) {
	int id,low,high;
	id = gui_get_first();

    hd44780_out8hex(id);

	high = gui_get_next();
	low = gui_get_next();
	high = ((uint16_t)high) << 8 | low;
	temp_sensors[id].target = high;

	high = gui_get_next();
	low = gui_get_next();
	high = ((uint16_t)high) << 8 | low;
	temp_sensors[id].target_night = high;

	high = gui_get_next();
	low = gui_get_next();
	high = ((uint16_t)high) << 8 | low;
	temp_sensors[id].max = high;

	high = gui_get_next();
	low = gui_get_next();
	high = ((uint16_t)high) << 8 | low;
	temp_sensors[id].min = high;

	temp_sensors[id].histeresis = gui_get_next();

	temp_sensors[id].flags = (temp_sensors[id].flags & ~TEMP_SENSOR_ROLE_MASK) | (gui_get_next() & TEMP_SENSOR_ROLE_MASK);

	temp_sensors[id].out_cooling = gui_get_next();

	temp_sensors[id].out_heating = gui_get_next();

	save_temp_sensor_settings(EEPROM_TEMP_SETTINGS_BEGIN,id);

	gui_cm_return_status_unit(GUI_COMMAND_OK,GUI_TEMP_COMMAND,GUI_SUBCOMMAND_SET,id);
}
//==================================================================================== AD
void gui_topoff_get_status() {
	gui_command_start(GUI_TOP_OFF_COMMAND,GUI_SUBCOMMAND_GET_STATUS_RESPONSE);
	gui_command_data(top_off.flags);
	gui_command_data(top_off.out);
	gui_command_data(top_off.max_run);
	gui_command_end();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_TOP_OFF_COMMAND,GUI_SUBCOMMAND_GET_STATUS);
}

void gui_topoff_set() {
	uint8_t flags = gui_get_first();
	if (flags & _BV(TOP_OFF_BLOCK)) {
		top_off.flags |= _BV(TOP_OFF_BLOCK);
	} else {
		top_off.flags &= ~_BV(TOP_OFF_BLOCK);
	}

	if (flags & _BV(TOP_OFF_BLOCK_MIN_RO)) {
		top_off.flags |= _BV(TOP_OFF_BLOCK_MIN_RO);
	} else {
		top_off.flags &= ~_BV(TOP_OFF_BLOCK_MIN_RO);
	}
	top_off.out = gui_get_next();
	top_off.max_run = gui_get_next();
	save_topoff_settings (EEPROM_TOPOFF_SETTINGS_BEGIN);
	gui_cm_return_status(GUI_COMMAND_OK,GUI_TOP_OFF_COMMAND,GUI_SUBCOMMAND_SET);
}

void gui_topoff_clear() {
	alarmLog(ALARM_FLAG_TOP_OFF_MAX_RUN,OFF,LOG_EVENT_TOP_OFF_MAX_RUN,0);
	alarmLog(ALARM_FLAG_TOP_OFF_MIN_RO,OFF,LOG_EVENT_TOP_OFF_MIN_RO,0);
	top_off_unblock_run();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_TOP_OFF_COMMAND,GUI_SUBCOMMAND_CLEAR);
}
//==================================================================================== alarms

void gui_alarm_get_status(void) {
	gui_command_start(GUI_ALARM_COMMAND,GUI_SUBCOMMAND_GET_STATUS_RESPONSE);
	gui_command_data(alarms_flags);
	gui_command_data(alarms_notification_enabled());
	gui_command_end();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_ALARM_COMMAND,GUI_SUBCOMMAND_GET_STATUS);
}

void gui_alarm_clear(void) {
	alarms_clear();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_ALARM_COMMAND,GUI_SUBCOMMAND_CLEAR);
}

void gui_alarm_block(void) {
	alarms_block_notification();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_ALARM_COMMAND,GUI_SUBCOMMAND_SET_BLOCK);
}
//==================================================================================== outs

void gui_out_get_status(void) {
	uint8_t i;
	gui_command_start(GUI_OUT_COMMAND,GUI_SUBCOMMAND_GET_STATUS_RESPONSE);
	for (i=0;i<OUTPUTS_NUM;i++) {
		gui_command_data(outputs[i].flags);
	}
	gui_command_end();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_OUT_COMMAND,GUI_SUBCOMMAND_GET_STATUS);
}

void gui_out_get_settings(void) {
	uint8_t i,j;
	gui_command_start(GUI_OUT_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE);
	for (i=0;i<OUTPUTS_NUM;i++) {
		gui_command_data(outputs[i].flags);
		load_name(i * EEPROM_OUTS_SIZE + EEPROM_OUTS_NAME_OFF + EEPROM_OUTS_SETTING_BEGIN);
		for(j=0;j<NAME_LENGTH;j++) {
			gui_command_data(name[j]);
		}
	}
	gui_command_end();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_OUT_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS);
}

void gui_out_set(void) {
	uint8_t i,id;
	id = gui_get_first();

	outputs[id].flags &= ~OUTPUTS_QBUTTONS_MASK;
	outputs[id].flags |= gui_get_next();
	save_output_settings(EEPROM_OUTS_SETTING_BEGIN,id);

	for(i=0;i<NAME_LENGTH;i++) {
		name[i] = gui_get_next();
	}
	save_name(id * EEPROM_OUTS_SIZE + EEPROM_OUTS_NAME_OFF + EEPROM_OUTS_SETTING_BEGIN);
	gui_cm_return_status_unit(GUI_COMMAND_OK,GUI_OUT_COMMAND,GUI_SUBCOMMAND_SET,id);
}

void gui_out_set_state(void) {
	uint8_t id,state;
	id = gui_get_first();
	state = gui_get_next();
	output_set(id,state);
	gui_cm_return_status_unit(GUI_COMMAND_OK,GUI_OUT_COMMAND,GUI_SUBCOMMAND_SET_STATE,id);
}

void gui_out_set_block(void) {
	uint8_t id,state;
	id = gui_get_first();
	state = gui_get_next();
	if (state) {
		output_set_flag(id,OUTPUT_BLOCK_FLAG,ON);
	} else {
		output_set_flag(id,OUTPUT_BLOCK_FLAG,OFF);
	}
	save_output_settings(EEPROM_OUTS_SETTING_BEGIN,id);
	gui_cm_return_status_unit(GUI_COMMAND_OK,GUI_OUT_COMMAND,GUI_SUBCOMMAND_SET_BLOCK,id);
}

void gui_out_set_state_all(void) {
	uint8_t state;
	state = gui_get_first();
	outputs_all(state);
	gui_cm_return_status(GUI_COMMAND_OK,GUI_OUT_COMMAND,GUI_SUBCOMMAND_SET_STATE_ALL);
}

void gui_out_set_block_all(void) {
	uint8_t state;
	state = gui_get_first();
	outputs_block_all(state);
	save_outputs_settings (EEPROM_OUTS_SETTING_BEGIN);
	gui_cm_return_status(GUI_COMMAND_OK,GUI_OUT_COMMAND,GUI_SUBCOMMAND_SET_BLOCK_ALL);
}

//====================================================================================== time

void gui_daytime_set(void) {
	daytime_from.hours = gui_get_first();
	daytime_from.minutes = gui_get_next();
	daytime_to.hours = gui_get_next();
	daytime_to.minutes = gui_get_next();
	save_basic_settings(EEPROM_BASIC_SETTINGS_BEGIN);
	gui_cm_return_status(GUI_COMMAND_OK,GUI_DAYTIME_COMMAND,GUI_SUBCOMMAND_SET);
}

void gui_daytime_get_settings(void) {
	gui_command_start(GUI_DAYTIME_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE);
	gui_command_data(daytime_from.hours);
	gui_command_data(daytime_from.minutes);
	gui_command_data(daytime_to.hours);
	gui_command_data(daytime_to.minutes);
	gui_command_end();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_DAYTIME_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS);
}

void gui_datetime_set() {
	year = gui_get_first();
	month = gui_get_next();
	day = gui_get_next();
	wday = gui_get_next();
	rtc_set_date(year,month,day,wday);
	system_flags |= _BV(FLAG_DAY_PAST);
	hours = gui_get_next();
	minutes = gui_get_next();
	seconds = 0;
	rtc_set_time(hours,minutes,0);
	SSF(FLAG_FORCE_LCD_REFRESH);
	gui_cm_return_status(GUI_COMMAND_OK,GUI_DATETIME_COMMAND,GUI_SUBCOMMAND_SET);
}

void gui_datetime_get_settings(void) {
	gui_command_start(GUI_DATETIME_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE);
	gui_command_data(year);
	gui_command_data(month);
	gui_command_data(day);
	gui_command_data(hours);
	gui_command_data(minutes);
	gui_command_end();
	gui_cm_return_status(GUI_COMMAND_OK,GUI_DATETIME_COMMAND,GUI_SUBCOMMAND_GET_SETTINGS);
}




