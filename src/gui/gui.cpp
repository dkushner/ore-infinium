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

#include "gui.h"

#include "../game.h"
#include "../debug.h"
#include "../settings/settings.h"

#include "core/SystemInterfaceSDL2.h"
#include "core/ShellRenderInterfaceOpenGL.h"
#include "core/ShellFileInterface.h"

#include <Rocket/Core.h>
#include <Rocket/Debugger.h>
#include <Rocket/Controls.h>

GUI* GUI::s_instance(0);

GUI* GUI::instance()
{
    if (!s_instance)
        s_instance = new GUI();

    return s_instance;
}

GUI::GUI()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1600, 900, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    m_system = new SystemInterfaceSDL2();
    Rocket::Core::SetSystemInterface(m_system);

    m_renderer = new ShellRenderInterfaceOpenGL();
    Rocket::Core::SetRenderInterface(m_renderer);

    m_fileInterface = new ShellFileInterface("../gui/assets");
    Rocket::Core::SetFileInterface(m_fileInterface);

    Debug::fatal(Rocket::Core::Initialise(), Debug::Area::Graphics, "rocket init failure");
    Rocket::Controls::Initialise();

    m_context = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(1600, 900));

    Rocket::Debugger::Initialise(m_context);

    bool success = Rocket::Core::FontDatabase::LoadFontFace("../gui/assets/Delicious-Roman.otf");
    Debug::fatal(success, Debug::Area::Graphics, "font load failure");

    m_context->LoadMouseCursor("../gui/assets/cursor.rml");
}

GUI::~GUI()
{

}

void GUI::handleEvent(const SDL_Event& event)
{
    switch(event.type) {
    case SDL_KEYDOWN:
        m_context->ProcessKeyDown(m_system->TranslateKey(event.key.keysym.sym), m_system->GetKeyModifiers());
        break;

    case SDL_KEYUP:
        m_context->ProcessKeyUp(m_system->TranslateKey(event.key.keysym.sym), m_system->GetKeyModifiers());
        break;

    case SDL_TEXTINPUT:
        m_context->ProcessTextInput(event.text.text[0]);
        break;

    case SDL_TEXTEDITING:
        //FIXME: HANDLE THAT i guess? http://wiki.libsdl.org/moin.cgi/Tutorials/TextInput
        break;

    case SDL_MOUSEMOTION:
        m_context->ProcessMouseMove(event.motion.x, event.motion.y, m_system->GetKeyModifiers());
        break;

    case SDL_MOUSEWHEEL: {
        const int y = event.wheel.y;
        if (y > 0) {
            //scroll down
        } else if (y < 0) {
            m_context->ProcessMouseWheel(y, m_system->GetKeyModifiers());
        }
        break;
    }

    case SDL_MOUSEBUTTONDOWN:
        m_context->ProcessMouseButtonDown(m_system->TranslateMouseButton(event.button.button), m_system->GetKeyModifiers());
        break;

    case SDL_MOUSEBUTTONUP:
        m_context->ProcessMouseButtonUp(m_system->TranslateMouseButton(event.button.button), m_system->GetKeyModifiers());
        break;
    }
}

void GUI::debugRenderingChanged()
{
    Rocket::Debugger::SetVisible(Settings::instance()->debugGUIRenderingEnabled);
}

void GUI::render()
{
    m_context->Update();
    m_context->Render();
}

void GUI::addInputDemand()
{
    ++m_inputDemand;
    Debug::log(Debug::Area::Graphics) << "GUI::addInputDemand, input demand count: " << m_inputDemand;
}

void GUI::removeInputDemand()
{

    if (m_inputDemand > 0) {
       --m_inputDemand;
    } else {
        Debug::log(Debug::Area::Graphics) << "WARNING: GUI::removeInputDemand minor failure, tried to remove refcount below 0. This will break GUI focusing., input demand count: " << m_inputDemand;
    }

    Debug::log(Debug::Area::Graphics) << "GUI::removeInputDemand, input demand count: " << m_inputDemand;
}

bool GUI::inputDemanded()
{
    return m_inputDemand != 0;
}
