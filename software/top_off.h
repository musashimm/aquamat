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

/** @file top_off.h
	@brief Plik nagłówkowy - Implementacja automatycznej dolewki.
*/

#define TOP_OFF_MAXSUMP_PORT B
#define TOP_OFF_MAXSUMP_PIN PORT2
#define TOP_OFF_MAXSUMP_PORTOUT PORT(TOP_OFF_MAXSUMP_PORT)
#define TOP_OFF_MAXSUMP_PORTIN  pin(TOP_OFF_MAXSUMP_PORT)
#define TOP_OFF_MAXSUMP_DDR     DDR(TOP_OFF_MAXSUMP_PORT)

#define TOP_OFF_MINSUMP_PORT B
#define TOP_OFF_MINSUMP_PIN PORT1
#define TOP_OFF_MINSUMP_PORTOUT    PORT(TOP_OFF_MINSUMP_PORT)
#define TOP_OFF_MINSUMP_PORTIN     pin(TOP_OFF_MINSUMP_PORT)
#define TOP_OFF_MINSUMP_DDR        DDR(TOP_OFF_MINSUMP_PORT)

#define TOP_OFF_MINRO_PORT B
#define TOP_OFF_MINRO_PIN PORT0
#define TOP_OFF_MINRO_PORTOUT    PORT(TOP_OFF_MINRO_PORT)
#define TOP_OFF_MINRO_PORTIN     pin(TOP_OFF_MINRO_PORT)
#define TOP_OFF_MINRO_DDR        DDR(TOP_OFF_MINRO_PORT)

#define TOP_OFF_BLOCK_PORT B
#define TOP_OFF_BLOCK_PIN PORT3
#define TOP_OFF_BLOCK_PORTOUT    PORT(TOP_OFF_BLOCK_PORT)
#define TOP_OFF_BLOCK_PORTIN     pin(TOP_OFF_BLOCK_PORT)
#define TOP_OFF_BLOCK_DDR        DDR(TOP_OFF_BLOCK_PORT)

#define TOP_OFF_FLAGS_MASK (_BV(TOP_OFF_MAX_SUMP)|_BV(TOP_OFF_MIN_SUMP)|_BV(TOP_OFF_MIN_RO)|_BV(TOP_OFF_BLOCK_BY_PIN))

#define TOP_OFF_OFF_MASK (_BV(TOP_OFF_MAX_RUN_REACHED)|_BV(TOP_OFF_BLOCK_BY_PIN)|_BV(TOP_OFF_BLOCK)|_BV(TOP_OFF_MAX_SUMP))

#define TOPSSF(x) (top_off.flags |= _BV(x))
#define TOPCSF(x) (top_off.flags &= ~_BV(x))
#define TOPSF(x) (top_off.flags & _BV(x))

#define TOP_OFF_IS_RUNNING() (!(output_check_flag(top_off.out,OUTPUT_ACTIVE_FLAG)) && !output_check_flag(top_off.out,OUTPUT_BLOCK_FLAG))
#define TOP_OFF_IS_NOT_RUNNING() (output_check_flag(top_off.out,OUTPUT_ACTIVE_FLAG) && !output_check_flag(top_off.out,OUTPUT_BLOCK_FLAG))

void top_off_process();
void top_off_init(void);
void top_off_unblock_run(void);
void top_off_status_display(void);
void top_off_on(void);
void top_off_off(void);
void top_off_check_time(void);

/** @struct TOP_OFF
	@brief Struktura przechowywująca dane o automatycznej dolewce.
*/
struct TOP_OFF {
	/** Flagi.
	@verbatim
7654|3210
    |   X - blokad apracy
    |   0 - nieaktywna
    |   1 - aktywna
    |  X  - blokada poprzez pin TOP_OFF_BLOCK
    |  0  - nieaktywna
    |  1  - aktywna
    | X   - aktywna blokada top_off_min_ro ?
    | 0   - nie
    | 1   - tak
    |X    - przekroczony czas pracy pompy ?
    |0    - nie
    |1    - tak
   X|     - aktywny czujnik top_off_max_sump ?
   0|     - nie
   1|     - tak
  X |     - aktywny czujnik top_off_min_sump ?
  0 |     - nie
  1 |     - tak
 X  |     - aktywny czujnik top_off_min_ro ?
 0  |     - nie
 1  |     - tak
X   |     - pompa pracuje ?
0   |     - nie
1   |     - tak
	@endverbatim
	*/
	uint8_t flags;
	uint8_t out;	/**< Przypisane wyjście.*/
	uint8_t run_time; /**< Czas, podczas, którego aktywne jest wyjście. */
	uint8_t max_run;	/**< Maksymalny czas, w którym wyjście jest w stanie aktywnym (włączonym). Wartość \ref UINT8_T_DISABLED (0xFF/255) blokuje sprawdzanie czasu.*/
};

extern struct TOP_OFF top_off;
