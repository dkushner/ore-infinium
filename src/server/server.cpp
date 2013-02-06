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

#include "server.h"

#include "src/debug.h"

Server::Server(const char* address, unsigned int port)
{
    Debug::log(Debug::Area::Network) << "creating server at host address: " << address << " port: " << port;

    enet_address_set_host(m_address, address);
    m_address->port = port;

    m_server = enet_host_create (m_address, 32 /* allow up to 32 clients and/or outgoing connections */,
                                                            2 /* allow up to 2 channels to be used, 0 and 1 */,
                                                            0 /* assume any amount of incoming bandwidth */,
                                                            0 /* assume any amount of outgoing bandwidth */ );

    Debug::assertf(m_server, "failed to create ENet host");
}

Server::~Server()
{
    enet_host_destroy(m_server);
}
