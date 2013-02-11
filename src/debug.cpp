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

#include "debug.h"
#include "game.h"
#include "settings/settings.h"

#include <GL/glew.h>

#include <assert.h>
#include <iostream>
#include <sstream>

#ifndef NDEBUG
#endif

LogStream Debug::log(Debug::Area area)
{
    return LogStream(area);
}

void Debug::assertf(bool value, const std::string& message)
{
    if (!value) {
        std::cout << message << "\n";
        assert(0);
    }
}

void Debug::fatal(bool value, Debug::Area area, const std::string& message)
{
    if (!value) {
        Debug::log(area) << "FATAL: " << message;
        assert(0);
    }
}

void Debug::checkGLError()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        Debug::log(Debug::Area::Graphics) << gluErrorString(error);
        assert(0);
    }
}

void Debug::checkSDLError()
{
    std::string error = SDL_GetError();
    if (*error.c_str() != '\0') {
        Debug::log(Debug::Area::System) << "SDL Error: " << error;
        SDL_ClearError();
    }
}

//COLOR HOWTO: http://www.ibm.com/developerworks/linux/library/l-tip-prompt/ , only for linux..obviously
void Debug::glDebugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam)
{
    if (Settings::instance()->debugRendererOutput) {
        std::string sourceString;

        sourceString.append("\e[32;40m");
        sourceString.append("\nError Source:");

        switch(source) {
            case GL_DEBUG_SOURCE_API:
                sourceString.append("\e[37;40m");
                sourceString.append("[API]");
                sourceString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                sourceString.append("\e[37;40m");
                sourceString.append("[WINDOW SYSTEM]");
                sourceString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                sourceString.append("\e[37;40m");
                sourceString.append("[SHADER COMPILER]");
                sourceString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_SOURCE_THIRD_PARTY:
                sourceString.append("\e[37;40m");
                sourceString.append("[THIRD_PARTY]");
                sourceString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_SOURCE_APPLICATION:
                sourceString.append("\e[37;40m");
                sourceString.append("[APPLICATION]");
                sourceString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_SOURCE_OTHER:
                sourceString.append("\e[37;40m");
                sourceString.append("[OTHER]");
                sourceString.append("\e[37;40m\e[0m");
                break;
        }

        sourceString.append("\e[32;40m\e[0m");

        std::string severityString;
        severityString.append("\e[32;40m");
        severityString.append("\nError Source:");

        switch(severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                severityString.append("\e[37;40m");
                severityString.append("[HIGH]");
                severityString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_SEVERITY_MEDIUM:
                severityString.append("\e[37;40m");
                severityString.append("[MEDIUM]");
                severityString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_SEVERITY_LOW:
                severityString.append("\e[37;40m");
                severityString.append("[LOW]");
                severityString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_SEVERITY_NOTIFICATION:
                severityString.append("\e[37;40m");
                severityString.append("[NOTIFICATION]");
                severityString.append("\e[37;40m\e[0m");
                break;
        }

        severityString.append("\e[32;40m\e[0m");

        std::string typeString;
        typeString.append("\e[32;40m");
        typeString.append("\nError Source:");

        switch(type) {
            case GL_DEBUG_TYPE_ERROR:
                typeString.append("\e[37;40m");
                typeString.append("[ERROR]");
                typeString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                typeString.append("\e[37;40m");
                typeString.append("[DEPRECATED BEHAVIOR]");
                typeString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                typeString.append("\e[37;40m");
                typeString.append("[UNDEFINED BEHAVIOR]");
                typeString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_TYPE_PORTABILITY:
                typeString.append("\e[37;40m");
                typeString.append("[PORTABILITY]");
                typeString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_TYPE_PERFORMANCE:
                typeString.append("\e[37;40m");
                typeString.append("[PERFORMANCE]");
                typeString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_TYPE_OTHER:
                typeString.append("\e[37;40m");
                typeString.append("[OTHER]");
                typeString.append("\e[37;40m\e[0m");
                break;

            case GL_DEBUG_TYPE_MARKER:
                typeString.append("\e[37;40m");
                typeString.append("[MARKER]");
                typeString.append("\e[37;40m\e[0m");
                break;
        }

        typeString.append("\e[32;40m\e[0m");

        std::string messageString;
        messageString.append("\e[32;40m");
        messageString.append("\nError Message: ");
        messageString.append("\e[32;40m\e[0m");

        messageString.append("\e[30;41m");
        messageString.append(message);
        messageString.append("\e[30;41m\e[0m");

        std::string idString;
        idString.append("\e[32;40m");
        idString.append("\nError ID: ");
        idString.append("\e[32;40m\e[0m");

        log(Debug::Area::Graphics) << "\nOpenGL Error Report: " << sourceString << typeString << idString << severityString << messageString << "\n";
    }
}

////////////////////////////////////////////////////////////////// LogStream class ///////////////////////////////////////////////////////////////////////////

LogStream::LogStream(Debug::Area area) : m_area(area)
{
}

LogStream::~LogStream()
{
    if (Settings::instance()->debugOutput || Settings::instance()->debugRendererOutput) {
        std::string areaString;

        areaString.append("\e[36;40m");
        switch (m_area) {
        case Debug::Area::General:
            areaString.append("[General]");
            break;
        case Debug::Area::Network:
            areaString.append("[Network]");
            break;
        case Debug::Area::NetworkClient:
            areaString.append("[Network::Client]");
            break;
        case Debug::Area::NetworkServer:
            areaString.append("[Network::Server]");
            break;
        case Debug::Area::Graphics:
            areaString.append("[Graphics]");
            break;
        case Debug::Area::Physics:
            areaString.append("[Physics]");
            break;
        case Debug::Area::Sound:
            areaString.append("[Sound]");
            break;
        case Debug::Area::System:
            areaString.append("[System]");
            break;
        }

        areaString.append("\e[36;40m\e[0m");

        std::string msg = str();

        std::string preFormatter;

        preFormatter.append("\e[34;40m");
        preFormatter.append("\n====== ");
        preFormatter.append("\e[34;40m\e[0m");

        std::string postFormatter;
        postFormatter.append("\e[34;40m");
        postFormatter.append(" |========\n");
        postFormatter.append("\e[34;40m\e[0m");

        std::cout << preFormatter << areaString << " | " << str() << postFormatter;
    }
}

LogStream::LogStream(const LogStream& stream): std::stringstream(stream.str())
{
    m_area = stream.m_area;
}
