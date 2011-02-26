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

	$Id:$
*/

/** @file timerssec.c
	@brief Implementacja timerów sekundowych.

	Warunkiem włączenia timera jest zgodność czasów: aktualnego i czasu ustawionego w timerze. Warunkiem wyłączenia jest kiedy licznik <em>on</em> osiągnie wartość <em>duration</em>.
*/

#include "aquamat.h"
#include "timerssec.h"
#include "outputs.h"
#include "menu.h"
#include "eeprom.h"
#include "strings.h"
#include "hd44780.h"
#include "ui.h"
#include "usart.h"

extern uint8_t tmp_var;

struct TIMERSEC timerssec[TIMERSSEC_NUM];

/** Zwraca status timera.
	@param t numer timera, od 0 do \ref TIMERSSEC_NUM - 1
*/
uint8_t timerssec_status (uint8_t t) {

	if (timerssec[t].flags & _BV(TIMERSSEC_FLAG_BLOCKED)) {
		return TIM_STATUS_BLOCKED;
	}

	if (timerssec[t].out >= OUTPUTS_NUM) {
		return TIM_STATUS_BAD_OUT;
	}

	if (!(check_is_wday(timerssec[t].flags,TIMERSSEC_FLAG_WDAY_MASK))) {
		return TIM_STATUS_NOT_WDAY;
	}

	if (output_check_flag(timerssec[t].out,OUTPUT_BLOCK_FLAG)) {
		return TIM_STATUS_OUT_BLOCKED;
	}

	if (is_not_valid_MIT(&timerssec[t].when)) {
		return TIM_STATUS_BAD_TIME_1;
	}

	if (timerssec[t].flags & _BV(TIMERSSEC_FLAG_BUSY)) {
		return TIM_STATUS_ALREADY_BUSY;
	}

	if (check_if_its_time(&timerssec[t].when)) {
		return TIM_STATUS_ACTIVE;
	}

	return TIM_STATUS_NOTACTIVE;
}

/** Przełącza skojarzone wyjście.
	@param t numer timera, od 0 do \ref TIMERSSEC_NUM - 1
	@param action \ref ON włączenie, \ref OFF wyłączenie
*/
void timerssec_switch_out (uint8_t t,uint8_t action) {
	if (action) {
		output_switch(timerssec[t].out,ON);
		timerssec[t].flags |= _BV(TIMERSSEC_FLAG_BUSY);
	} else {
		output_switch(timerssec[t].out,OFF);
		timerssec[t].flags &= ~_BV(TIMERSSEC_FLAG_BUSY);
	}
}

/** Sprawdza warunek wyłączenia timera. Procedura wywoływana co sekundę.
*/
void timerssec_when_active(void) {
	uint8_t t;
	for (t=0;t<TIMERSSEC_NUM;t++) {
		if (timerssec_status(t) == TIM_STATUS_ALREADY_BUSY) {
			if (++timerssec[t].on >= timerssec[t].duration) {
				timerssec_switch_out(t,OFF);
			}
		}
	}
}

/** Sprawdza warunek włączenia timera. Procedura wywoływana co minutę.
*/
void timerssec_when_notactive(void) {
	uint8_t t;
	for (t=0;t<TIMERSSEC_NUM;t++) {
		if (timerssec_status(t) == TIM_STATUS_ACTIVE) {
			timerssec[t].on=0;
			timerssec_switch_out(t,ON);
		}
	}
}

