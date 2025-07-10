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

#include "cityuid_app_i.h"

void (*const cityuid_scene_on_enter_handlers[])(void*) = {
    cityuid_main_menu_scene_on_enter,
    cityuid_detect_scene_on_enter,
    cityuid_read_scene_on_enter,
    cityuid_load_scene_on_enter,
    cityuid_results_scene_on_enter,
    cityuid_about_scene_on_enter};

bool (*const cityuid_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    cityuid_main_menu_scene_on_event,
    cityuid_detect_scene_on_event,
    cityuid_read_scene_on_event,
    cityuid_load_scene_on_event,
    cityuid_results_scene_on_event,
    cityuid_about_scene_on_event,
};

void (*const cityuid_scene_on_exit_handlers[])(void*) = {
    cityuid_main_menu_scene_on_exit,
    cityuid_detect_scene_on_exit,
    cityuid_read_scene_on_exit,
    cityuid_load_scene_on_exit,
    cityuid_results_scene_on_exit,
    cityuid_about_scene_on_exit};

static const SceneManagerHandlers cityuid_scene_manager_handlers = {
    .on_enter_handlers = cityuid_scene_on_enter_handlers,
    .on_event_handlers = cityuid_scene_on_event_handlers,
    .on_exit_handlers = cityuid_scene_on_exit_handlers,
    .scene_num = CityUIDSceneCount};

/* callbacks */

static bool cityuid_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    // passthrough
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool cityuid_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    // passthrough
    return scene_manager_handle_back_event(app->scene_manager);
}

/* App */

static App* app_alloc() {
    App* app = malloc(sizeof(App));

    app->scene_manager = scene_manager_alloc(&cityuid_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, cityuid_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, cityuid_back_event_callback);

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CityUIDSubmenuView, submenu_get_view(app->submenu));

    app->textbox = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CityUIDTextBoxView, text_box_get_view(app->textbox));

    app->popup = popup_alloc();
    view_dispatcher_add_view(app->view_dispatcher, CityUIDPopupView, popup_get_view(app->popup));

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CityUIDWidgetView, widget_get_view(app->widget));

    app->dialogs_app = furi_record_open(RECORD_DIALOGS);
    app->dialog_message = dialog_message_alloc();
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->nfc = nfc_alloc();
    app->nfc_scanner = NULL;
    app->nfc_poller = NULL;

    app->target_manager = NULL;

    app->file_path = furi_string_alloc_set(EXT_PATH("nfc"));
    app->cityuid = cityuid_alloc();

    return app;
}

static void app_free(App* app) {
    furi_assert(app);

    cityuid_free(app->cityuid);
    furi_string_free(app->file_path);

    nfc_free(app->nfc);

    furi_record_close(RECORD_NOTIFICATION);
    dialog_message_free(app->dialog_message);
    furi_record_close(RECORD_DIALOGS);
    app->dialogs_app = NULL;

    view_dispatcher_remove_view(app->view_dispatcher, CityUIDWidgetView);
    view_dispatcher_remove_view(app->view_dispatcher, CityUIDPopupView);
    view_dispatcher_remove_view(app->view_dispatcher, CityUIDSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, CityUIDTextBoxView);
    view_dispatcher_free(app->view_dispatcher);

    scene_manager_free(app->scene_manager);

    widget_free(app->widget);
    popup_free(app->popup);
    submenu_free(app->submenu);
    text_box_free(app->textbox);

    free(app);
}

/* entry point */

int32_t cityuid_app(void* p) {
    UNUSED(p);

    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);

    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, CityUIDMainMenuScene);

    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}

void cityuid_app_blink_start(App* app) {
    notification_message(app->notifications, &sequence_blink_start_cyan);
}

void cityuid_app_blink_stop(App* app) {
    notification_message(app->notifications, &sequence_blink_stop);
}

void cityuid_app_error_dialog(App* app, const char* message) {
    dialog_message_set_text(app->dialog_message, message, 60, 30, AlignCenter, AlignCenter);
    dialog_message_set_buttons(app->dialog_message, NULL, "OK", NULL);
    dialog_message_show(app->dialogs_app, app->dialog_message);
}
