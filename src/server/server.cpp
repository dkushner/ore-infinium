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

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "src/debug.h"

#include <iostream>
#include <fstream>

Server::Server(unsigned int maxClients, unsigned int port)
{
    Debug::log(Debug::Area::NetworkServer) << "creating server at port: " << port;

    m_address.host = ENET_HOST_ANY;
    m_address.port = port;

    m_server = enet_host_create (&m_address, maxClients /* allow up to 32 clients and/or outgoing connections */,
                                                            2 /* allow up to 2 channels to be used, 0 and 1 */,
                                                            0 /* assume any amount of incoming bandwidth */,
                                                            0 /* assume any amount of outgoing bandwidth */ );

    Debug::assertf(m_server, "failed to create ENet server");
}

Server::~Server()
{
    enet_host_destroy(m_server);
}

void Server::tick()
{

}

void Server::poll()
{
    ENetEvent event;
    int eventStatus;

    eventStatus = enet_host_service(m_server, &event, 0);

    if (eventStatus > 0) {

        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                Debug::log(Debug::Area::NetworkServer) << "Received a new peer connection from host:  " << event.peer->address.host << " at port: " << event.peer->address.port;
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                processMessage(event);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                Debug::log(Debug::Area::NetworkServer) << "Peer has disconnected:  " << event.peer->address.host << " at port: " << event.peer->address.port;
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
//    std::cout << "(Server) Message from client, our client->server round trip latency is: " << event.peer->roundTripTime  << "\n";
//    std::cout << "(Server) latency is: " << event.peer->lowestRoundTripTime  << "\n";

    std::stringstream ss(std::string(event.packet->data, event.packet->dataLength));

    uint32_t packetType = Packet::deserializePacketType(ss);

    switch (packetType) {
        case Packet::FromClientPacketContents::InitialConnectionDataFromClientPacket:
            receiveInitialClientData(&ss);
            break;

        case Packet::FromClientPacketContents::ChatMessageFromClientPacket:
            PacketBuf::ChatMessage chatMessage;
            Packet::deserialize(&ss, &chatMessage);
            Debug::log(Debug::Area::NetworkServer) << "(Server) chat message received: " << chatMessage.message();
            break;
    }

    enet_packet_destroy(event.packet);

    // Lets broadcast this message to all
    //                enet_host_broadcast(m_server, 0, event.packet);
    //                enet_peer_send()
}

void Server::receiveInitialClientData(std::stringstream* ss)
{
    PacketBuf::ClientInitialConnection message;
    Packet::deserialize(ss, &message);

    Debug::log(Debug::Area::NetworkServer) << "client sent player name and version data name: " << message.playername() << " version major: " << message.versionmajor() << " minor: " << message.versionminor();
}
