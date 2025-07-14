/*
This file is part of CityUID App.
A Flipper Zero application to analyse student ID cards from City University of Hong Kong

Based on UDECard App by Alexander Hahn (https://github.com/hahnworks/UDECard)
Copyright (C) 2025 Jason Lam (github.com/jasonlam510)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CITYUID_H
#define CITYUID_H

#include <furi.h>

#include <nfc/nfc_device.h>

#define CITYUID_UID_BLOCK 0
#define CITYUID_UID_SIZE  4

// used to identify whether the card is a CityUID
#define CITYUID_CONSTANT_BLOCK2 2
#define CITYUID_CONSTANT_BLOCK2_CONTENTS_BYTES \
    {0x30, 0x31, 0x33, 0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}
static const uint8_t CITYUID_CONSTANT_BLOCK2_CONTENTS[] = CITYUID_CONSTANT_BLOCK2_CONTENTS_BYTES;
#define CITYUID_CONSTANT_BLOCK2_SIZE 16

// student ID
#define CITYUID_STUDENT_ID_BLOCK 1
#define CITYUID_STUDENT_ID_SIZE 12

// card expiry date
#define CITYUID_CARD_EXPIRY_BLOCK 4
#define CITYUID_CARD_EXPIRY_SIZE 12

// card issue date
#define CITYUID_CARD_ISSUE_BLOCK 5
#define CITYUID_CARD_ISSUE_SIZE 8

// student name (spans multiple blocks)
#define CITYUID_STUDENT_NAME_BLOCK1 12
#define CITYUID_STUDENT_NAME_BLOCK2 13
#define CITYUID_STUDENT_NAME_BLOCK3 14
#define CITYUID_STUDENT_NAME_BLOCK4 16
#define CITYUID_STUDENT_NAME_BLOCK5 17
#define CITYUID_STUDENT_NAME_TOTAL_SIZE 96

// admit date
#define CITYUID_ADMIT_DATE_BLOCK 29
#define CITYUID_ADMIT_DATE_SIZE 8

// key locations in flipper's dictionary (assuming same keys as UDE for now)
#define FLIPPER_MFCLASSIC_DICT_PATH       "nfc/assets/mf_classic_dict.nfc"
#define FLIPPER_MFCLASSIC_DICT_TOTAL_KEYS 2042
#define CITYUID_KEY_SIZE                  6
#define CITYUID_KEYA_0_INDEX              3
#define CITYUID_KEYA_1_INDEX              206
#define CITYUID_KEYA_2_INDEX              208
#define CITYUID_KEYA_3_INDEX              212
#define CITYUID_KEYA_4_INDEX              214
#define CITYUID_KEYA_5_INDEX              431

#include <nfc/protocols/mf_classic/mf_classic.h>

typedef enum CityUIDCardType {
    CityUIDCardTypeStudent,
    CityUIDCardTypeOthers,
    CityUIDCardTypeUnknown,
} CityUIDCardType;

#define CITYUID_CARD_TYPE_TO_STRING(X)             \
    ((X) == CityUIDCardTypeStudent  ? "Student" :  \
     (X) == CityUIDCardTypeOthers   ? "Others" :   \
                                      "Unknown")

typedef enum CityUIDLoadingResult {
    CityUIDLoadingResultSuccess,
    CityUIDLoadingResultErrorNotNFC,
    CityUIDLoadingResultErrorNotMfClassic,
} CityUIDLoadingResult;

typedef enum CityUIDParsingResult {
    CityUIDParsingResultSuccess = 0,
    CityUIDParsingResultErrorMagic = 1 << 0,
    CityUIDParsingResultErrorCardType = 1 << 1,
    CityUIDParsingResultErrorStudentId = 1 << 2,
    CityUIDParsingResultErrorCardExpiry = 1 << 3,
    CityUIDParsingResultErrorCardIssue = 1 << 4,
    CityUIDParsingResultErrorStudentName = 1 << 5,
    CityUIDParsingResultErrorAdmitDate = 1 << 6
} CityUIDParsingResult;

typedef struct CityUID {
    MfClassicData* carddata;

    CityUIDLoadingResult loading_result;
    CityUIDParsingResult parsing_result;

    uint8_t uid[4];

    CityUIDCardType card_type; // Student, Staff, Alumni, or Unknown
    char student_id[CITYUID_STUDENT_ID_SIZE + 1];        // "SDXXXXXXXX#"
    char card_expiry_date[CITYUID_CARD_EXPIRY_SIZE + 1]; // 'STA20250615'
    char card_issue_date[CITYUID_CARD_ISSUE_SIZE + 1];   // "YYYYMMDD"
    char student_name[CITYUID_STUDENT_NAME_TOTAL_SIZE + 1]; // Full name from blocks 12-17
    char admit_date[CITYUID_ADMIT_DATE_SIZE + 1];        // "YYYYMMDD"
} CityUID;

CityUID* cityuid_alloc();
void cityuid_free(CityUID* cityuid);
CityUIDParsingResult cityuid_parse(CityUID* cityuid, MfClassicData* mfclassicdata);

CityUIDLoadingResult cityuid_load_from_nfc_device(CityUID* cityuid, NfcDevice* nfc_device);
CityUIDLoadingResult cityuid_load_from_path(CityUID* cityuid, FuriString* path);

char* cityuid_loading_error_string(CityUIDLoadingResult loading_result);
bool cityuid_gather_keys(uint8_t sector_keys[][6]);

#endif
