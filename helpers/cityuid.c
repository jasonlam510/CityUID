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

#include "cityuid.h"

#include "../cityuid_app_i.h"

void CityUID_demo_fill(CityUID* cityuid) {
    FURI_LOG_I("CITYUID", "Filling demo card data.");
    cityuid->parsing_result = CityUIDParsingResultSuccess;
    cityuid->loading_result = CityUIDLoadingResultSuccess;
    cityuid->carddata = NULL;
    cityuid->card_type = CityUIDCardTypeStudent;
    strncpy(cityuid->student_id, "SD123456780", CITYUID_STUDENT_ID_SIZE);
    cityuid->student_id[CITYUID_STUDENT_ID_SIZE] = '\0';
    strncpy(cityuid->card_expiry_date, "20250615", CITYUID_CARD_EXPIRY_SIZE);
    cityuid->card_expiry_date[CITYUID_CARD_EXPIRY_SIZE] = '\0';
    strncpy(cityuid->card_issue_date, "20230901", CITYUID_CARD_ISSUE_SIZE);
    cityuid->card_issue_date[CITYUID_CARD_ISSUE_SIZE] = '\0';
    strncpy(cityuid->student_name, "CHAN Tai Man", CITYUID_STUDENT_NAME_TOTAL_SIZE);
    cityuid->student_name[CITYUID_STUDENT_NAME_TOTAL_SIZE] = '\0';
    strncpy(cityuid->admit_date, "20220820", CITYUID_ADMIT_DATE_SIZE);
    cityuid->admit_date[CITYUID_ADMIT_DATE_SIZE] = '\0';
}

CityUID* cityuid_alloc() {
    CityUID* cityuid = malloc(sizeof(CityUID));

    cityuid->parsing_result = ~0;
    cityuid->loading_result = ~0;

    cityuid->carddata = mf_classic_alloc();
    memset(cityuid->uid, 0, sizeof(cityuid->uid));

    cityuid->card_type = CityUIDCardTypeUnknown;
    memset(cityuid->student_id, 0, sizeof(cityuid->student_id));
    memset(cityuid->card_expiry_date, 0, sizeof(cityuid->card_expiry_date));
    memset(cityuid->card_issue_date, 0, sizeof(cityuid->card_issue_date));
    memset(cityuid->student_name, 0, sizeof(cityuid->student_name));
    memset(cityuid->admit_date, 0, sizeof(cityuid->admit_date));

    return cityuid;
}

void cityuid_free(CityUID* cityuid) {
    mf_classic_free(cityuid->carddata);

    free(cityuid);
}

bool cityuid_parse_uid(CityUID* cityuid) {
    MfClassicData* mfclassicdata = cityuid->carddata;

    /* extract UID from block 0 */
    MfClassicBlock uid_block = mfclassicdata->block[CITYUID_UID_BLOCK];
    memcpy(cityuid->uid, uid_block.data, CITYUID_UID_SIZE);
    
    return true;
}

bool cityuid_parse_magic(CityUID* cityuid) {
    MfClassicData* mfclassicdata = cityuid->carddata;

    /* check for magic bytes */
    MfClassicBlock constant_block2 = mfclassicdata->block[CITYUID_CONSTANT_BLOCK2];
    if(memcmp(
           constant_block2.data, CITYUID_CONSTANT_BLOCK2_CONTENTS, CITYUID_CONSTANT_BLOCK2_SIZE) !=
           0) {
        // if we are here, this card is probably not a CityUID
        return false;
    }
    return true;
}

// Forward declaration
bool cityuid_parse_student_id(CityUID* cityuid);

bool cityuid_parse_card_type(CityUID* cityuid) {
    // First, we need to parse the student ID to get the prefix
    if(!cityuid_parse_student_id(cityuid)) {
        cityuid->card_type = CityUIDCardTypeUnknown;
        return false;
    }

    // Identify card type based on the prefix
    if(strncmp(cityuid->student_id, "SD", 2) == 0) {
        cityuid->card_type = CityUIDCardTypeStudent;
    } else {
        // Any other prefix (ST, AL, etc.) will be classified as "Others"
        cityuid->card_type = CityUIDCardTypeOthers;
    }

    return true;
}

