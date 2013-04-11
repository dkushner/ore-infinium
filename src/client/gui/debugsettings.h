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

#ifndef DEBUGSETTINGS_H
#define DEBUGSETTINGS_H

#include <Rocket/Core.h>

namespace Rocket
{
namespace Core
{
class ElementDocument;
}
namespace Controls
{
class ElementTabSet;
}
}

class Client;

class DebugSettings : public Rocket::Core::EventListener
{
public:
    DebugSettings(Client* client);
    ~DebugSettings();

    /// reimplemented from Rocket::Core::EventListener
    virtual void ProcessEvent(Rocket::Core::Event& event);

    void show();
    void hide();
    bool visible();

    Rocket::Core::ElementDocument* document();

private:
    void loadDocument();
    /// load initial debug rendering settings into the GUI (from the debug variable/bitfield)
    void loadRenderingSettings();

private:
    Client* m_client = nullptr;

    Rocket::Core::ElementDocument* m_debugSettings = nullptr;
    Rocket::Controls::ElementTabSet* m_tabSet = nullptr;
};

#endif
