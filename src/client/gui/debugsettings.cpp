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

#include "debugsettings.h"
#include "gui.h"

#include "src/client/client.h"
#include <src/debug.h>
#include <src/settings/settings.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>
#include <sstream>

#include <assert.h>

DebugSettings::DebugSettings(Client* client) : m_client(client)
{
    loadDocument();
}

DebugSettings::~DebugSettings()
{
}

void DebugSettings::ProcessEvent(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    const Rocket::Core::String& type = event.GetType();

    const bool isChecked = event.GetCurrentElement()->HasAttribute("checked");

    if (id == "ClientRendererArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::ClientRendererArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::ClientRendererArea;
        }
    } else if (id == "TileRendererArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::TileRendererArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::TileRendererArea;
        }
    } else if (id == "SpriteSheetRendererArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::SpriteSheetRendererArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::SpriteSheetRendererArea;
        }
    } else if (id == "LightingRendererArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::LightingRendererArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::LightingRendererArea;
        }
    } else if (id == "PhysicsArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::PhysicsArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::PhysicsArea;
        }
    } else if (id == "AudioArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::AudioArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::AudioArea;
        }
    } else if (id == "AudioLoaderArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::AudioLoaderArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::AudioLoaderArea;
        }
    } else if (id == "GUILoggerArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::GUILoggerArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::GUILoggerArea;
        }
    } else if (id == "ShadersArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::ShadersArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::ShadersArea;
        }
    } else if (id == "NetworkClientInitialArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::NetworkClientInitialArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::NetworkClientInitialArea;
        }
    } else if (id == "NetworkServerInitialArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::NetworkServerInitialArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::NetworkServerInitialArea;
        }
    } else if (id == "NetworkClientContinuousArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::NetworkClientContinuousArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::NetworkClientContinuousArea;
        }
    } else if (id == "NetworkServerContinuousArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::NetworkServerContinuousArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::NetworkServerContinuousArea;
        }
    } else if (id == "ClientInventoryArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::ClientInventoryArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::ClientInventoryArea;
        }
    } else if (id == "ServerInventoryArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::ServerInventoryArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::ServerInventoryArea;
        }
    } else if (id == "ServerEntityLogicArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::ServerEntityLogicArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::ServerEntityLogicArea;
        }
    } else if (id == "ImageLoaderArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::ImageLoaderArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::ImageLoaderArea;
        }
    } else if (id == "WorldGeneratorArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::WorldGeneratorArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::WorldGeneratorArea;
        }
    } else if (id == "WorldLoaderArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::WorldLoaderArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::WorldLoaderArea;
        }
    } else if (id == "ClientEntityCreationArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::ClientEntityCreationArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::ClientEntityCreationArea;
        }
    } else if (id == "ServerEntityCreationArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::ServerEntityCreationArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::ServerEntityCreationArea;
        }
    } else if (id == "SettingsArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::SettingsArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::SettingsArea;
        }
    } else if (id == "StartupArea") {
        if (isChecked) {
            Settings::instance()->debugAreas |= Debug::Area::StartupArea;
        } else {
            Settings::instance()->debugAreas &= ~Debug::Area::StartupArea;
        }
    }

    //////////////// RENDERER SETTINGS
    if (id == "GUIDebugRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::GUIRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::GUIRenderingDebug;
        }
    } else if (id == "LightRenderingPass") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::LightRenderingPassDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::GUIRenderingDebug;
        }
    } else if (id == "TileRenderingPass") {

        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::TileRenderingPassDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::GUIRenderingDebug;
        }
    } else if (id == "Box2DShapeRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::Box2DShapeRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::Box2DShapeRenderingDebug;
        }
    } else if (id == "Box2DCenterOfMassRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::Box2DCenterOfMassRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::Box2DCenterOfMassRenderingDebug;
        }
    } else if (id == "Box2DJointRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::Box2DJointRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::Box2DJointRenderingDebug;
        }
    } else if (id == "Box2DAABBRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::Box2DAABBRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::Box2DAABBRenderingDebug;
        }
    }
}

/*
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
    */

void DebugSettings::loadDocument()
{
    m_debugSettings = GUI::instance()->context()->LoadDocument("../client/gui/assets/debugSettings.rml");
    m_debugSettings->GetElementById("title")->SetInnerRML("Debug Settings");

    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_debugSettings->GetElementById("tabset"));

    //load initial values
    loadRenderingSettings();

    /////////// LOG DEBUG AREA TAB
    m_debugSettings->GetElementById("ClientRendererArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("TileRendererArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("SpriteSheetRendererArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("LightingRendererArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("PhysicsArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("AudioArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("AudioLoaderArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("GUILoggerArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ShadersArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("NetworkClientInitialArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("NetworkServerInitialArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("NetworkClientContinuousArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("NetworkServerContinuousArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ClientInventoryArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ServerInventoryArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ServerEntityLogicArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ImageLoaderArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("WorldGeneratorArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("WorldLoaderArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ClientEntityCreationArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ServerEntityCreationArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("SettingsArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("StartupArea")->AddEventListener("change", this);

    //////  RENDERER SETTINGS TAB
    m_debugSettings->GetElementById("GUIDebugRendering")->AddEventListener("change", this);
    m_debugSettings->GetElementById("LightRenderingPass")->AddEventListener("change", this);
    m_debugSettings->GetElementById("TileRenderingPass")->AddEventListener("change", this);
    m_debugSettings->GetElementById("Box2DShapeRendering")->AddEventListener("change", this);
    m_debugSettings->GetElementById("Box2DCenterOfMassRendering")->AddEventListener("change", this);
    m_debugSettings->GetElementById("Box2DJointRendering")->AddEventListener("change", this);
    m_debugSettings->GetElementById("Box2DAABBRendering")->AddEventListener("change", this);
}

void DebugSettings::loadRenderingSettings()
{
    int flags = Settings::instance()->debugRendererFlags;

    //all checkboxen are off by default, so just set them to on if they need to be.
    if (flags & Debug::RenderingDebug::GUIRenderingDebug) {
        m_debugSettings->GetElementById("GUIDebugRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::LightRenderingPassDebug) {
        m_debugSettings->GetElementById("LightRenderingPass")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::TileRenderingPassDebug) {
        m_debugSettings->GetElementById("TileRenderingPass")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::Box2DShapeRenderingDebug) {
        m_debugSettings->GetElementById("Box2DShapeRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::Box2DCenterOfMassRenderingDebug) {
        m_debugSettings->GetElementById("Box2DCenterOfMassRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::Box2DJointRenderingDebug) {
        m_debugSettings->GetElementById("Box2DJointRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::Box2DAABBRenderingDebug) {
        m_debugSettings->GetElementById("Box2DAABBRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }
}

void DebugSettings::show()
{
    m_debugSettings->Show();
}

void DebugSettings::hide()
{
    m_debugSettings->Hide();
}

bool DebugSettings::visible()
{
    return m_debugSettings->IsVisible();
}

Rocket::Core::ElementDocument* DebugSettings::document()
{
    return m_debugSettings;
}

