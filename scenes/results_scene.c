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

void cityuid_results_scene_on_enter(void* context) {
    App* app = context;

    CityUID* cityuid = app->cityuid;

    widget_reset(app->widget);

    char ksnr_string[sizeof("KS-Nr. ") + UDECARD_KSNR_SIZE_MAX_LENGTH] = {0};
    char member_type_string[sizeof("[!?] Member type: Employee")] = {0};
    char member_number_string[sizeof("[!?] Employee No.: ") + UDECARD_MEMBER_NUMBER_SIZE] = {0};
    char balance_string[sizeof("[!?] Balance: 000.00 EUR")] = {0};
    char transaction_string[sizeof("[!?] Transactions: 00000")] = {0};
    char error_string[sizeof("(Parsing error: 000)")] = {0};

    snprintf(ksnr_string, sizeof(ksnr_string), "KS-Nr. %s", cityuid->ksnr);
    snprintf(
        member_type_string,
        sizeof(member_type_string),
        "Member type: %s %s",
        UDECARD_MEMBER_TYPE_TO_STRING(cityuid->member_type),
        cityuid->parsing_result & CityUIDParsingResultErrorMemberType ? "[!?]" : "");
    snprintf(
        member_number_string,
        sizeof(member_number_string),
        "%s No.: %s %s",
        UDECARD_MEMBER_TYPE_TO_STRING(cityuid->member_type),
        cityuid->member_number,
        cityuid->parsing_result & CityUIDParsingResultErrorMemberNumber ? "[!?]" : "");
    snprintf(
        balance_string,
        sizeof(balance_string),
        "Balance: %d.%02d EUR %s",
        cityuid->balance / 100,
        cityuid->balance % 100,
        cityuid->parsing_result & CityUIDParsingResultErrorBalance ? "[!?]" : "");
    snprintf(
        transaction_string,
        sizeof(transaction_string),
        "Transactions: %d %s",
        cityuid->transaction_count,
        cityuid->parsing_result & CityUIDParsingResultErrorTransactionCount ? "[!?]" : "");
    if(cityuid->parsing_result != CityUIDParsingResultSuccess)
        snprintf(
            error_string, sizeof(error_string), "(Parsing error: %03i)", cityuid->parsing_result);

    widget_add_string_element(app->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, ksnr_string);
    widget_add_string_element(
        app->widget, 0, 11, AlignLeft, AlignTop, FontSecondary, member_type_string);
    widget_add_string_element(
        app->widget, 0, 21, AlignLeft, AlignTop, FontSecondary, member_number_string);
    widget_add_string_element(
        app->widget, 0, 31, AlignLeft, AlignTop, FontSecondary, balance_string);
    widget_add_string_element(
        app->widget, 0, 41, AlignLeft, AlignTop, FontSecondary, transaction_string);
    widget_add_string_element(
        app->widget, 0, 51, AlignLeft, AlignTop, FontSecondary, error_string);

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
