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

void Debug::assertf(bool value, std::string message)
{
    if (!value) {
        std::cout << message << "\n";
        assert(0);
    }
}

void Debug::fatal(bool value, Debug::Area area, std::string message)
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

void Debug::glDebugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam)
{
    std::string sourceString;

    sourceString.append("\e[35;40m");
    sourceString.append("\nError Source:");

    switch(source) {
        case GL_DEBUG_SOURCE_API:
            sourceString.append("[API]");
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceString.append("[WINDOW SYSTEM]");
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceString.append("[SHADER COMPILER]");
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceString.append("[THIRD_PARTY]");
            break;

         case GL_DEBUG_SOURCE_APPLICATION:
            sourceString.append("[APPLICATION]");
            break;

         case GL_DEBUG_SOURCE_OTHER:
            sourceString.append("[OTHER]");
            break;
    }

    sourceString.append("\e[35;40m\e[0m");

    std::string severityString;
    severityString.append("\e[35;40m");
    severityString.append("\nError Source:");

    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            severityString.append("[HIGH]");
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            severityString.append("[MEDIUM]");
            break;

        case GL_DEBUG_SEVERITY_LOW:
            severityString.append("[LOW]");
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severityString.append("[NOTIFICATION]");
            break;
    }

    severityString.append("\e[35;40m\e[0m");

    std::string typeString;
    typeString.append("\e[35;40m");
    typeString.append("\nError Source:");

    switch(type) {
        case GL_DEBUG_TYPE_ERROR:
            typeString.append("[ERROR]");
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeString.append("[DEPRECATED BEHAVIOR]");
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeString.append("[UNDEFINED BEHAVIOR]");
            break;

        case GL_DEBUG_TYPE_PORTABILITY:
            typeString.append("[PORTABILITY]");
            break;

         case GL_DEBUG_TYPE_PERFORMANCE:
            typeString.append("[PERFORMANCE]");
            break;

         case GL_DEBUG_TYPE_OTHER:
            typeString.append("[OTHER]");
            break;

        case GL_DEBUG_TYPE_MARKER:
            typeString.append("[MARKER]");
            break;
    }

    typeString.append("\e[35;40m\e[0m");

    std::string messageString;
    messageString.append("\e[37;40m");
    messageString.append("\nError Message: ");
    messageString.append("\e[37;40m\e[0m");

    messageString.append("\e[30;41m");
    messageString.append(message);
    messageString.append("\e[30;41m\e[0m");

    log(Debug::Area::Graphics) << "\nOpenGL Error Report: " << sourceString << typeString << "\nError ID: " << id << severityString << messageString << "\n";
}

////////////////////////////////////////////////////////////////// LogStream class ///////////////////////////////////////////////////////////////////////////

LogStream::LogStream(Debug::Area area) : m_area(area)
{
}

LogStream::~LogStream()
{
    std::string areaString;

    areaString.append("\e[36;40m");
    switch (m_area) {
    case Debug::Area::General:
        areaString.append("[General]");
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
    std::cout << "\n======= " << areaString << " | " << str() << "| ========" << "\n";
}

LogStream::LogStream(const LogStream& stream): std::stringstream(stream.str())
{
    m_area = stream.m_area;
}
