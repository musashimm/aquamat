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

/** @file menu.h
	@brief Plik nagłówkowy - funkcje obsługi menu.
*/

#include <stdint.h>

#define MENU_VARS_LOADED_FLAG 0

#define MENU_SF(x) ms.flags |= _BV(x)
#define MENU_CF(x) ms.flags &= ~_BV(x)
#define MENU_F(x) ms.flags  & _BV(x)

				     // 0   1   2   3   4
enum MENU_KEY {NONE,UP,DOWN,ESC,SET};

/** @struct MENU_STATE
	@brief Struktura przechowująca stan menu.
*/
struct MENU_STATE{
    uint8_t menu_id;    /**< Identyfikator menu.*/
    uint8_t submenu_id; /**< Identyfikator podmenu.*/
	uint8_t mod_phase;  /**< Faza modyfikacji. */
	uint8_t unit_id;    /**< Identyfikator modyfikowanego elementu. */
/** Flagi.
@verbatim
7654|3210
    |   X - MENU_VARS_LOADED_FLAG
    |   0 - niezaładowane
    |   1 - załadaowane
@endverbatim
	*/
		uint8_t flags;
};

/* ----------------------------------------------- zmiana stanu ---- */
void menu_set(uint8_t,uint8_t,uint8_t,uint8_t);
void menu_key_phase_up (enum MENU_KEY);
void menu_phase_up(void);
uint8_t menu_phase();
uint8_t menu_unit();
uint8_t menu_menuid();
void menu_reset_phase(void);
void menu_set_phase(void);
void menu_reset_phase_key(enum MENU_KEY);
void menu_unit_next (enum MENU_KEY,uint8_t);
void menu_unit_prev (enum MENU_KEY,uint8_t);
void menu_next (enum MENU_KEY);
void menu_prev (enum MENU_KEY);
void menu_top (enum MENU_KEY);
void menu_submenu_top (enum MENU_KEY);
void menu_submenu_next (enum MENU_KEY);
void menu_submenu_prev (enum MENU_KEY key);
void menu_dummy(void);
void menu_standard_top(void);
void menu_standard_phase(void);
void menu_standard_unit(uint8_t);
uint8_t menu_func_index();
void menu_phase_change (enum MENU_KEY key,uint8_t pidx);
/* ----------------------------------------------- modyfikacja parametrów ----- */

void menu_inc_uint8 (enum MENU_KEY key,uint8_t* var,uint8_t max,uint8_t);
void menu_dec_uint8 (enum MENU_KEY key,uint8_t* var,uint8_t min,uint8_t);
void menu_zero_uint8 (enum MENU_KEY key,uint8_t* var);
void menu_toggle_bit(enum MENU_KEY,uint8_t*,uint8_t);

void menu_mod_bit(uint8_t posx,uint8_t posy,uint8_t *var,uint8_t bit);
void menu_mod_uint8(uint8_t posx,uint8_t posy, uint8_t *var, uint8_t min, uint8_t max,uint8_t reload);
void menu_mod_char(uint8_t posx,uint8_t posy,char *var,uint8_t base);
void menu_mod_temp(uint8_t posx,uint8_t posy, TEMP *var);

/* ------------------------------------------------ externy -------------- */

extern struct MENU_STATE ms;
extern PROGMEM const uint8_t MENU_LENGTH[];
extern PROGMEM const uint8_t TEMP_CONVERT_DEC_TO_DS[];
extern PROGMEM const uint8_t TEMP_CONVERT_DS_TO_DEC[];
extern volatile enum MENU_KEY key_pressed;
