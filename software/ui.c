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

	$Id: ui.c 49 2009-05-05 21:26:37Z musashi@todryk.pl $
*/

/** @file ui.c
@brief Implementacja interfejsu użytkownika na LCD.

@todo
- w czasie wyświetlania ustawień wyjścia i zmianie nazwy wyjścia z poziomu GUI zmienia się nazwa wyjścia
*/

#include "aquamat.h"
#include <avr/pgmspace.h>
#include "ui.h"
#include "hd44780.h"
#include "rtc.h"
#include "temp.h"
#include "outputs.h"
#include "strings.h"
#include "alarms.h"
#include "top_off.h"
#include "eeprom.h"
#include "menu.h"
#include "timers.h"
#include "timersv.h"

extern uint8_t minutes;
extern uint8_t hours;
extern uint8_t tmp_var;
extern uint8_t tmp_var2;
extern uint8_t year;
extern uint8_t month;
extern uint8_t day;
extern uint8_t wday;

volatile enum MENU_KEY key_pressed;
volatile uint8_t counterKey;
volatile uint8_t oldRotate;


PROGMEM char STATUS_MENU_S[] = "";
PROGMEM char ALARMS_MENU_S[] =  ". Alarmy        \n                ";
PROGMEM char OUTPUTS_MENU_S[] = ". Wyjscia       \n                ";
PROGMEM char AD_MENU_S[] =      ". Automatyczna  \n dolewka        ";
PROGMEM char TEMP_MENU_S[] =    ". Kontrola      \n temperatury    ";
PROGMEM char TIME_MENU_S[] =    ". Ustawienia    \n czasu          ";
PROGMEM char TIMERS_MENU_S[] =  ". Timery        \n godzinowe      ";
PROGMEM char TIMERSV_MENU_S[] = ". Timery        \n interwalowe    ";

PROGMEM char AD_CLEAR_MENU_S[] = " Czy skasowac   \n alarmy AD?     ";

PROGMEM char ALARMS_CLEAR_MENU_S[] = "   Czy usunac   \naktywne alarmy? ";
PROGMEM char ALARMS_BLOCK_MENU_S[] =    " Blokada alarmow";
PROGMEM char ALARMS_BLOCK_MENU2_S[] =    " Czy zmienic? ";
PROGMEM char DISCOVER_SENSOR_MENU_S[] = "   Czy wykryc   \n czujniki temp? ";

PGM_P MENU0_STRS[] PROGMEM =
{
	STATUS_MENU_S,
	ALARMS_MENU_S,
	OUTPUTS_MENU_S,
	AD_MENU_S,
	TEMP_MENU_S,
	TIME_MENU_S,
	TIMERS_MENU_S,
	TIMERSV_MENU_S
};

PROGMEM FuncPtr FuncPtrTable[] = {
	status_rotate,status_outputs,status_temp,status_ad,								//4
	ui_menu_0,ui_alarms_display,ui_alarms_clear,ui_alarms_block,			//3
	ui_menu_0,ui_outputs_settings_display,														//2
	ui_menu_0,ui_topoff_clear_max_run,ui_topoff_settings_display,			//3
	ui_menu_0,ui_temp_settings_display,ui_temp_outputs_cooling_display,ui_temp_outputs_heating_display,ui_temp_param_display,ui_temp_sensor_discover,	//6
	ui_menu_0,ui_time_set_hours,ui_time_set_day,											//3
	ui_menu_0,ui_timers_settings_display,															//2
	ui_menu_0,ui_timersv_settings_display															//2
};

PROGMEM const uint8_t MENU_LENGTH[] = {
	8,
	4, //0
	4, //1
	2, //2
	3, //3
	6, //4
	3, //5
	2, //6
	2  //7
};

void ui_init (void) {
	UI_DDR &= ~UI_KEYPAD_MASK;
	UI_PORTOUT |= UI_KEYPAD_MASK;
	menu_set(0,0,0,0);
}

void ui_key_pressed(void) {
	if ((counterKey > UI_KEYPAD_DELAY_READ) && !key_pressed) {

		if (!(UI_PORTIN & _BV(UI_KEY_UP_PIN))) {
			key_pressed = UP;
		} else if (!(UI_PORTIN & _BV(UI_KEY_ESC_PIN))) {
			key_pressed = ESC;
		} else if (!(UI_PORTIN & _BV(UI_KEY_SET_PIN))) {
			key_pressed = SET;
		} else if (!(UI_PORTIN & _BV(UI_KEY_DOWN_PIN))) {
			key_pressed = DOWN;
		}

		if (key_pressed) {
			counterKey=0;
			//back_light_counter=0;
			SSF(FLAG_FORCE_LCD_REFRESH);
		}

	}
}


