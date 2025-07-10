/*
This file is part of CityUID App.
A Flipper Zero application to analyse student ID cards from the University of Duisburg-Essen (Intercard)

Copyright (C) 2025 Alexander Hahn

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

#ifndef UDECARD_H
#define UDECARD_H

#include <furi.h>

#include <nfc/nfc_device.h>

#define BLOCKS_PER_SECTOR 4

#define UDECARD_UID_BLOCK (0 * BLOCKS_PER_SECTOR + 0)
#define UDECARD_UID_SIZE  4

// used to identify whether the card is a CityUID
#define UDECARD_CONSTANT_BLOCK1 (0 * BLOCKS_PER_SECTOR + 1)
#define UDECARD_CONSTANT_BLOCK1_CONTENTS_BYTES \
    {0x55, 0x06, 0x38, 0x2a, 0x38, 0x2a, 0x38, 0x2a, 0x38, 0x2a, 0x38, 0x2a, 0x38, 0x2a, 0x38, 0x2a}
static const uint8_t UDECARD_CONSTANT_BLOCK1_CONTENTS[] = UDECARD_CONSTANT_BLOCK1_CONTENTS_BYTES;
#define UDECARD_CONSTANT_BLOCK1_SIZE 16

#define UDECARD_CONSTANT_BLOCK2 (0 * BLOCKS_PER_SECTOR + 2)
#define UDECARD_CONSTANT_BLOCK2_CONTENTS_BYTES \
    {0x38, 0x2a, 0x38, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
static const uint8_t UDECARD_CONSTANT_BLOCK2_CONTENTS[] = UDECARD_CONSTANT_BLOCK2_CONTENTS_BYTES;
#define UDECARD_CONSTANT_BLOCK2_SIZE 16

// membertype
#define UDECARD_MEMBERTYPE_BLOCK              (1 * BLOCKS_PER_SECTOR + 2)
#define UDECARD_MEMBERTYPE_BYTE_IN_BLOCK_1OCC 0
#define UDECARD_MEMBERTYPE_BYTE_IN_BLOCK_2OCC 2
#define UDECARD_MEMBER_TYPE_STUDENT_BYTE      0x02
#define UDECARD_MEMBER_TYPE_EMPLOYEE_BYTE     0x03

// balance
#define UDECARD_BALANCE_BLOCK_1OCC (3 * BLOCKS_PER_SECTOR + 0)
#define UDECARD_BALANCE_BYTE_1OCC  2
#define UDECARD_BALANCE_BLOCK_2OCC (3 * BLOCKS_PER_SECTOR + 1)
#define UDECARD_BALANCE_BYTE_2OCC  7

// transaction counter
#define UDECARD_TRANSCOUNT_BLOCK_1OCC (2 * BLOCKS_PER_SECTOR + 2)
#define UDECARD_TRANSCOUNT_BYTE_1OCC  13
#define UDECARD_TRANSCOUNT_BLOCK_2OCC (3 * BLOCKS_PER_SECTOR + 0)
#define UDECARD_TRANSCOUNT_BYTE_2OCC  10

// student number
#define UDECARD_MEMBER_NUMBER_BLOCK_1OCC (5 * BLOCKS_PER_SECTOR + 0)
#define UDECARD_MEMBER_NUMBER_BLOCK_2OCC (6 * BLOCKS_PER_SECTOR + 0)
#define UDECARD_MEMBER_NUMBER_BLOCK_3OCC (8 * BLOCKS_PER_SECTOR + 0)
#define UDECARD_MEMBER_NUMBER_BLOCK_4OCC (9 * BLOCKS_PER_SECTOR + 0)
#define UDECARD_MEMBER_NUMBER_SIZE       12

// key locations in flipper’s dictonary
#define FLIPPER_MFCLASSIC_DICT_PATH       "nfc/assets/mf_classic_dict.nfc"
#define FLIPPER_MFCLASSIC_DICT_TOTAL_KEYS 2042
#define UDECARD_KEY_SIZE                  6
#define UDECARD_KEYA_0_INDEX              3
#define UDECARD_KEYA_1_INDEX              206
#define UDECARD_KEYA_2_INDEX              208
#define UDECARD_KEYA_3_INDEX              212
#define UDECARD_KEYA_4_INDEX              214
#define UDECARD_KEYA_5_INDEX              431

#define UDECARD_KSNR_SIZE_MAX_LENGTH 10

#include <nfc/protocols/mf_classic/mf_classic.h>

// AFAIK the University also issues cash cards (for payment only)
// and library cards (for borrowing books).
// Pull requests or additional information is very appreciated!
typedef enum CityUIDMemberType {
    CityUIDMemberTypeStudent,
    CityUIDMemberTypeEmployee,
    CityUIDMemberTypeUnknown,
} CityUIDMemberType;

#define UDECARD_MEMBER_TYPE_TO_STRING(X)             \
    ((X) == CityUIDMemberTypeStudent  ? "Student" :  \
     (X) == CityUIDMemberTypeEmployee ? "Employee" : \
                                        "Unknown")

typedef enum CityUIDLoadingResult {
    CityUIDLoadingResultSuccess,
    CityUIDLoadingResultErrorNotNFC,
    CityUIDLoadingResultErrorNotMfClassic,
} CityUIDLoadingResult;

typedef enum CityUIDParsingResult {
    CityUIDParsingResultSuccess = 0,
    CityUIDParsingResultErrorMagic = 1 << 0,
    CityUIDParsingResultErrorKsnr = 1 << 1,
    CityUIDParsingResultErrorMemberType = 1 << 2,
    CityUIDParsingResultErrorBalance = 1 << 3,
    CityUIDParsingResultErrorMemberNumber = 1 << 4,
    CityUIDParsingResultErrorTransactionCount = 1 << 5
} CityUIDParsingResult;

typedef struct CityUID {
    MfClassicData* carddata;

    CityUIDLoadingResult loading_result;
    CityUIDParsingResult parsing_result;

    uint8_t uid[4];

    char ksnr[UDECARD_KSNR_SIZE_MAX_LENGTH + 1]; // KS-Nr. (Karten-Seriennummer)
    CityUIDMemberType member_type; // Stundent or Employee
    char member_number
        [UDECARD_MEMBER_NUMBER_SIZE +
         1]; // student number for students, employee number for employees
    uint16_t balance;
    uint16_t transaction_count;
} CityUID;

CityUID* cityuid_alloc();
void cityuid_free(CityUID* cityuid);
CityUIDParsingResult cityuid_parse(CityUID* cityuid, MfClassicData* mfclassicdata);
void uid_to_ksnr(char* ksnr, uint8_t* uid);
int xor3_to_int(const uint8_t* bytes);

CityUIDLoadingResult cityuid_load_from_nfc_device(CityUID* cityuid, NfcDevice* nfc_device);
CityUIDLoadingResult cityuid_load_from_path(CityUID* cityuid, FuriString* path);

char* cityuid_loading_error_string(CityUIDLoadingResult loading_result);
bool cityuid_gather_keys(uint8_t sector_keys[][6]);

#endif
