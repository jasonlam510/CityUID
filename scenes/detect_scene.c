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

#include "detect_scene.h"

#include "cityuid_app_i.h"

bool cityuid_detect_scene_is_event_from_mfc(NfcScannerEvent event) {
    if(event.type == NfcScannerEventTypeDetected) {
        for(unsigned int i = 0; i < event.data.protocol_num; i++) {
            if(event.data.protocols[i] == NfcProtocolMfClassic) {
                return true;
            }
        }
    }
    return false;
}

void cityuid_detect_scene_nfc_scanner_callback(NfcScannerEvent event, void* context) {
    App* app = context;

    switch(event.type) {
    case NfcScannerEventTypeDetected:
        if(cityuid_detect_scene_is_event_from_mfc(event)) { // mfclassic?
            view_dispatcher_send_custom_event(
                app->view_dispatcher, CityUIDDetectSceneDetectedEvent);
        } else {
            view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDDetectSceneErrorEvent);
        }
        break;
    default:
        break;
    }
}

void cityuid_detect_scene_on_enter(void* context) {
    App* app = context;

    popup_reset(app->popup);
    popup_set_header(app->popup, "Reading", 97, 15, AlignCenter, AlignTop);
    popup_set_text(
        app->popup, "Put CityU SID\nnext to\nFlipper's back", 94, 27, AlignCenter, AlignTop);
    popup_set_icon(app->popup, 0, 8, &I_ApplyFlipperToUDE_60x50);
    view_dispatcher_switch_to_view(app->view_dispatcher, CityUIDPopupView);

    if(!cityuid_gather_keys(app->sector_keys)) {
        cityuid_app_error_dialog(app, "Gathering keys failed.");
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDDetectSceneFatalErrorEvent);
    }

    // start nfc scanner
    app->nfc_scanner = nfc_scanner_alloc(app->nfc);
    nfc_scanner_start(app->nfc_scanner, cityuid_detect_scene_nfc_scanner_callback, app);

    cityuid_app_blink_start(app);
}

bool cityuid_detect_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case CityUIDDetectSceneDetectedEvent:
            scene_manager_next_scene(app->scene_manager, CityUIDReadScene);
            consumed = true;
            break;
        case CityUIDDetectSceneErrorEvent:
            nfc_scanner_stop(app->nfc_scanner);
            nfc_scanner_start(
                app->nfc_scanner, cityuid_detect_scene_nfc_scanner_callback, context);
            break;
        case CityUIDDetectSceneFatalErrorEvent:
            scene_manager_previous_scene(app->scene_manager);
        }
        break;
    default:
        break;
    }

    return consumed;
}

void cityuid_detect_scene_on_exit(void* context) {
    App* app = context;
    nfc_scanner_stop(app->nfc_scanner);
    nfc_scanner_free(app->nfc_scanner);

    popup_reset(app->popup);
    cityuid_app_blink_stop(app);
}
