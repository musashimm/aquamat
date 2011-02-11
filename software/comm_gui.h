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
/** @file comm_gui.h
	@brief Plik nagłówkowy - Implementacja komunikacji z interfejsem graficznym.
*/

/** @def CM_BUFFER_LENGTH
	Rozmiar bufora do odbierania znaków.
*/
#define CM_BUFFER_LENGTH 30

/** Wartość wskaźnika dla pustego bufora.
*/
#define PCMBUFF_EMPTY 0

void gui_cm_process_char(void);
void gui_cm_return_status(uint8_t,uint8_t,uint8_t);
void gui_cm_return_status_unit(uint8_t,uint8_t,uint8_t,uint8_t);
void gui_cm_execute(void);
uint8_t gui_get_first(void);
uint8_t gui_get_next(void);
void gui_command_start(uint8_t,uint8_t);
void gui_command_data(uint8_t);
void gui_command_end();

void gui_client_present(void);
void gui_restart (void);

void gui_datetime_get_settings(void);
void gui_datetime_set(void);
void gui_daytime_get_settings(void);
void gui_daytime_set(void);

void gui_out_set_block_all(void);
void gui_out_set_state_all(void);
void gui_out_set_block(void);
void gui_out_set_state(void);
void gui_out_set(void);
void gui_out_get_settings(void);
void gui_out_get_status(void);

void gui_alarm_get_status(void);
void gui_alarm_clear(void);
void gui_alarm_block(void);

void gui_topoff_get_status(void);
void gui_topoff_set(void);
void gui_topoff_clear(void);

void gui_temp_get_status(void);
void gui_temp_get_settings(void);
void gui_temp_discover(void);
void gui_temp_set(void);
void gui_temp_rom(uint8_t);

void gui_timers_get_settings(void);
void gui_timers_set(void);

void gui_timersv_get_settings(void);
void gui_timersv_set(void);

void gui_log_get(void);
void gui_log_clear(void);

