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

#include "pwm.h"

/** @file pwm.c
	@brief Implementacja wyjść PWM.

	Definicja \ref PWM_PORT określa, który port będzie portem PWM. Definicja \ref PWM_PINS określa, które porty będą PWM.
*/

extern uint8_t clock10ms;

/** @struct PWM
	@brief Struktura przechowywująca dane o wyjściach PWM.

	Index wyjścia PWM odpowiada pinowi w porcie \ref PWM_PORT.
*/
struct PWM pwms[PWM_NUM];

/** Inicjalizacja wyjść PWM
*/
void pwm_init(void) {
	PWM_DDR |= PWM_PINS;
}

/** Procedura ustawienia pwm na porcie.
*/
inline
void pwm_to_port(void) {
	uint8_t i;
	for (i=0;i < PWM_NUM;i++) {
		if (clock10ms <= pwms[i].width) {
			PWM_PORTOUT |= _BV(i);
		} else {
			PWM_PORTOUT &= ~_BV(i);
		}
	}
}

/** Wyświetlanie i konfiguracja wyjść PWM
*/
void ui_pwm_settings_display(void) {

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