//################################################# building funkcs

void ui_menu_0(void) {
	PGM_P p;
	hd44780_outdata(menu_menuid() + 0x30);
	memcpy_P(&p, &MENU0_STRS[menu_menuid()], sizeof(PGM_P));
	hd44780_outstrn_P(p);
	menu_next(UP);
	menu_prev(DOWN);
	menu_top(ESC);
	menu_submenu_next(SET);
}

//===========================================================================================================

void status_rotate (void) {

    uint8_t curRotate;
    curRotate = seconds/10;

    if (curRotate != oldRotate) {
        hd44780_clear();
        oldRotate = curRotate;
    }

 	switch (seconds/10) {
 		case 0:
 		case 3:
				ui_outputs_status_display();
				break;
		case 1:
		case 4:
				top_off_status_display();
				break;
 		default:
				ui_temp_status_display();
				break;
 	}
	menu_next(UP);
	menu_prev(DOWN);
	menu_submenu_next(SET);
}

void status_outputs(void) {
	ui_outputs_status_display();
	menu_next(UP);
	menu_prev(DOWN);
	menu_submenu_next(SET);
	menu_top(ESC);
}

void status_temp(void) {
	ui_temp_status_display();
	menu_next(UP);
	menu_prev(DOWN);
	menu_submenu_next(SET);
	menu_top(ESC);
}

void status_ad(void) {
	top_off_status_display();
	menu_next(UP);
	menu_prev(DOWN);
	menu_submenu_next(SET);
	menu_top(ESC);
}

void ui_outputs_status_display(void) {
	if (SF(FLAG_FORCE_LCD_REFRESH)) {
		if (alarms_is_active() && alarms_notification_enabled()) {
			hd44780_outstrn_P(PSTR("   ! ALARM !    "));
		} else {
			lcd_clock();
		}
		hd44780_goto(2,1);
		lcd_outputs_status();
	} else {
		if (alarms_is_active() && alarms_notification_enabled()) {
			hd44780_outstrn_P(PSTR("     ALARM      "));
		} else {
			hd44780_goto(1,3);
			hd44780_outstrn_P(SPACE_S);
		}
	}
}

void ui_temp_status_display(void) {
	if (SF(FLAG_FORCE_LCD_REFRESH)) {
		lcd_temp_sensor(0);
		lcd_temp_sensor(1);
		hd44780_goto(2,1);
		lcd_temp_sensor(2);
		lcd_temp_sensor(3);
	} else {
		lcd_clear_temp_indicators();
	}
}

void lcd_temp_sensor(uint8_t t) {
	hd44780_outstrn_P(SPACE_S);
	if (( temp_sensors[t].flags & TEMP_SENSOR_ROLE_MASK) != TEMP_SENSOR_ROLE_INACTIVE) {
		if(!((temp_sensors[t].flags)&_BV(TEMP_SENSOR_STATUS_OK))) {
			hd44780_outdata(LCD_CHAR_FAILURE);
		} else if (temp_sensors[t].flags & (_BV(TEMP_ALARM_MIN)|_BV(TEMP_ALARM_MAX))) {
			hd44780_outstrn_P(EXCLEMATION_S);
		} else {
			hd44780_outstrn_P(SPACE_S);
		}
		display_temp_value(temp_sensors[t].temp);

		if((temp_sensors[t].flags)&_BV(TEMP_SENSOR_HEATING)) {
			hd44780_outdata(LCD_CHAR_HEATING);
		} else if ((temp_sensors[t].flags)&_BV(TEMP_SENSOR_COOLING)) {
			hd44780_outdata(LCD_CHAR_COOLING);
		} else {
			hd44780_outstrn_P(SPACE_S);
		}
	} else {
		hd44780_outstrn_P(INTEMPSENS_S);
		hd44780_outdata(LCD_CHAR_DEGREE);
		hd44780_outstrn_P(SPACE_S);
	}
}