/** Wyświetlanie i konfiguracja timerów sekundowych
*/
void ui_timerssec_settings_display(void) {

	if (!MENU_F(MENU_VARS_LOADED_FLAG)) {
		load_name(timerssec[menu_unit()].out * EEPROM_OUTS_SIZE + EEPROM_OUTS_NAME_OFF + EEPROM_OUTS_SETTING_BEGIN);
		tmp_var = (timerssec[menu_unit()].flags & TIMERSSEC_FLAG_WDAY_MASK) >> 1;
		MENU_SF(MENU_VARS_LOADED_FLAG);
	}

	hd44780_label(PSTR("TmrS"),FALSE,FALSE);
	hd44780_out4hex(menu_unit()+1);
	hd44780_switch_state(!(timerssec[menu_unit()].flags & _BV(TIMERSSEC_FLAG_BLOCKED)),FALSE);
	hd44780_outstrn_P(SPACE_S);
	lcd_wday_display(tmp_var);
	hd44780_outstrn_P(SPACE_S);
	diplay_MIT(&timerssec[menu_unit()].when);
	display_break_line(1,0);
	hd44780_out8dec(timerssec[menu_unit()].on);
	hd44780_outstrn_P(SLASH_S);
	hd44780_out8dec(timerssec[menu_unit()].duration);
	display_out_long(timerssec[menu_unit()].out);

	switch (menu_phase()) {
	case 0:
		menu_standard_unit(TIMERSSEC_NUM);
		break;
	case 1:
		menu_set_phase();
		break;
	case 2:
		menu_mod_bit(1,7,&(timerssec[menu_unit()].flags),TIMERSSEC_FLAG_BLOCKED);
		break;
	case 3:
		menu_mod_uint8(1,10,&tmp_var,0,TIMERSSEC_MAX_WDAY,FALSE);
		timerssec[menu_unit()].flags &= ~TIMERSSEC_FLAG_WDAY_MASK;
		timerssec[menu_unit()].flags |= (tmp_var << 1);
		break;
	case 4:
		menu_mod_uint8(1,13,&timerssec[menu_unit()].when.hours,0,MIT_MAX_HOURS,FALSE);
		break;
	case 5:
		menu_mod_uint8(1,16,&timerssec[menu_unit()].when.minutes,0,MIT_MAX_MINUTES,FALSE);
		break;
	case 6:
		menu_mod_uint8(2,5,&(timerssec[menu_unit()].duration),TIMERSSEC_MIN_SET_TIME,TIMERSSEC_MAX_SET_TIME,FALSE);
		break;
	case 7:
		menu_mod_uint8(2,16,&(timerssec[menu_unit()].out),0,OUTPUTS_NUM,TRUE);
		break;
	case 8:
		save_timersec_settings(EEPROM_TIMERSSEC_SETTINGS_BEGIN,menu_unit());
		menu_reset_phase();
	}
}

/** Wczytuje konfigurację wszystkich timerów sekundowych.
*/
void load_timerssec_settings (uint16_t addr) {
	uint8_t i;
 	for (i=0;i<TIMERSSEC_NUM;i++) {
 		load_timersec_settings(i * EEPROM_TIMERSEC_SIZE + addr,i);
 	}
}

/** Zapisuje konfigurację wszystkich timerów sekundowych.
*/
void save_timerssec_settings (uint16_t addr) {
	uint8_t i;
 	for (i=0;i<TIMERSSEC_NUM;i++) {
 		save_timersec_settings(addr,i);
 	}
}

/** Zapisuje konfigurację jednego trimera sekundowego.
	@param uint16_t base_addr adres bazowy, pod który będzie zapisana konfiguracja
	@param uint8_t t index timera
*/
void save_timersec_settings(uint16_t base_addr, uint8_t t) {
	uint16_t t_addr = t * EEPROM_TIMERSEC_SIZE + base_addr;
	eeprom_write_byte(t_addr+EEPROM_TIMERSEC_WHEN_HOURS_OFFSET,timerssec[t].when.hours);
	eeprom_write_byte(t_addr+EEPROM_TIMERSEC_WHEN_MINUTES_OFFSET,timerssec[t].when.minutes);
	eeprom_write_byte(t_addr+EEPROM_TIMERSEC_DURATION_OFFSET,timerssec[t].duration);
	eeprom_write_byte(t_addr+EEPROM_TIMERSEC_OUT_OFFSET,timerssec[t].out);
	eeprom_write_byte(t_addr+EEPROM_TIMERSEC_FLAGS_OFFSET,timerssec[t].flags & TIMERSSEC_FLAG_CONFIG_MASK);
}

/** Wczytuje konfigurację wszystkich timerów sekundowych.
	@param uint16_t addr adres, pod którym zostanie zapisana konfiguracja
	@param uint8_t t index timera
*/
void load_timersec_settings(uint16_t addr, uint8_t t) {
	set_MIT(&timerssec[t].when,eeprom_read_byte(addr+EEPROM_TIMERSEC_WHEN_HOURS_OFFSET),eeprom_read_byte(addr+EEPROM_TIMERSEC_WHEN_MINUTES_OFFSET),MIT_DEFAULT_HOURS,MIT_DEFAULT_MINUTES);
	eeprom_read_byte_and_set(addr+EEPROM_TIMERSEC_DURATION_OFFSET,&timerssec[t].duration,TIMERSSEC_MIN_SET_TIME,TIMERSSEC_MAX_SET_TIME,TIMERSSEC_DEFAULT_TIME);
	outputs_assign(&timerssec[t].out,eeprom_read_byte(addr+EEPROM_TIMERSEC_OUT_OFFSET));
	timerssec[t].flags = eeprom_read_byte(addr+EEPROM_TIMERSEC_FLAGS_OFFSET) & TIMERSSEC_FLAG_CONFIG_MASK;
}

