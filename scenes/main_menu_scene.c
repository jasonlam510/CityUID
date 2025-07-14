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

#include "main_menu_scene.h"

#include "../cityuid_app_i.h"

void cityuid_main_menu_callback(void* context, uint32_t index) {
    FURI_LOG_I("CityUID", "Main menu callback");
    App* app = context;

    switch(index) {
    case CityUIDMainMenuSceneReadEvent:
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDMainMenuSceneReadEvent);
        break;
    case CityUIDMainMenuSceneLoadEvent:
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDMainMenuSceneLoadEvent);
        break;
    case CityUIDMainMenuSceneAboutEvent:
        view_dispatcher_send_custom_event(app->view_dispatcher, CityUIDMainMenuSceneAboutEvent);
        break;
    }
}

void cityuid_main_menu_scene_on_enter(void* context) {
    App* app = context;

    submenu_reset(app->submenu);

    submenu_set_header(app->submenu, "CityUID");
    submenu_add_item(
        app->submenu, "Read", CityUIDMainMenuSceneItemRead, cityuid_main_menu_callback, app);
    submenu_add_item(
        app->submenu, "Load", CityUIDMainMenuSceneItemLoad, cityuid_main_menu_callback, app);
    submenu_add_item(
        app->submenu, "About", CityUIDMainMenuSceneItemAbout, cityuid_main_menu_callback, app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, CityUIDMainMenuScene));

    view_dispatcher_switch_to_view(app->view_dispatcher, CityUIDSubmenuView);
}

bool cityuid_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I("CityUID", "Main menu event.");
    App* app = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case CityUIDMainMenuSceneReadEvent:
            scene_manager_set_scene_state(
                app->scene_manager, CityUIDMainMenuScene, CityUIDMainMenuSceneItemRead);
            scene_manager_next_scene(app->scene_manager, CityUIDDetectScene);
            consumed = true;
            break;
        case CityUIDMainMenuSceneLoadEvent:
            scene_manager_set_scene_state(
                app->scene_manager, CityUIDMainMenuScene, CityUIDMainMenuSceneItemLoad);
            scene_manager_next_scene(app->scene_manager, CityUIDLoadScene);
            consumed = true;
            break;
        case CityUIDMainMenuSceneAboutEvent:
            scene_manager_set_scene_state(
                app->scene_manager, CityUIDMainMenuScene, CityUIDMainMenuSceneItemAbout);
            scene_manager_next_scene(app->scene_manager, CityUIDAboutScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }

    return consumed;
}

void cityuid_main_menu_scene_on_exit(void* context) {
    App* app = context;

    submenu_reset(app->submenu);
}