void lcd_clock(void) {
	hd44780_out8dec(hours);
	hd44780_outstrn_P(COLON_S);
	hd44780_out8dec(minutes);
	hd44780_outstrn_P(SPACE_S);
	hd44780_outstrn_P(SPACE_S);
	hd44780_out8dec(month);
	hd44780_outstrn_P(SLASH_S);
	hd44780_out8dec(day);
	hd44780_outstrn_P(SPACE_S);
	lcd_wday_display(wday);
	hd44780_outstrn_P(SPACE_S);
}

void lcd_wday_display(uint8_t wd) {
	PGM_P p;
	memcpy_P(&p, &RTC_WDAYS_STRS[wd], sizeof(PGM_P));
	hd44780_outstrn_P(p);
}

void lcd_outputs_status(void) {
	uint8_t i;
	for (i=0;i<OUTPUTS_NUM;i++) {
	    hd44780_switch_state(!output_check_flag(i,OUTPUT_ACTIVE_FLAG),output_check_flag(i,OUTPUT_BLOCK_FLAG));
//		if (output_check_flag(i,OUTPUT_ACTIVE_FLAG)) {
//			if (output_check_flag(i,OUTPUT_BLOCK_FLAG)) {
//				hd44780_outdata(LCD_CHAR_OFF_BLOCKED);
//			} else {
//				hd44780_outdata(LCD_CHAR_OFF);
//			}
//		} else {
//			if (output_check_flag(i,OUTPUT_BLOCK_FLAG)) {
//				hd44780_outdata(LCD_CHAR_ON_BLOCKED);
//			} else {
//				hd44780_outdata(LCD_CHAR_ON);
//			}
//		}
	}
}

void lcd_clear_temp_indicators(void) {
	hd44780_goto(1,2);
	hd44780_outstrn_P(SPACE_S);
	hd44780_goto(1,8);
	hd44780_outstrn_P(SPACE_S);
	hd44780_goto(1,10);
	hd44780_outstrn_P(SPACE_S);
	hd44780_goto(1,16);
	hd44780_outstrn_P(SPACE_S);
	hd44780_goto(2,2);
	hd44780_outstrn_P(SPACE_S);
	hd44780_goto(2,8);
	hd44780_outstrn_P(SPACE_S);
	hd44780_goto(1,1);
}
//======================================================== alarmy ==========================================================
void ui_alarms_display(void) {
	hd44780_label(PSTR("Tx"),FALSE,FALSE);
	hd44780_switch_state(alarms_flags & _BV(ALARM_FLAG_TEMP_MAX),FALSE);
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(PSTR("Tn"),FALSE,FALSE);
	hd44780_switch_state(alarms_flags & _BV(ALARM_FLAG_TEMP_MIN),FALSE);
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(PSTR("Tf"),FALSE,FALSE);
	hd44780_switch_state(alarms_flags & _BV(ALARM_FLAG_TEMP_SENSOR_FAILURE),FALSE);
	display_break_line(2,0);
	hd44780_label(PSTR("Ar"),FALSE,FALSE);
	hd44780_switch_state(alarms_flags & _BV(ALARM_FLAG_TOP_OFF_MIN_RO),FALSE);
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(PSTR("Am"),FALSE,FALSE);
	hd44780_switch_state(alarms_flags & _BV(ALARM_FLAG_TOP_OFF_MAX_RUN),FALSE);
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(PSTR("Aln"),FALSE,FALSE);
	hd44780_switch_state(alarms_notification_enabled(),FALSE);
	hd44780_outstrn_P(SPACE_S);
	menu_standard_top();
}

void ui_alarms_clear(void) {

	hd44780_outstrn_P(ALARMS_CLEAR_MENU_S);

	switch (menu_phase()) {
	case 0:
					menu_standard_top();
					break;
	case 1:
					menu_set_phase();
					break;
	case 2:
					hd44780_goto(2,16);
					menu_standard_phase();
					break;
	case 3:
					alarms_clear();
					menu_reset_phase();
					break;
	}
}

