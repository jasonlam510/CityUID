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

#include "load_scene.h"

#include "cityuid_app_i.h"

bool cityuid_load_scene_file_path_dialog(App* app);

void cityuid_load_scene_on_enter(void* context) {
    App* app = context;

    if(cityuid_load_scene_file_path_dialog(app)) {
        CityUIDLoadingResult loading_result = cityuid_load_from_path(app->cityuid, app->file_path);
        if(loading_result != CityUIDLoadingResultSuccess) {
            view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDLoadSceneFailedEvent);
            return;
        }
        CityUIDParsingResult parsing_result = cityuid_parse(app->cityuid, app->cityuid->carddata);
        if(parsing_result & CityUIDParsingResultErrorMagic) { // other parsing errors are not fatal
            view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDLoadSceneFailedEvent);
            return;
        }
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDLoadSceneSuccessEvent);
    } else {
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDLoadSceneCancelledEvent);
    }
}

bool cityuid_load_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case CityUIDLoadSceneSuccessEvent:
            scene_manager_next_scene(app->scene_manager, CityUIDResultsScene);
            consumed = true;
            break;
        case CityUIDLoadSceneFailedEvent:
            if(app->cityuid->loading_result != CityUIDLoadingResultSuccess) {
                cityuid_app_error_dialog(
                    app, cityuid_loading_error_string(app->cityuid->loading_result));
            } else { // parsing error
                cityuid_app_error_dialog(app, "Not a CityUID.");
            }
            cityuid_load_scene_on_enter(context); // reopen file dialog
            consumed = true;
            break;
        case CityUIDLoadSceneCancelledEvent:
            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }

    return consumed;
}

void cityuid_load_scene_on_exit(void* context) {
    App* app = context;

    UNUSED(app);
    // nothing to do here
}

bool cityuid_load_scene_file_path_dialog(App* app) {
    DialogsFileBrowserOptions browser_options;

    dialog_file_browser_set_basic_options(&browser_options, ".nfc", &I_Nfc_10x10);
    browser_options.base_path = EXT_PATH("nfc");
    browser_options.hide_dot_files = true;

    bool result = dialog_file_browser_show(
        app->dialogs_app, app->file_path, app->file_path, &browser_options);

    return result;
}
