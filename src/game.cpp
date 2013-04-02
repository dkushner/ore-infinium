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
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (m_startupDebugLogging) {
       
    }
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
    m_client = new Client();

    tick();
    shutdown();
}

void Game::tick()
{
    SDL_StartTextInput();

    std::chrono::system_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

    double accumulator = 0.0;
    const double dt = (1.0/60.0) * 1000.0; // runs at 60 hz
    double t = 0.0;

    double fps = 0.0;
    while (m_running) {

        std::chrono::system_clock::time_point newTime = std::chrono::high_resolution_clock::now();
        double frameTime = std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(newTime - currentTime).count();

        if (frameTime > (1.0 / 15.0) * 1000.0) {
            frameTime = (1.0 / 15.0) * 1000.0; // note: max frame time to avoid spiral of death
        }
        currentTime = newTime;

        accumulator += frameTime;

        while (accumulator >= dt) {
            t += dt;
            accumulator -= dt;

            m_client->tick(frameTime);
        }

        //dedicated server, no client.
        //if (m_server) {

        //}

        m_client->render(frameTime);

        const double alpha = accumulator / dt;

        // sleep so we don't burn cpu
        std::chrono::milliseconds timeUntilNextFrame(int(dt - accumulator));
        std::this_thread::sleep_for(timeUntilNextFrame);
    }

    /*

    while (1) {

        std::chrono::system_clock::time_point newTime = std::chrono::high_resolution_clock::now();
        double frameTime = std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(newTime - currentTime).count();

        if (frameTime > (1.0 / 15.0) * 1000.0) {
            frameTime = (1.0 / 15.0) * 1000.0; // note: max frame time to avoid spiral of death
        }
        currentTime = newTime;

        accumulator += frameTime;

        while (accumulator >= dt) {
            m_world->update(dt);

            t += dt;
            accumulator -= dt;
        }

        const double alpha = accumulator / dt;

        poll();
        // do network shit
        // sleep so we don't burn cpu
        std::chrono::milliseconds timeUntilNextFrame(int(dt - accumulator));
        std::this_thread::sleep_for(timeUntilNextFrame);
    }
    */


shutdown:
    shutdown();
}

void Game::shutdown()
{
    exit(0);
}