void ui_alarms_block(void) {

	hd44780_outstrn_P(ALARMS_BLOCK_MENU_S);
	display_break_line(0,0);
	hd44780_outstrn_P(ALARMS_BLOCK_MENU2_S);
	hd44780_switch_state(AF(ALARM_FLAG_BLOCK),FALSE);
	hd44780_outstrn_P(PAR_S);

	switch (menu_phase()) {

	case 0:         menu_standard_top();
					break;

	case 1:         menu_set_phase();
					break;

	case 2:         menu_mod_bit(2,15,&alarms_flags,ALARM_FLAG_BLOCK);
					break;

    case 3:         menu_reset_phase();
                    break;
	}
}
//========================================================= temp ==========================================================
void ui_temp_settings_display(void) {
	display_unit_label(PSTR("T"));
	hd44780_switch_state(!(temp_sensors[menu_unit()].flags & _BV(TEMP_SENSOR_ROLE_0)),FALSE);
	hd44780_outstrn_P(SPACE_S);
	display_temp_value(temp_sensors[menu_unit()].target);
	hd44780_outstrn_P(SPACE_S);
	display_temp_value(temp_sensors[menu_unit()].target_night);
	display_break_line(2,0);
	display_temp_value(temp_sensors[menu_unit()].min);
	hd44780_outstrn_P(SPACE_S);
	display_temp_value(temp_sensors[menu_unit()].max);
	display_temp_value(temp_sensors[menu_unit()].histeresis);
	hd44780_outstrn_P(PAR_S);

	switch (menu_phase()) {
	case 0:
					menu_standard_top();
					break;
	case 1:
					menu_set_phase();
					break;
	case 2:
					hd44780_goto(1,3);
					menu_standard_unit(TEMP_SENSORS_NUM);
					break;
	case 3:
					menu_mod_bit(1,4,&(temp_sensors[menu_unit()].flags),TEMP_SENSOR_ROLE_0);
					break;
	case 4:
					menu_mod_temp(1,9,&(temp_sensors[menu_unit()].target));
					break;
	case 5:
					menu_mod_temp(1,15,&(temp_sensors[menu_unit()].target_night));
					break;
	case 6:
					menu_mod_temp(2,4,&(temp_sensors[menu_unit()].min));
					break;
	case 7:
					menu_mod_temp(2,10,&(temp_sensors[menu_unit()].max));
					break;
	case 8:
					tmp_var = pgm_read_byte(&TEMP_CONVERT_DS_TO_DEC[temp_sensors[menu_unit()].histeresis]);
					menu_mod_uint8(2,15,&tmp_var,1,9,TRUE);
					temp_sensors[menu_unit()].histeresis = pgm_read_byte(&TEMP_CONVERT_DEC_TO_DS[tmp_var]);
					break;
	case 9:
					save_temp_sensor_settings(EEPROM_TEMP_SETTINGS_BEGIN,menu_unit());
					menu_reset_phase();
					break;
	}
}

void ui_temp_outputs_heating_display(void) {

	if (!MENU_F(MENU_VARS_LOADED_FLAG)) {
		load_output_name(EEPROM_OUTS_SETTING_BEGIN,temp_sensors[menu_unit()].out_heating);
		MENU_SF(MENU_VARS_LOADED_FLAG);
	}

	display_unit_label(PSTR("T"));
	hd44780_outstrn_P(SPACE_S);
	hd44780_outstrn_P(PSTR("Podgrzew."));
	display_break_line(3,2);
	display_out_long(temp_sensors[menu_unit()].out_heating);
	hd44780_outstrn_P(PAR_S);

	switch (menu_phase()) {
	case 0:
					menu_standard_top();
					break;
	case 1:
					menu_set_phase();
					break;
	case 2:
					hd44780_goto(1,3);
					menu_standard_unit(TEMP_SENSORS_NUM);
					break;
	case 3:
					menu_mod_uint8(2,13,&temp_sensors[menu_unit()].out_heating,0,OUTPUTS_NUM,TRUE);
					break;
	case 4:
					save_temp_sensor_settings(EEPROM_TEMP_SETTINGS_BEGIN,menu_unit());
					menu_reset_phase();
					break;
	}
}

