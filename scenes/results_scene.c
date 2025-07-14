/*
This file is part of CityUID App.
A Flipper Zero application to analyse student ID cards from the University of Duisburg-Essen (Intercard)

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

#include "results_scene.h"

#include "cityuid_app_i.h"

#define CITYUID_STUDENT_ID_BLOCK 1
#define CITYUID_CARD_EXPIRY_BLOCK 4
#define CITYUID_CARD_ISSUE_BLOCK 5
#define CITYUID_STUDENT_NAME_BLOCK1 12
#define CITYUID_STUDENT_NAME_BLOCK2 13
#define CITYUID_STUDENT_NAME_BLOCK3 14
#define CITYUID_STUDENT_NAME_BLOCK4 16
#define CITYUID_STUDENT_NAME_BLOCK5 17
#define CITYUID_ADMIT_DATE_BLOCK 29

void cityuid_results_scene_on_enter(void* context) {
    App* app = context;
    CityUID* cityuid = app->cityuid;

    widget_reset(app->widget);

    // Header: SID: XXXXXXXX (numeric part only)
    char sid_header[32] = {0};
    char id_only[9] = {0};
    if(strlen(cityuid->student_id) >= 10) {
        strncpy(id_only, cityuid->student_id + 2, 8);
        id_only[8] = '\0';
    } else {
        strncpy(id_only, cityuid->student_id, sizeof(id_only) - 1);
    }
    snprintf(sid_header, sizeof(sid_header), "SID: %s", id_only);
    widget_add_string_element(app->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, sid_header);

    // Student Name
    char name_string[128];
    snprintf(name_string, sizeof(name_string), "Student Name: %s", cityuid->student_name);
    widget_add_string_element(app->widget, 0, 14, AlignLeft, AlignTop, FontSecondary, name_string);

    // Card Type
    char card_type_string[32];
    snprintf(card_type_string, sizeof(card_type_string), "Card Type: %s", CITYUID_CARD_TYPE_TO_STRING(cityuid->card_type));
    widget_add_string_element(app->widget, 0, 24, AlignLeft, AlignTop, FontSecondary, card_type_string);

    // Card Issue Number (last digit of student_id)
    char issue_number_string[32];
    char issue_number = (strlen(cityuid->student_id) >= 11) ? cityuid->student_id[10] : '?';
    snprintf(issue_number_string, sizeof(issue_number_string), "Card Issue Number: %c", issue_number);
    widget_add_string_element(app->widget, 0, 34, AlignLeft, AlignTop, FontSecondary, issue_number_string);

    // Expiry (skip first 3 chars if prefix is 'STA', show only YYYYMMDD)
    char expiry_string[32];
    if (strncmp(cityuid->card_expiry_date, "STA", 3) == 0 && strlen(cityuid->card_expiry_date) >= 11) {
        snprintf(expiry_string, sizeof(expiry_string), "Expiry: %.8s", cityuid->card_expiry_date + 3);
    } else {
        snprintf(expiry_string, sizeof(expiry_string), "Expiry: %s", cityuid->card_expiry_date);
    }
    widget_add_string_element(app->widget, 0, 44, AlignLeft, AlignTop, FontSecondary, expiry_string);

    // Admit Date
    char admit_string[32];
    snprintf(admit_string, sizeof(admit_string), "Admit Date: %s", cityuid->admit_date);
    widget_add_string_element(app->widget, 0, 54, AlignLeft, AlignTop, FontSecondary, admit_string);

    view_dispatcher_switch_to_view(app->view_dispatcher, CityUIDWidgetView);
}

bool cityuid_results_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeBack:
        if(!scene_manager_search_and_switch_to_previous_scene(
               app->scene_manager, CityUIDDetectScene))
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, CityUIDLoadScene);
        consumed = true;
    default:
        break;
    }

    return consumed;
}

void cityuid_results_scene_on_exit(void* context) {
    App* app = context;

    widget_reset(app->widget);
}