bool cityuid_parse_student_id(CityUID* cityuid) {
    MfClassicData* mfclassicdata = cityuid->carddata;

    MfClassicBlock student_id_block = mfclassicdata->block[CITYUID_STUDENT_ID_BLOCK];
    
    // Debug: Print raw data from Block 1 (Student ID)
    FURI_LOG_I("CITYUID", "Raw student ID block data (Block 1):");
    for(int i = 0; i < 16; i++) {
        FURI_LOG_I("CITYUID", "  [%d]: %02X '%c'", i, student_id_block.data[i], student_id_block.data[i]);
    }
    
    // Also check Block 4 (Card Expiry) for comparison
    MfClassicBlock expiry_block = mfclassicdata->block[CITYUID_CARD_EXPIRY_BLOCK];
    FURI_LOG_I("CITYUID", "Raw card expiry block data (Block 4):");
    for(int i = 0; i < 16; i++) {
        FURI_LOG_I("CITYUID", "  [%d]: %02X '%c'", i, expiry_block.data[i], expiry_block.data[i]);
    }
    
    strncpy(cityuid->student_id, (char*)student_id_block.data, CITYUID_STUDENT_ID_SIZE);
    cityuid->student_id[CITYUID_STUDENT_ID_SIZE] = '\0';

    FURI_LOG_I("CITYUID", "Before trimming: '%s'", cityuid->student_id);

    // Remove trailing spaces
    remove_trailing_spaces_from_string(cityuid->student_id);

    FURI_LOG_I("CITYUID", "After trimming: '%s'", cityuid->student_id);

    // Validate format: SDXXXXXXXX#
    if(strlen(cityuid->student_id) < 3 || 
       cityuid->student_id[0] != 'S' || 
       cityuid->student_id[1] != 'D') {
        FURI_LOG_W("CITYUID", "Student ID validation failed: length=%zu, first_char='%c', second_char='%c'", 
                   strlen(cityuid->student_id), cityuid->student_id[0], cityuid->student_id[1]);
        return false;
    }

    FURI_LOG_I("CITYUID", "Student ID parsed successfully: '%s'", cityuid->student_id);
    return true;
}

bool cityuid_parse_card_expiry_date(CityUID* cityuid) {
    MfClassicData* mfclassicdata = cityuid->carddata;

    MfClassicBlock expiry_block = mfclassicdata->block[CITYUID_CARD_EXPIRY_BLOCK];
    
    // Copy the full expiry field (up to 11 chars + null terminator)
    strncpy(cityuid->card_expiry_date, (char*)expiry_block.data, 11);
    cityuid->card_expiry_date[11] = '\0';
    remove_trailing_spaces_from_string(cityuid->card_expiry_date);

    // Validate: must be at least 11 chars and start with 'STA'
    if(strlen(cityuid->card_expiry_date) < 11 || strncmp(cityuid->card_expiry_date, "STA", 3) != 0) {
        return false;
    }

    return true;
}

bool cityuid_parse_card_issue_date(CityUID* cityuid) {
    MfClassicData* mfclassicdata = cityuid->carddata;

    MfClassicBlock issue_block = mfclassicdata->block[CITYUID_CARD_ISSUE_BLOCK];
    strncpy(cityuid->card_issue_date, (char*)issue_block.data, CITYUID_CARD_ISSUE_SIZE);
    cityuid->card_issue_date[CITYUID_CARD_ISSUE_SIZE] = '\0';

    // Remove trailing spaces
    remove_trailing_spaces_from_string(cityuid->card_issue_date);

    // Validate YYYYMMDD format
    if(strlen(cityuid->card_issue_date) != 8) {
        return false;
    }

    return true;
}

bool cityuid_parse_student_name(CityUID* cityuid) {
    MfClassicData* mfclassicdata = cityuid->carddata;

    // Combine data from blocks 12, 13, 14, 16, 17 (skip block 15 which is sector trailer)
    char* name_buffer = cityuid->student_name;
    int offset = 0;

    // Block 12
    MfClassicBlock name_block1 = mfclassicdata->block[CITYUID_STUDENT_NAME_BLOCK1];
    memcpy(name_buffer + offset, name_block1.data, 16);
    offset += 16;

    // Block 13
    MfClassicBlock name_block2 = mfclassicdata->block[CITYUID_STUDENT_NAME_BLOCK2];
    memcpy(name_buffer + offset, name_block2.data, 16);
    offset += 16;

    // Block 14
    MfClassicBlock name_block3 = mfclassicdata->block[CITYUID_STUDENT_NAME_BLOCK3];
    memcpy(name_buffer + offset, name_block3.data, 16);
    offset += 16;

    // Block 16 (skip block 15)
    MfClassicBlock name_block4 = mfclassicdata->block[CITYUID_STUDENT_NAME_BLOCK4];
    memcpy(name_buffer + offset, name_block4.data, 16);
    offset += 16;

    // Block 17
    MfClassicBlock name_block5 = mfclassicdata->block[CITYUID_STUDENT_NAME_BLOCK5];
    memcpy(name_buffer + offset, name_block5.data, 16);
    offset += 16;

    // Null terminate
    cityuid->student_name[CITYUID_STUDENT_NAME_TOTAL_SIZE] = '\0';

    // Remove trailing spaces
    remove_trailing_spaces_from_string(cityuid->student_name);

    return true;
}