void ui_temp_outputs_cooling_display(void) {

	if (!MENU_F(MENU_VARS_LOADED_FLAG)) {
		load_output_name(EEPROM_OUTS_SETTING_BEGIN,temp_sensors[menu_unit()].out_cooling);
		MENU_SF(MENU_VARS_LOADED_FLAG);
	}

	display_unit_label(PSTR("T"));
	hd44780_outstrn_P(SPACE_S);
	hd44780_outstrn_P(PSTR("Chlodzace"));
	display_break_line(3,2);
	display_out_long(temp_sensors[menu_unit()].out_cooling);
	hd44780_outstrn_P(PAR_S);

	switch (menu_phase()) {
	case 0:
					menu_standard_top();
					break;
	case 1:
					menu_set_phase();
					break;
	case 2:
					hd44780_goto(1,3);
					menu_standard_unit(TEMP_SENSORS_NUM);
					break;
	case 3:
					menu_mod_uint8(2,13,&temp_sensors[menu_unit()].out_cooling,0,OUTPUTS_NUM,TRUE);
					break;
	case 4:
					save_temp_sensor_settings(EEPROM_TEMP_SETTINGS_BEGIN,menu_unit());
					menu_reset_phase();
					break;
	}
}

void ui_temp_param_display(void) {
	uint8_t j;
	display_unit_label(PSTR("T"));
	hd44780_label(PSTR("F"),TRUE,FALSE);
	hd44780_out8hex(temp_sensors[menu_unit()].flags);
	hd44780_out8hex(temp_sensors[menu_unit()].bad_reads);
	hd44780_label(PSTR("A"),TRUE,FALSE);
	hd44780_switch_state(temp_sensors[menu_unit()].flags & _BV(TEMP_ALARM_MIN),FALSE);
	hd44780_switch_state(temp_sensors[menu_unit()].flags & _BV(TEMP_ALARM_MAX),FALSE);
	hd44780_switch_state(!(temp_sensors[menu_unit()].flags & _BV(TEMP_SENSOR_STATUS_OK)),FALSE);
	hd44780_next_line();
	for (j=0;j<8;j++) {
		hd44780_out8hex(temp_sensors[menu_unit()].rom[j]);
	}

	switch (menu_phase()) {
	case 0:
					menu_standard_top();
					break;
	case 1:
					menu_set_phase();
					break;
	case 2:
					hd44780_goto(1,3);
					menu_standard_unit(TEMP_SENSORS_NUM);
					break;
	case 3:
					menu_reset_phase();
					break;
	}

}

void ui_temp_sensor_discover(void) {
	hd44780_outstrn_P(DISCOVER_SENSOR_MENU_S);

	switch (menu_phase()) {
	case 0:
					menu_standard_top();
					break;
	case 1:
					menu_set_phase();
					break;
	case 2:
					hd44780_goto(2,16);
					menu_standard_phase();
					break;
	case 3:
					temp_discover_sensors();
					menu_reset_phase();
					break;
	}

}

//========================================================= timers ========================================================
void ui_timersv_settings_display(void) {

	if (!MENU_F(MENU_VARS_LOADED_FLAG)) {
		load_output_name(EEPROM_OUTS_SETTING_BEGIN,timersv[menu_unit()].out);
		MENU_SF(MENU_VARS_LOADED_FLAG);
	}

	hd44780_label(PSTR("TmrI"),FALSE,FALSE);
	hd44780_out4hex(menu_unit()+1);
	hd44780_switch_state(!(timersv[menu_unit()].flags & _BV(TIMERS_FLAG_BLOCKED)),FALSE);
	hd44780_outstrn_P(SPACE_S);
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(PSTR("Wy"),FALSE,FALSE);
	hd44780_out8dec(timersv[menu_unit()].off);
	display_break_line(3,0);
	hd44780_label(PSTR("Wl"),FALSE,FALSE);
	hd44780_out8dec(timersv[menu_unit()].on);
	display_out_long(timersv[menu_unit()].out);

	switch (menu_phase()) {
	case 0:
		menu_standard_unit(TIMERSV_NUM);
		break;
	case 1:
		menu_set_phase();
		break;
	case 2:
		menu_mod_bit(1,7,&(timersv[menu_unit()].flags),TIMERSV_FLAG_BLOCKED);
		break;
	case 3:
		menu_mod_uint8(1,14,&(timersv[menu_unit()].off),TIMERSV_MIN_SET_TIME,TIMERSV_MAX_SET_TIME,FALSE);
		break;
	case 4:
		menu_mod_uint8(2,5,&(timersv[menu_unit()].on),TIMERSV_MIN_SET_TIME,TIMERSV_MAX_SET_TIME,FALSE);
		break;
	case 5:
		menu_mod_uint8(2,16,&(timersv[menu_unit()].out),0,OUTPUTS_NUM,TRUE);
		break;
	case 6:
		save_timerv_settings(EEPROM_TIMERSV_SETTING_BEGIN,menu_unit());
		menu_reset_phase();
	}
}

