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

#include "read_scene.h"

#include "cityuid_app_i.h"

NfcCommand cityuid_read_scene_nfc_poller_callback(NfcGenericEvent event, void* context) {
    App* app = context;

    NfcCommand command = NfcCommandContinue;

    MfClassicPollerEvent* mfc_event = event.event_data;

    ReadTarget* target = NULL;

    switch(mfc_event->type) {
    case MfClassicPollerEventTypeCardDetected:
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDReadSceneCardDetectedEvent);
        command = NfcCommandContinue;
        break;

    case MfClassicPollerEventTypeCardLost:
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDReadSceneCardLostEvent);
        command = NfcCommandStop;
        break;

    case MfClassicPollerEventTypeRequestMode:
        // poller wants mode, let's give him one
        mfc_event->data->poller_mode.mode = MfClassicPollerModeRead;
        break;

    case MfClassicPollerEventTypeRequestReadSector:
        // poller requests data to read sector
        target = read_target_manager_get_next(app->target_manager);
        if(target == NULL) {
            // no more targets, stop poller
            mfc_event->data->read_sector_request_data.key_provided = false;
            view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDReadSceneDoneEvent);
            break;
        }
        // configure read
        mfc_event->data->read_sector_request_data.sector_num = target->sector_num;
        mfc_event->data->read_sector_request_data.key = target->keyA;
        mfc_event->data->read_sector_request_data.key_type = MfClassicKeyTypeA;
        mfc_event->data->read_sector_request_data.key_provided = true;
        break;

    case MfClassicPollerEventTypeSuccess:
        // save read data
        command = NfcCommandStop;
        nfc_device_set_data(
            app->nfc_device, NfcProtocolMfClassic, nfc_poller_get_data(app->nfc_poller));
        break;

    case MfClassicPollerEventTypeFail:
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDReadSceneErrorEvent);
        break;
    default:
        break;
    }

    return command;
}

void cityuid_read_scene_on_enter(void* context) {
    App* app = context;

    popup_reset(app->popup);
    popup_set_header(app->popup, "Don't move", 97, 15, AlignCenter, AlignTop);
    popup_set_icon(app->popup, 0, 8, &I_loading_24x24);
    view_dispatcher_switch_to_view(app->view_dispatcher, CityUIDPopupView);

    // set read targets
    app->target_manager = read_target_manager_alloc();
    app->target_manager->head = read_target_alloc(0, app->sector_keys[0]);
    for(int i = 1; i < 10; i++) {
        if(i == 4 || i == 7) continue; // skip 4 and 7
        read_target_append(app->target_manager->head, read_target_alloc(i, app->sector_keys[i]));
    }

    // create nfc device for data saving
    app->nfc_device = nfc_device_alloc();

    // start nfc poller
    app->nfc_poller = nfc_poller_alloc(app->nfc, NfcProtocolMfClassic);
    nfc_poller_start(app->nfc_poller, cityuid_read_scene_nfc_poller_callback, app);

    cityuid_app_blink_start(app);
}

bool cityuid_read_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case CityUIDReadSceneCardDetectedEvent:
            consumed = true;
            break;
        case CityUIDReadSceneCardLostEvent:
        case CityUIDReadSceneErrorEvent:
            cityuid_app_blink_stop(app);
            cityuid_app_error_dialog(app, "Card lost / Error occured.");
            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
            break;
        case CityUIDReadSceneDoneEvent:
            cityuid_app_blink_stop(app);
            dolphin_deed(DolphinDeedNfcReadSuccess);

            // parse card data
            CityUIDLoadingResult loading_result =
                cityuid_load_from_nfc_device(app->cityuid, app->nfc_device);
            if(loading_result != CityUIDLoadingResultSuccess) {
                view_dispatcher_send_custom_event(
                    app->view_dispatcher, CityUIDReadSceneLoadFailedEvent);
                return true;
            }
            CityUIDParsingResult parsing_result =
                cityuid_parse(app->cityuid, app->cityuid->carddata);
            if(parsing_result &
               CityUIDParsingResultErrorMagic) { // other parsing errors are not fatal
                view_dispatcher_send_custom_event(
                    app->view_dispatcher, CityUIDReadSceneLoadFailedEvent);
                return true;
            } else {
                view_dispatcher_send_custom_event(
                    app->view_dispatcher, CityUIDReadSceneLoadSuccessEvent);
            }
            consumed = true;
            break;

        case CityUIDReadSceneLoadFailedEvent:
            if(app->cityuid->loading_result != CityUIDLoadingResultSuccess) {
                cityuid_app_error_dialog(
                    app, cityuid_loading_error_string(app->cityuid->loading_result));
            } else { // parsing error
                cityuid_app_error_dialog(app, "Not a CityUID.");
            }
            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
            break;

        case CityUIDReadSceneLoadSuccessEvent:
            notification_message(app->notifications, &sequence_single_vibro);
            scene_manager_next_scene(app->scene_manager, CityUIDResultsScene);
            consumed = true;
            break;

        default:
            break;
        }
        break;
    default:
        break;
    }

    return consumed;
}

void cityuid_read_scene_on_exit(void* context) {
    App* app = context;

    nfc_poller_stop(app->nfc_poller);
    nfc_poller_free(app->nfc_poller);
    nfc_device_free(app->nfc_device);

    read_target_manager_free(app->target_manager);

    popup_reset(app->popup);
    cityuid_app_blink_stop(app);
}
