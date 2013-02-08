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

#include "client.h"

#include "src/network/packet.h"
#include "src/network/protobuf-compiled/packet.pb.h"

#include "src/debug.h"

Client::Client(const char* address, unsigned int port)
{
    m_client = enet_host_create (nullptr /* create a client host */,
                               1 /* only allow 1 outgoing connection */,
                               2 /* allow up 2 channels to be used, 0 and 1 */,
                               57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                               14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

    Debug::assertf(m_client, "failed to create ENet client host");

    enet_address_set_host(&m_address, address);
    m_address.port = port;

    peer = enet_host_connect(m_client, &m_address, 2, 0);

    if (peer == NULL) {
        fprintf(stderr, "Client failed to connect to server");
        exit(EXIT_FAILURE);
    }
}

Client::~Client()
{
    enet_host_destroy(m_client);
}

void Client::poll()
{
    ENetEvent event;
    int eventStatus;

    eventStatus = 1;

    eventStatus = enet_host_service(m_client, &event, 0);


    // If we had some event that interested us
    if (eventStatus > 0) {
        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("(Client) We got a new connection from %x\n", event.peer->address.host);

                char hostname[32];
                enet_address_get_host_ip(&event.peer->address, hostname, static_cast<size_t>(32));
                printf("(Client) CONNECTED TO HOST IP: %s\n", hostname);
//                Debug::log()
                enet_peer_ping(event.peer);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
//                printf("(Client) Message from server : %s\n", event.packet->data);
                std::cout << "(Client) Message from server, our client->server round trip latency is: " << event.peer->lastRoundTripTime << "\n";


                // Lets broadcast this message to all
                // enet_host_broadcast(client, 0, event.packet);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("(Client) %s disconnected.\n", event.peer->data);
                // Reset client's information
                event.peer->data = NULL;
                break;
        }
    }

    PacketBuf::ChatMessage message;
    message.set_message("THIS IS A TEST PROTOBUF MESSAGE FROM CLIENT");
    std::stringstream ss(std::stringstream::out | std::stringstream::binary);
    message.SerializeToOstream(&ss);
    ss.str();
    ss.str().size();

    ENetPacket *packet = enet_packet_create(ss.str().c_str(), ss.str().size(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}
