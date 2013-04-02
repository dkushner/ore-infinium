/******************************************************************************
 *   Copyright (C) 2012 by Shaun Reich <sreich@kde.org>                       *
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

#ifndef DEBUG_H
#define DEBUG_H

#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>

class LogStream;

class Debug
{
public:
    enum Area
    {
        ClientRendererArea = 1 << 0,
        TileRendererArea = 1 << 1,
        SpriteSheetRendererArea = 1 << 2,
        LightingRendererArea = 1 << 3,
        PhysicsArea = 1 << 4,
        AudioArea = 1 << 5,
        AudioLoaderArea = 1 << 6,
        GUILoggerArea = 1 << 7,
        ShadersArea = 1 << 8,
        NetworkClientInitialArea = 1 << 9,
        NetworkServerInitialArea = 1 << 10,
        NetworkClientContinuousArea = 1 << 11,
        NetworkServerContinuousArea = 1 << 12,
        ClientInventoryArea = 1 << 13,
        ServerInventoryArea = 1 << 14,
        ServerEntityLogicArea = 1 << 15,
        ImageLoaderArea = 1 << 16,
        WorldGeneratorArea = 1 << 17,
        WorldLoaderArea = 1 << 18,
        ClientEntityCreationArea = 1 << 19,
        ServerEntityCreationArea = 1 << 20,
        SettingsArea = 1 << 21,
        StartupArea = 1 << 22
    };

    static LogStream log(Area area);

    static void assertf(bool value, const std::string& message);
    static void fatal(bool value, Area area, const std::string& message);

    static void checkGLError();
    static void checkSDLError();

    static void glDebugCallback(unsigned int source, unsigned int type,
                                unsigned int id, unsigned int severity,
                                int length, const char* message, void* userParam);
};

class LogStream : public std::stringstream
{
public:
    LogStream(Debug::Area area);
    ~LogStream();

    LogStream(const LogStream& stream);

private:
    Debug::Area m_area;
};

#endif
