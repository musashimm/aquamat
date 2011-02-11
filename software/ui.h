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

	$Id: ui.h 46 2009-03-19 21:19:39Z musashi@todryk.pl $
*/

/** @file ui.h
	@brief Plik nagłówkowy - Implementacja interfejsu użytkownika na LCD.
*/

#define UI_KEY_UP_PIN 7
#define UI_KEY_DOWN_PIN 6
#define UI_KEY_SET_PIN 5
#define UI_KEY_ESC_PIN 4
#define UI_KEYPAD_PORT B
#define UI_PORTOUT PORT(UI_KEYPAD_PORT)
#define UI_PORTIN  pin(UI_KEYPAD_PORT)
#define UI_DDR     DDR(UI_KEYPAD_PORT)
#define UI_KEYPAD_MASK 0xF0
#define UI_KEYPAD_DELAY_READ 0x20

enum UI_MENU {STATUS_MENU,ALARMS_MENU,OUTPUTS_MENU,AD_MENU,TEMP_MENU,TIME_MENU,TIMERS_MENU,TIMERSV_MENU};

void ui_init(void);
void ui_key_pressed(void);
void ui_menu_0(void);

/* ----------------------------------------------------- Status */
void status_rotate (void);
void status_outputs(void);
void status_temp(void);
void status_ad(void);
void lcd_temp_sensor(uint8_t t);
void lcd_clock(void);
void lcd_outputs_status(void);
void lcd_clear_temp_indicators(void);
void diplay_MIT(struct MIT *mit);
void display_out_long(uint8_t id);
void display_unit_label(const char *s);
void display_break_line(uint8_t before,uint8_t after);
void display_temp_value(TEMP t);
void lcd_wday_display(uint8_t);
/* ------------------------------------------------------Time */
void ui_time_set_day(void);
void ui_time_set_hours(void);
/* -----------------------------------------------------Temp */
void ui_temp_status_display(void);
void ui_temp_settings_display(void);
void ui_temp_outputs_cooling_display(void);
void ui_temp_outputs_heating_display(void);
void ui_temp_param_display(void);
void ui_temp_sensor_discover(void);
/* -----------------------------------------------------Outputs */
void ui_outputs_status_display(void);
void ui_outputs_settings_display(void);
/* ----------------------------------------------------- AD --*/
void ui_topoff_settings_display(void);
void ui_topoff_clear_max_run(void);
/* ----------------------------------------------------- timery */
void ui_timersv_settings_display(void);
void ui_timers_settings_display(void);
/* ----------------------------------------------------- alarmy */
void ui_alarms_display(void);
void ui_alarms_clear(void);
void ui_alarms_block(void);
/* ------------------------------------------------------------ */
extern volatile enum MENU_KEY key_pressed;
extern volatile uint8_t counterKey;
extern volatile uint8_t back_light_counter;
extern FuncPtr FuncPtrTable[];

