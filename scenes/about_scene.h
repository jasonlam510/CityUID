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

#ifndef ABOUT_SCENE_H
#define ABOUT_SCENE_H

#include <gui/scene_manager.h>

#define VERSION "v1.0.3"
#define ABOUT_TEXT                                                          \
    "CityU Student Card Parser " VERSION "\n"                               \
    "by Jason Lam\n(github.com/jasonlam510)\n\n"                            \
    "Based on UDECard by Alexander Hahn\n(github.com/hahnworks)\n\n"        \
                                                                            \
    "Copyright (C) 2025\n"                                                  \
    "Jason Lam\n\n"                                                         \
    "This program is free software: you can redistribute it and/or modify " \
    "it under the terms of the GNU General Public License as published by " \
    "the Free Software Foundation, either version 3 of the License, or "    \
    "(at your option) any later version.\n\n"                               \
    "This program is distributed in the hope that it will be useful, "      \
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "       \
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "        \
    "GNU General Public License for more details."

void cityuid_about_scene_on_enter(void* context);
bool cityuid_about_scene_on_event(void* context, SceneManagerEvent event);
void cityuid_about_scene_on_exit(void* context);

#endif
