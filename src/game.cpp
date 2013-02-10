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

#include "game.h"

#include "debug.h"
#include "spritesheetmanager.h"
#include "fontmanager.h"

#include <google/protobuf/stubs/common.h>

#include "client/client.h"
#include "server/server.h"

#include "settings/settings.h"

#include <enet/enet.h>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include <vector>

#include <assert.h>

Game::Game()
{

}

Game::~Game()
{
//    delete m_world;
    enet_deinitialize();
    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
}

void Game::abort_game(const char* message)
{
    printf("%s \n", message);
    shutdown();
    exit(1);
}

void Game::init()
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    m_server = new Server();
    m_client = new Client();

    tick();
    shutdown();
}

void Game::tick()
{
    //FIXME: USE STD CHRONO, this breaks down heavily when in server mode(no SDL context)
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;

    SDL_StartTextInput();

    double fps = 0.0;
    while (m_running) {
        const double delta = static_cast<double>(SDL_GetTicks() - startTime);
        fps = (frameCount / delta) * 1000;

        m_server->poll();

        m_client->tick(delta, fps);
        m_client->render(delta);

        ++frameCount;
    }

shutdown:
    shutdown();
}

void Game::shutdown()
{
    exit(0);
}
