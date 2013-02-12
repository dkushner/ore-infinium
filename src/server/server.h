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

#ifndef SERVER_H
#define SERVER_H

#include <enet/enet.h>

#include <sstream>

class Server
{
public:
    /// we use port 44543 for our server by default..
    Server(unsigned int maxClients, unsigned int port = 44543);
    ~Server();

    void poll();
    void tick();
    void processMessage(ENetEvent& event);

    static constexpr int MAXPLAYERS = 8;

private:
    /**
     * @returns false if the initial client data is unsuitable (version mismatch)
     * In such a case, a disconnect is *necessary*.
     */
    bool receiveInitialClientData(std::stringstream* ss);
    void receiveChatMessage(std::stringstream* ss);

private:
    ENetHost* m_server = nullptr;
    ENetAddress m_address;
};

#endif