void ui_timers_settings_display(void) {

	if (!MENU_F(MENU_VARS_LOADED_FLAG)) {
		load_output_name(EEPROM_OUTS_SETTING_BEGIN,timers[menu_unit()].out);
		tmp_var = (timers[menu_unit()].flags & TIMERS_FLAG_WDAY_MASK) >> 1;
		MENU_SF(MENU_VARS_LOADED_FLAG);
	}

	hd44780_label(PSTR("TmrG"),FALSE,FALSE);
	hd44780_out4hex(menu_unit()+1);
	hd44780_switch_state(!(timers[menu_unit()].flags & _BV(TIMERS_FLAG_BLOCKED)),FALSE);
	hd44780_outstrn_P(SPACE_S);
	lcd_wday_display(tmp_var);
	hd44780_outstrn_P(SPACE_S);
	diplay_MIT(&timers[menu_unit()].from);
	display_break_line(1,0);
	diplay_MIT(&timers[menu_unit()].to);
	display_out_long(timers[menu_unit()].out);

	switch (menu_phase()) {
	case 0:
		menu_standard_unit(TIMERS_NUM);
		break;
	case 1:
		menu_set_phase();
		break;
	case 2:
		menu_mod_bit(1,7,&(timers[menu_unit()].flags),TIMERS_FLAG_BLOCKED);
		break;
	case 3:
		menu_mod_uint8(1,10,&tmp_var,0,TIMERS_MAX_WDAY,FALSE);
		timers[menu_unit()].flags &= 0xF1;
		timers[menu_unit()].flags |= (tmp_var << 1);
		break;
	case 4:
		menu_mod_uint8(1,13,&timers[menu_unit()].from.hours,0,MIT_MAX_HOURS,FALSE);
		break;
	case 5:
		menu_mod_uint8(1,16,&timers[menu_unit()].from.minutes,0,MIT_MAX_MINUTES,FALSE);
		break;
	case 6:
		menu_mod_uint8(2,2,&timers[menu_unit()].to.hours,0,MIT_MAX_HOURS,FALSE);
		break;
	case 7:
		menu_mod_uint8(2,5,&timers[menu_unit()].to.minutes,0,MIT_MAX_MINUTES,FALSE);
		break;
	case 8:
		menu_mod_uint8(2,16,&(timers[menu_unit()].out),0,OUTPUTS_NUM,TRUE);
		break;
	case 9:
		save_timer_settings(EEPROM_TIMERS_SETTING_BEGIN,menu_unit());
		//save_timers_settings(EEPROM_TIMERS_SETTING_BEGIN);
		menu_reset_phase();
	}
}

//======================================================== wyjścia =========================================================
void ui_outputs_settings_display(void) {

	if (!MENU_F(MENU_VARS_LOADED_FLAG)) {
		load_output_name(EEPROM_OUTS_SETTING_BEGIN,menu_unit());
		tmp_var = outputs[menu_unit()].flags & OUTPUTS_QBUTTONS_MASK;
		MENU_SF(MENU_VARS_LOADED_FLAG);
	}

	hd44780_label(OUT_S,FALSE,FALSE);
	hd44780_out8dec(menu_unit()+1);
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(BL_S,FALSE,FALSE);
	hd44780_switch_state(output_check_flag(menu_unit(),OUTPUT_BLOCK_FLAG),FALSE);
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(AKT_S,FALSE,FALSE);
	hd44780_switch_state(!output_check_flag(menu_unit(),OUTPUT_ACTIVE_FLAG),FALSE);
	hd44780_next_line();
	hd44780_label(QB_S,TRUE,FALSE);
	if (tmp_var >= OUTPUTS_QBUTTONS_NUM) {
		tmp_var = OUTPUTS_QBUTTONS_NUM;
		hd44780_outstrn_P(MINUS_S);
	} else {
		hd44780_out4hex(tmp_var+1);
	}
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(NZW_S,TRUE,FALSE);
	hd44780_outstrn(output_name);
	hd44780_outstrn_P(SPACE_S);

	switch (menu_phase()) {
	case 0:
		menu_standard_unit(OUTPUTS_NUM);
		break;
	case 1:
		menu_set_phase();
		break;
	case 2:
		menu_mod_bit(1,10,&(outputs[menu_unit()].flags),OUTPUT_BLOCK_FLAG);
		break;
	case 3:
		menu_mod_bit(1,16,&(outputs[menu_unit()].flags),OUTPUT_ACTIVE_FLAG);
		SOUTF(OUTPUTS_CHANGE_FLAG);
		break;
	case 4:
		menu_mod_uint8(2,5,&tmp_var,0,OUTPUTS_QBUTTONS_NUM,FALSE);
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		menu_mod_char(2,12,output_name,5);
		break;
	case 10:
		outputs[menu_unit()].flags &= ~OUTPUTS_QBUTTONS_MASK;
		outputs[menu_unit()].flags |= tmp_var;
		save_output_settings(EEPROM_OUTS_SETTING_BEGIN,menu_unit());
		save_output_name(EEPROM_OUTS_SETTING_BEGIN,menu_unit());
		menu_reset_phase();
	}
}

