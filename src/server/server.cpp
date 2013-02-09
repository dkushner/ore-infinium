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

#include "src/network/packet.h"

#include "src/network/protobuf-compiled/packet.pb.h"

#include "src/debug.h"

#include <iostream>
#include <fstream>

Server::Server(unsigned int port)
{
    Debug::log(Debug::Area::Network) << "creating server at port: " << port;

    m_address.host = ENET_HOST_ANY;
    m_address.port = port;

    m_server = enet_host_create (&m_address, 32 /* allow up to 32 clients and/or outgoing connections */,
                                                            2 /* allow up to 2 channels to be used, 0 and 1 */,
                                                            0 /* assume any amount of incoming bandwidth */,
                                                            0 /* assume any amount of outgoing bandwidth */ );

    Debug::assertf(m_server, "failed to create ENet server");
}

Server::~Server()
{
    enet_host_destroy(m_server);
}

void Server::poll()
{
    ENetEvent event;
    int eventStatus;

    // c. Connect and user service
    eventStatus = 1;

    eventStatus = enet_host_service(m_server, &event, 0);

//    PacketBuf::ChatMessage message;
//    message.set_message("THIS IS A TEST PROTOBUF MESSAGE FROM CLIENT");
//    std::stringstream ss(std::stringstream::out | std::stringstream::binary);
//    ss.str();
//    ss.str().size();
//
//    person.SerializeToOstream(&ss);
//
    if (eventStatus > 0) {

        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("(Server) We got a new connection from %x:%u \n", event.peer->address.host, event.peer->address.port);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                processMessage(event);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", event.peer->data);

                // Reset client's information
                event.peer->data = NULL;
                break;

        }
    }

}

void Server::processMessage(ENetEvent& event)
{
//    std::cout << "(Server) Message from client : " << event.packet->data << "\n";
    std::cout << "(Server) Message from client, our client->server round trip latency is: " << event.peer->roundTripTime  << "\n";

    std::cout << "(Server) ping interval is: " << event.peer->pingInterval  << "\n";
    std::cout << "(Server) latency is: " << event.peer->lowestRoundTripTime  << "\n";



    std::stringstream ss;

    ss << event.packet->data;


    char c;
    ss.readsome(&c, sizeof(PacketBuf::Packet));
    std::cout << " INT VAL: " << c << '\n';

    PacketBuf::ChatMessage message;
    message.ParseFromIstream(&ss);

    std::cout << "(Server) chat message processed, serial contents: " << message.message() << "\n";

    enet_packet_destroy(event.packet);

    // Lets broadcast this message to all
    //                enet_host_broadcast(m_server, 0, event.packet);
    //                enet_peer_send()
}