bool cityuid_parse_admit_date(CityUID* cityuid) {
    MfClassicData* mfclassicdata = cityuid->carddata;

    MfClassicBlock admit_block = mfclassicdata->block[CITYUID_ADMIT_DATE_BLOCK];
    strncpy(cityuid->admit_date, (char*)admit_block.data, CITYUID_ADMIT_DATE_SIZE);
    cityuid->admit_date[CITYUID_ADMIT_DATE_SIZE] = '\0';

    // Remove trailing spaces
    remove_trailing_spaces_from_string(cityuid->admit_date);

    // Validate YYYYMMDD format
    if(strlen(cityuid->admit_date) != 8) {
        return false;
    }

    return true;
}

CityUIDParsingResult cityuid_parse(CityUID* cityuid, MfClassicData* mfclassicdata) {
    cityuid->carddata = mfclassicdata;

    cityuid->parsing_result = CityUIDParsingResultSuccess;

    // Log the entire raw card data (all blocks as hex)
    FURI_LOG_I("CITYUID", "--- RAW CARD DATA ---");
    for(int i = 0; i < 64; i++) { // Mifare Classic 1K has 64 blocks
        char hexbuf[64] = {0};
        char* p = hexbuf;
        for(int j = 0; j < 16; j++) {
            p += snprintf(p, sizeof(hexbuf) - (p-hexbuf), "%02X ", mfclassicdata->block[i].data[j]);
        }
        FURI_LOG_I("CITYUID", "Block %02d: %s", i, hexbuf);
    }
    FURI_LOG_I("CITYUID", "--- END RAW CARD DATA ---");

    FURI_LOG_I("CITYUID", "Starting card parsing...");

    bool (*parse_functions[])(CityUID*) = {
        cityuid_parse_uid,            // Parse UID first
        cityuid_parse_magic,
        cityuid_parse_card_type,      // This will also parse student_id internally
        cityuid_parse_card_expiry_date,
        cityuid_parse_card_issue_date,
        cityuid_parse_student_name,
        cityuid_parse_admit_date,
    };

    const char* function_names[] = {
        "UID",
        "Magic",
        "Card Type",
        "Card Expiry",
        "Card Issue",
        "Student Name",
        "Admit Date",
    };

    for(unsigned int i = 0; i < sizeof(parse_functions) / sizeof(parse_functions[0]); i++) {
        FURI_LOG_I("CITYUID", "Parsing %s...", function_names[i]);
        if(!parse_functions[i](cityuid)) {
            FURI_LOG_W("CITYUID", "Failed to parse %s", function_names[i]);
            cityuid->parsing_result |= (1 << i);
        } else {
            FURI_LOG_I("CITYUID", "Successfully parsed %s", function_names[i]);
        }
    }

    FURI_LOG_I("CITYUID", "Parsing complete. Result: %d", cityuid->parsing_result);

    // Comment out demo data to see real parsing results
    // CityUID_demo_fill(cityuid);

    return cityuid->parsing_result;
}

CityUIDLoadingResult cityuid_load_from_nfc_device(CityUID* cityuid, NfcDevice* nfc_device) {
    if(!nfc_device) return cityuid->loading_result = CityUIDLoadingResultErrorNotNFC;

    FURI_LOG_I(
        "CITYUID", "Loading from NFC device, protocol: %d", nfc_device_get_protocol(nfc_device));

    if(nfc_device_get_protocol(nfc_device) != NfcProtocolMfClassic) {
        return cityuid->loading_result = CityUIDLoadingResultErrorNotMfClassic;
    }

    const MfClassicData* c_mfclassicdata = nfc_device_get_data(nfc_device, NfcProtocolMfClassic);
    mf_classic_copy(cityuid->carddata, c_mfclassicdata);

    return cityuid->loading_result = CityUIDLoadingResultSuccess;
}

