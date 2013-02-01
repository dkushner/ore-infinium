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

#ifndef GUI_H
#define GUI_H

namespace Rocket {
    namespace Core {
        class Context;
    }
}

class ShellRenderInterfaceOpenGL;
class ShellFileInterface;
class SystemInterfaceSDL2;

union SDL_Event;

class GUI
{
public:
    GUI();
    ~GUI();

    void handleEvent(const SDL_Event& event);
    void render();

    /**
     * Notify to the gui that the state has changed
     * so we can decide what to do
     */
    void debugRenderingChanged();

private:
    Rocket::Core::Context* m_context = nullptr;

    ShellRenderInterfaceOpenGL* m_renderer = nullptr;
    ShellFileInterface* m_fileInterface = nullptr;
    SystemInterfaceSDL2* m_system = nullptr;
};

#endif
