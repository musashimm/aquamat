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

#include "aquamat.h"
#include "commonDefs.h"
#include "qbuttons.h"
#include "outputs.h"

/** @file qbuttons.c
		@brief Implementacja klawiszy szybkiego dostępu.
*/

/** @var uint8_t qbold
Przechowuje starą wartość stanu QB, akcja wykonywana tylko przy zmianie stanu QB (QB != OLDQB)
*/
uint8_t qbold = UINT8_T_DISABLED;

/** Inicjalizuje porty dla QB
*/
void qbuttons_init() {
	QBUTTONS_DDR &= ~QBUTTONS_MASK;
	QBUTTONS_PORTOUT |= QBUTTONS_MASK;
}

/** Sprawdza stan QB i ewentualnie przestawia stan skojarzonego wyjścia
*/
void qbuttons_process() {
        uint8_t qbId,outId,qbcur;
        qbcur = QBUTTONS_PORTIN >> 2;
        for (qbId=0;qbId<QBUTTONS_NUM;qbId++) {
            if (!(qbcur & _BV(qbId)) && (qbold & _BV(qbId))) {
                for (outId=0;outId<OUTPUTS_NUM;outId++) {
                    if ((outputs[outId].flags & OUTPUTS_QBUTTONS_MASK) == qbId) {
                        output_toggle(outId);
                    }
                }
            }
        }
        qbold = qbcur;
}
//void qbuttons_process() {
//	uint8_t qbId,outId;
//	if ((pinD >> 2) != oldqb) {
//		for (qbId=0;qbId<QBUTTONS_NUM;qbId++) {
//			if (!((QBUTTONS_PORTIN >> 2) & _BV(qbId))) {
//				for (outId=0;outId<OUTPUTS_NUM;outId++) {
//					if ((outputs[outId].flags & OUTPUTS_QBUTTONS_MASK) == qbId) {
//						output_toggle(outId);
//					}
//				}
//			}
//		}
//	oldqb = pinD >> 2;
//	}
//}