CityUIDLoadingResult cityuid_load_from_path(CityUID* cityuid, FuriString* path) {
    NfcDevice* nfc_device = nfc_device_alloc();
    if(!nfc_device_load(nfc_device, furi_string_get_cstr(path)))
        return cityuid->loading_result = CityUIDLoadingResultErrorNotNFC;

    if((cityuid->loading_result = cityuid_load_from_nfc_device(cityuid, nfc_device)) !=
       CityUIDLoadingResultSuccess) {
        nfc_device_free(nfc_device);
        return cityuid->loading_result;
    }

    nfc_device_free(nfc_device);

    return cityuid->loading_result;
}

char* cityuid_loading_error_string(CityUIDLoadingResult loading_result) {
    switch(loading_result) {
    case CityUIDLoadingResultErrorNotNFC:
        return "Not an NFC file.";
    case CityUIDLoadingResultErrorNotMfClassic:
        return "Not a MifareClassic card.";
    default:
        return "Unknown error.";
    }
}

bool cityuid_gather_keys(uint8_t sector_keys[][6]) {
    if(!keys_dict_check_presence(EXT_PATH(FLIPPER_MFCLASSIC_DICT_PATH))) {
        FURI_LOG_E(
            "CITYUID", "Keys dictionary not found at %s", EXT_PATH(FLIPPER_MFCLASSIC_DICT_PATH));
        return false;
    }
    KeysDict* keys_dict =
        keys_dict_alloc(EXT_PATH(FLIPPER_MFCLASSIC_DICT_PATH), KeysDictModeOpenExisting, 6);
    if(!keys_dict) {
        FURI_LOG_E(
            "CITYUID",
            "Failed to open keys dictionary at %s",
            EXT_PATH(FLIPPER_MFCLASSIC_DICT_PATH));
        return false;
    }
    if(FLIPPER_MFCLASSIC_DICT_TOTAL_KEYS != keys_dict_get_total_keys(keys_dict)) {
        FURI_LOG_I(
            "CITYUID",
            "Keys dictionary at %s has wrong number of keys: %d, expected: %d",
            EXT_PATH(FLIPPER_MFCLASSIC_DICT_PATH),
            keys_dict_get_total_keys(keys_dict),
            FLIPPER_MFCLASSIC_DICT_TOTAL_KEYS);
        // only throw an error if it is LESS
        if(keys_dict_get_total_keys(keys_dict) < FLIPPER_MFCLASSIC_DICT_TOTAL_KEYS) {
            FURI_LOG_E("CITYUID", "Keys dictionary is too small!");
            keys_dict_free(keys_dict);
            return false;
        }
    }

    int cityuid_keys_indices[] = {
        CITYUID_KEYA_0_INDEX,
        CITYUID_KEYA_1_INDEX,
        CITYUID_KEYA_2_INDEX,
        CITYUID_KEYA_3_INDEX,
        CITYUID_KEYA_4_INDEX,
        CITYUID_KEYA_5_INDEX,
    };
    int cityuid_keys_total = sizeof(cityuid_keys_indices) / sizeof(cityuid_keys_indices[0]);
    uint8_t gathered_keys[cityuid_keys_total][16];

    uint8_t curkey[6] = {0};
    int found = 0;
    for(int i = 1;
        keys_dict_get_next_key(keys_dict, curkey, sizeof(curkey)) && found < cityuid_keys_total;
        i++) {
        if(cityuid_keys_indices[found] == i) {
            memcpy(gathered_keys[found], curkey, sizeof(curkey));
            found++;
        }
    }

    for(int i = 0; i < cityuid_keys_total; i++) {
        FURI_LOG_I(
            "CITYUID",
            "Key %d: %02X %02X %02X %02X %02X %02X",
            i,
            gathered_keys[i][0],
            gathered_keys[i][1],
            gathered_keys[i][2],
            gathered_keys[i][3],
            gathered_keys[i][4],
            gathered_keys[i][5]);
    }

    memcpy(sector_keys[0], gathered_keys[0], sizeof(gathered_keys[0]));
    memcpy(sector_keys[1], gathered_keys[5], sizeof(gathered_keys[5]));
    memcpy(sector_keys[2], gathered_keys[5], sizeof(gathered_keys[5]));
    memcpy(sector_keys[3], gathered_keys[5], sizeof(gathered_keys[5]));
    // 4
    memcpy(sector_keys[5], gathered_keys[1], sizeof(gathered_keys[1]));
    memcpy(sector_keys[6], gathered_keys[2], sizeof(gathered_keys[2]));
    // 7
    memcpy(sector_keys[8], gathered_keys[3], sizeof(gathered_keys[3]));
    memcpy(sector_keys[9], gathered_keys[4], sizeof(gathered_keys[4]));
    // 10...15

    keys_dict_free(keys_dict);

    return true;
}