//======================================================== automatyczna dolewka ============================================
void ui_topoff_clear_max_run(void) {

	hd44780_outstrn_P(AD_CLEAR_MENU_S);

	switch (menu_phase()) {
	case 0:
					menu_standard_top();
					break;
	case 1:
					menu_set_phase();
					break;
	case 2:
					hd44780_goto(2,12);
					menu_standard_top();
					break;
	case 3:
					alarmLog(ALARM_FLAG_TOP_OFF_MAX_RUN,OFF,LOG_EVENT_TOP_OFF_MAX_RUN,0);
					alarmLog(ALARM_FLAG_TOP_OFF_MIN_RO,OFF,LOG_EVENT_TOP_OFF_MIN_RO,0);
					top_off_unblock_run();
					menu_reset_phase();
					break;
	}

}

void ui_topoff_settings_display(void) {

	if (top_off.out > OUTPUTS_NUM) {
		top_off.out = OUTPUTS_NUM;
	}

	if (!MENU_F(MENU_VARS_LOADED_FLAG)) {
		load_output_name(EEPROM_OUTS_SETTING_BEGIN,top_off.out);
		MENU_SF(MENU_VARS_LOADED_FLAG);
	}
	hd44780_label(PSTR("AD"),FALSE,FALSE);
	hd44780_switch_state(!(top_off.flags & _BV(TOP_OFF_BLOCK)),FALSE);
	hd44780_outstrn_P(SPACE_S);
	hd44780_label(PSTR("MxT"),FALSE,FALSE);
	if (top_off.max_run < UINT8_T_DISABLED) {
		hd44780_out8dec3(top_off.max_run);
	} else {
		hd44780_outstrn_P(MINUSMINUSMINUS_S);
	}
	display_break_line(3,0);

	hd44780_label(PSTR("BRO"),FALSE,FALSE);
	hd44780_switch_state(top_off.flags & _BV(TOP_OFF_BLOCK_MIN_RO),FALSE);
	display_out_long(top_off.out);

	switch (menu_phase()) {
		case 0:
					menu_standard_top();
					break;
		case 1:
					menu_set_phase();
					break;
		case 2:
					menu_mod_bit(1,4,&top_off.flags,TOP_OFF_BLOCK);
					break;
		case 3:
					menu_mod_uint8(1,12,&top_off.max_run,TOP_OFF_MIN_RUN_TIME,UINT8_T_DISABLED-1,FALSE);
					break;
		case 4:
					menu_mod_bit(2,5,&top_off.flags,TOP_OFF_BLOCK_MIN_RO);
					break;
		case 5:
					menu_mod_uint8(2,16,&top_off.out,0,OUTPUTS_NUM,TRUE);
					break;
		case 6:
					save_topoff_settings (EEPROM_TOPOFF_SETTINGS_BEGIN);
					menu_reset_phase();
					break;
	}
}

