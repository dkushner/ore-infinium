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

#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>


#include <string>

class Server;
class Client;

class Game
{
public:
    Game();
    ~Game();

    void init();

    void tick();

    void abort_game(const char* message);
    void shutdown();


private:
    Client* m_client = nullptr;
    Server* m_server = nullptr;

    World *m_world = nullptr;


    bool m_running = true;
};

#endif
