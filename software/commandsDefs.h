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

#define GUI_BUFFER_FULL	0x81
#define GUI_UNKONWN_COMMAND	0x82
#define GUI_CLIENT_PRESENT	0x83
#define GUI_COMMAND_OK 0x84
#define GUI_COMMAND_CKSUM_ERROR 0x85
#define GUI_COMMAND_PARAMS_ERROR 0x86
#define GUI_EMPTY_RES 0x87
#define GUI_PROCESSING_COMMAND 0x88

#define GUI_EXTEND_MASK 0x7F
#define GUI_EXTEND_BYTE 0x80

#define GUI_STATUS_COMMAND 0x81
#define GUI_DATETIME_COMMAND 0x82
#define GUI_DAYTIME_COMMAND 0x83
#define GUI_TEMP_COMMAND 0x84
#define GUI_OUT_COMMAND 0x85
#define GUI_ALARM_COMMAND 0x86
#define GUI_TIMERS_COMMAND 0x87
#define GUI_TIMERSV_COMMAND 0x88
#define GUI_TOP_OFF_COMMAND 0x89
#define GUI_LOG_COMMAND 0x90
#define GUI_DEBUG_COMMAND 0x91

#define GUI_SUBCOMMAND_GET_STATUS 0x81
#define GUI_SUBCOMMAND_GET_STATUS_RESPONSE 0xA1
#define GUI_SUBCOMMAND_SET 0x82
#define GUI_SUBCOMMAND_CLEAR 0x83
#define GUI_SUBCOMMAND_GET_SETTINGS 0x84
#define GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE 0xA4
//#define GUI_SUBCOMMAND_SET_NAME 0x85
#define GUI_SUBCOMMAND_SET_STATE 0x86
#define GUI_SUBCOMMAND_SET_BLOCK 0x87
#define GUI_SUBCOMMAND_SET_STATE_ALL 0x88
#define GUI_SUBCOMMAND_SET_BLOCK_ALL 0x89
#define GUI_SUBCOMMAND_DISCOVER 0x8A
#define GUI_SUBCOMMAND_DISCOVER_RESPONSE 0xAA

#define GUI_END_OF_COMMAND 0xFE