//======================================================== ustawienia czasu ================================================
void ui_time_set_hours(void) {

	hd44780_outstrn_P(PSTR("  Czas: "));
	hd44780_out8dec(hours);
	hd44780_outstrn_P(COLON_S);
	hd44780_out8dec(minutes);
	display_break_line(4,2);
 	hd44780_out8dec(year);
	hd44780_outstrn_P(MINUS_S);
 	hd44780_out8dec(month);
	hd44780_outstrn_P(MINUS_S);
	hd44780_out8dec(day);
	hd44780_outstrn_P(SPACE_S);
	lcd_wday_display(wday);
	hd44780_outstrn_P(PAR_S);

	switch (menu_phase()) {
		case 0:
			menu_standard_top();
			break;
		case 1:
			menu_set_phase();
			break;
		case 2:
			menu_mod_uint8(1,10,&hours,0,MIT_MAX_HOURS,FALSE);
			break;
		case 3:
			menu_mod_uint8(1,13,&minutes,1,MIT_MAX_MINUTES,FALSE);
			break;
		case 4:
			menu_mod_uint8(2,4,&year,0,20,FALSE);
			break;
		case 5:
			menu_mod_uint8(2,7,&month,1,12,FALSE);
			break;
		case 6:
			menu_mod_uint8(2,10,&day,1,31,FALSE);
			break;
		case 7:
			menu_mod_uint8(2,13,&wday,0,6,FALSE);
			break;
		case 8:
			rtc_set_date(year,month,day,wday);
			rtc_set_time(hours,minutes,0);
			menu_reset_phase();
			break;
	}

}

void ui_time_set_day(void) {

	hd44780_outstrn_P(PSTR(" Pora dnia:"));
	display_break_line(5,2);
	diplay_MIT(&daytime_from);
	hd44780_outstrn_P(MINUS_S);
	diplay_MIT(&daytime_to);
	hd44780_outstrn_P(PAR_S);

	switch (menu_phase()) {
		case 0:
			menu_standard_top();
			break;
		case 1:
			menu_set_phase();
			break;
		case 2:
			menu_mod_uint8(2,4,&daytime_from.hours,0,MIT_MAX_HOURS,FALSE);
			break;
		case 3:
			menu_mod_uint8(2,7,&daytime_from.minutes,0,MIT_MAX_MINUTES,FALSE);
			break;
		case 4:
			menu_mod_uint8(2,10,&daytime_to.hours,0,MIT_MAX_HOURS,FALSE);
			break;
		case 5:
			menu_mod_uint8(2,13,&daytime_to.minutes,0,MIT_MAX_MINUTES,FALSE);
			break;
		case 6:
			save_basic_settings(EEPROM_BASIC_SETTINGS_BEGIN);
			menu_reset_phase();
			break;
	}

}

void diplay_MIT(struct MIT *mit) {				// LCD zajmuje 5 znaków
	if (mit->hours > MIT_MAX_HOURS) {
		hd44780_outstrn_P(MINUSMINUS_S);
	} else {
		hd44780_out8dec(mit->hours);
	}
	hd44780_outstrn_P(COLON_S);
	if (mit->minutes > MIT_MAX_MINUTES) {
		hd44780_outstrn_P(MINUSMINUS_S);
	} else {
		hd44780_out8dec(mit->minutes);
	}
}

void display_out_long(uint8_t id) {				// LCD zajmuje 10 znaków

	hd44780_label(PSTR("W"),TRUE,FALSE);

	if (id >= OUTPUTS_NUM) {
		hd44780_outstrn_P(MINUSMINUS_S);
		//hd44780_out8dec(id+1);
		hd44780_outstrn_P(SLASH_S);
		hd44780_outstrn_P(MINUSMINUS_S);
		hd44780_outstrn_P(MINUSMINUSMINUS_S);
	} else {
		hd44780_out8dec(id+1);
		hd44780_outstrn_P(SLASH_S);
		hd44780_outstrn(output_name);
	}
}

void display_break_line(uint8_t before,uint8_t after) {
	uint8_t i;
	for (i=0;i<before;i++) {
		hd44780_outstrn_P(SPACE_S);
	}
	hd44780_next_line();
	for (i=0;i<after;i++) {
		hd44780_outstrn_P(SPACE_S);
	}
}

void display_unit_label(const char *s) {
	hd44780_label(s,FALSE,FALSE);
	hd44780_out4hex(menu_unit()+1);
	//hd44780_outstrn_P(SPACE_S);
}

void display_temp_value(TEMP t) {
	tmp_var = (uint8_t)(t >> 4) ;
  if (tmp_var/10) {
		hd44780_out4hex(tmp_var/10);
	} else {
		hd44780_outstrn_P(SPACE_S);
	}
	hd44780_out4hex(tmp_var%10);
	hd44780_outstrn_P(DOT_S);
	hd44780_out4hex(temp_ds_to_dec(t));
	hd44780_outdata(LCD_CHAR_DEGREE);
}
