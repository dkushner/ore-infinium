/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

class Settings
{
public:
    static Settings* instance();
    virtual ~Settings();

public:
    int screenResolutionWidth = 1600;
    int screenResolutionHeight = 900;

    //HACK? Question mark question mark.
    int windowWidth = 1600;//screenResolutionWidth;
    int windowHeight = 900;//screenResolutionHeight;

    int debugAreas;
    int debugRendererFlags;

//static int SDL_LOGPRIORITY = SDL_LOG_PRIORITY_WARN;

private:
    Settings();
    Settings(const Settings& tm) {};
    Settings& operator=(const Settings& tm);

    static Settings* s_instance;
};

#endif