/** @file commandsDefs.h
	@brief Plik nagłówkowy - Implementacja protokołu komunikacyjnego

	@section protocol Protokół

	@subsection statusmgmt Wiadomości statusowe
	@verbatim
	1. <GUI_STATUS_COMMAND> <GUI_BUFFER_FULL> <CRC> <CRC> <GUI_END_OF_COMMAND> - przepełniony bufor odbiorczy sterownika

	2. <GUI_STATUS_COMMAND> <GUI_UNKONWN_COMMAND> <CRC> <GUI_END_OF_COMMAND> - nierozpoznana komenda

	3. <GUI_STATUS_COMMAND> <GUI_CLIENT_PRESENT> <MAIN> <MAJOR> <MINOR> <CRC> <GUI_END_OF_COMMAND> - aktywny sterownik z podaniem wersji oprogramowania

	4. <GUI_STATUS_COMMAND> <GUI_COMMAND_CKSUM_ERROR> <CRC> <GUI_END_OF_COMMAND> - błąd sumy kontrolnej komendy

	5. <GUI_STATUS_COMMAND> <GUI_COMMAND_OK> <CRC> <GUI_END_OF_COMMAND> - komenda poprawna

	6. <GUI_STATUS_COMMAND> <GUI_COMMAND_PARAMS_ERROR> <CRC> <GUI_END_OF_COMMAND> - błąd komendy - niepoprawne parametry
	@endverbatim

	@subsection timemgmt Zarządzanie czasami
	@verbatim
	1. <GUI_DATETIME_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS> <CRC> <GUI_END_OF_COMMAND> - pobranie czasu
		Odp: <GUI_DATETIME_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE> <YEAR> <MONTH> <DAY> <HOURS> <MINUTES> <CRC> <GUI_END_OF_COMMAND>

	2. <GUI_DATETIME_COMMAND> <GUI_SUBCOMMAND_SET> <YEAR> <MONTH> <DAY> <WDAY> <HOURS> <MINUTES> <CRC> <GUI_END_OF_COMMAND> - ustawienie czasu
		Odp: brak

	3. <GUI_DAYTIME_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS> <CRC> <GUI_END_OF_COMMAND> - pobranie pory dnia
		Odp: <GUI_DAYTIME_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE> <HOUR_FROM> <MINUTES_FORM> <HOUR_TO> <MINUTES_TO> <CRC> <GUI_END_OF_COMMAND>

	4. <GUI_DAYTIME_COMMAND> <GUI_SUBCOMMAND_SET_SETTINGS> <HOUR_FROM> <MINUTES_FORM> <HOUR_TO> <MINUTES_TO> <CRC> <GUI_END_OF_COMMAND> - ustawienie pory dnia
		Odp: brak
	@endverbatim

	@subsection alarmsmgmt Zarządzanie alarmami
	@verbatim
	1. <GUI_ALARM_COMMAND> <GUI_SUBCOMMAND_GET_STATUS> <CRC> <GUI_END_OF_COMMAND> - pobranie stanu alarmów
		Odp: <GUI_OUT_COMMAND> <GUI_SUBCOMMAND_GET_STATUS_RESPONSE> <FLAGS> <BLOCK> <CRC> <GUI_END_OF_COMMAND> - stan alarmów

	2. <GUI_ALARM_COMMAND> <GUI_SUBCOMMAND_CLEAR> <CRC> <GUI_END_OF_COMMAND> - zeruje aktualne alarmy
		Odp: brak

	3. <GUI_ALARM_COMMAND> <GUI_SUBCOMMAND_SET_BLOCK> <CRC> <GUI_END_OF_COMMAND> - blokuje powiadomienia o alarmach
		Odp: brak
	@endverbatim

	@subsection outsmgmt Zarządzanie wyjściami
	@verbatim

	1. <GUI_OUT_COMMAND> <GUI_SUBCOMMAND_GET_STATUS> <CRC> <GUI_END_OF_COMMAND> - pobranie stanu wyjścia (aktywność, blokada)
		Odp: <GUI_OUT_COMMAND> <GUI_SUBCOMMAND_GET_STATUS_RESPONSE> n x <bajt statusu> <CRC> <GUI_END_OF_COMMAND>

	2. <GUI_OUT_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS> <CRC> <GUI_END_OF_COMMAND> - pobranie stanu wyjścia oraz nazwy wyjść
		Odp: <GUI_OUT_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE> n x<bajt flags oraz bajty z nazwą> <CRC> <GUI_END_OF_COMMAND>

	3. <GUI_OUT_COMMAND><GUI_SUBCOMMAND_SET><id><FLAGS><A><B><C><D><E><CRC> <GUI_END_OF_COMMAND> - ustawienie nazwy wyjścia oraz flag
		Odp: Brak

	4. <GUI_OUT_COMMAND><GUI_SUBCOMMAND_SET_STATE><id><stan><CRC> <GUI_END_OF_COMMAND> - ustawienie stanu wyjścia <id> zgodnie z bajtem <stan>
		Odp: Brak

	5. <GUI_OUT_COMMAND><GUI_SUBCOMMAND_SET_BLOCK><id><stan><CRC> <GUI_END_OF_COMMAND> - ustawienie blokady wyjścia <id> zgodnie z bajtem <stan>
		Odp: Brak

	6. <GUI_OUT_COMMAND><GUI_SUBCOMMAND_SET_STATE_ALL><stan><CRC> <GUI_END_OF_COMMAND> - ustawienie stanu wszystkich wyjść zgodnie z bajtem <stan>
		Odp: Brak

	7. <GUI_OUT_COMMAND><GUI_SUBCOMMAND_SET_BLOCK_ALL><stan><CRC> <GUI_END_OF_COMMAND> - ustawienie stanu blokad wszystkich wyjść zgodnie z bajtem <stan>
		Odp: Brak

	@endverbatim

	@subsection topoffmgmt Zarządzanie automatyczną dolewką
	@verbatim

	1. <GUI_TOP_OFF_COMMAND> <GUI_SUBCOMMAND_GET_STATUS> <CRC> <GUI_END_OF_COMMAND> - pobranie stanu automatycznej dolewki
		Odp: <GUI_TOP_OFF_COMMAND> <GUI_SUBCOMMAND_GET_STATUS_RESPONSE> <FLAGS> <OUT> <MAX_RUN> <CRC> <GUI_END_OF_COMMAND>

	2. <GUI_TOP_OFF_COMMAND> <GUI_SUBCOMMAND_SET> <FLAGS> <OUT> <MAX_RUN> <CRC> <GUI_END_OF_COMMAND> - ustawienie parametrów pracy automatycznej dolewki
		Odp: Brak

	3. <GUI_TOP_OFF_COMMAND> <GUI_SUBCOMMAND_CLEAR> <CRC> <GUI_END_OF_COMMAND> - wyzerowanie blokady pracy AD oraz związanego z tym alarmu
		Odp: Brak
	@endverbatim


	@subsection tempmgmt Zarządzanie czujnikami temperatury
	@verbatim

	1. <GUI_TEMP_COMMAND> <GUI_SUBCOMMAND_GET_STATUS> <CRC> <GUI_END_OF_COMMAND> - pobranie stanu czujnikow
		Odp: <GUI_TEMP_COMMAND> <GUI_SUBCOMMAND_GET_STATUS_RESPONSE> <ID> <TENS> <UNITS> <FLAGS> <BR> <CRC> <GUI_END_OF_COMMAND>

	2. <GUI_TEMP_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS> <CRC> <GUI_END_OF_COMMAND> - pobranie konfiguracji
		Odp: <GUI_TEMP_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE> <ID> <TEMP> <TARGET> <TARGET_NIGHT> <MAX> <MIN> <HISTERESIS> <FLAGS> <BR> <OUT_COOLING> <OUT_HEATING> <CRC> <GUI_END_OF_COMMAND>

	3. <GUI_TEMP_COMMAND> <GUI_SUBCOMMAND_DISCOVER> <CRC> <GUI_END_OF_COMMAND> - wykryj czujniki
		Odp: <GUI_TEMP_COMMAND> <GUI_SUBCOMMAND_DISCOVER_RESPONSE> 8 x <BAJT> <CRC> <GUI_END_OF_COMMAND>

	4. <GUI_TEMP_COMMAND> <GUI_SUBCOMMAND_SET> <ID> <TARGET> <TARGET_NIGHT> <MAX> <MIN> <HISTERESIS> <FLAGS> <OUT_COOLING> <OUT_HEATING> <CRC> <GUI_END_OF_COMMAND> - ustawienie konfiguracji
	@endverbatim

	@subsection timersmgmt Zarządzenie timerami godzinowymi
	@verbatim
	1. <GUI_TIMERS_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS> <CRC> <GUI_END_OF_COMMAND> - pobranie konfiguracji
		Odp: <GUI_TIMERSV_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE> <ID> <FROMH> <FROMM> <TOH> <TOM> <OUT> <FLAGS> <CRC> <GUI_END_OF_COMMAND> - konfiguracja dla timera o identyfikatorze <ID>

	2. <GUI_TIMERS_COMMAND> <GUI_SUBCOMMAND_SET> <ID> <FROMH> <FROMM> <TOH> <TOM> <OUT> <FLAGS> <CRC> <GUI_END_OF_COMMAND> - ustawienie konfiguracji
		Odp: Brak
	@endverbatim

	@subsection timersvmgmt Zarządzenie timerami interwałowymi
	@verbatim
	1. <GUI_TIMERSV_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS> <CRC> <GUI_END_OF_COMMAND> - pobranie konfiguracji
		Odp: <GUI_TIMERSV_COMMAND> <GUI_SUBCOMMAND_GET_SETTINGS_RESPONSE> <ID> <OFF> <ON> <OUT> <FLAGS> <COUNTER> <CRC> <GUI_END_OF_COMMAND> - konfiguracja dla timera o identyfikatorze <ID>

	2. <GUI_TIMERSV_COMMAND> <GUI_SUBCOMMAND_SET> <ID> <OFF> <ON> <OUT> <FLAGS> <CRC> <GUI_END_OF_COMMAND> - ustawienie konfiguracji
		Odp: Brak
	@endverbatim

	@subsection eventmgmt Zarządzanie historią zdarzeń
	@verbatim
	1. <GUI_LOG_COMMAND> <GUI_SUBCOMMAND_GET_STATUS> <CRC> <GUI_END_OF_COMMAND> - żadanie historii zdarzeń
		Odp: <GUI_EVENT_COMMAND> <GUI_SUBCOMMAND_GET_STATUS_RESPONSE> 8 x <UCHAR> <CRC> <GUI_END_OF_COMMAND> - jeden wpis z historii zdarzeń (8 bajtów).

	2. <GUI_LOG_COMMAND> <GUI_SUBCOMMAND_CLEAR> <CRC> <GUI_END_OF_COMMAND> - wyczyszczenie loga
		Odp: brak.
	@endverbatim
 	@subsection crccalc Obliczanie sumy kontrolnej CRC
 	Żeby zabezpieczyć się przed prostymi błędami mogącymi wystąpić w czasie komunikacji, sterownik wylicza sumę kontrolną
 	otrzymanych komend. W przypadku wykrycia błędnej sumy kontrolnej komenda jest ignorowana.

 	Algorytm wyliczania:
 	@verbatim
 	crc=0;
	for (i=0;i<pcmbuff-2;i++) {
		crc = crc ^ cm_buffer[i];
	}
 	@endverbatim
 	Przykładowo:
 	@verbatim
    komenda |    89 81   |08FE
            |liczone CRC |<CRC><GUI_END_OF_COMMAND>
 	@endverbatim
*/

