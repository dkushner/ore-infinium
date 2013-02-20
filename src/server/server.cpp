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

#include "src/player.h"

#include "src/debug.h"
#include <src/camera.h>
#include <src/world.h>
#include "src/../config.h"

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <iostream>
#include <fstream>

#include <SDL2/SDL.h>

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
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;

    double fps = 0.0;
    while (1) {
        const double delta = static_cast<double>(SDL_GetTicks() - startTime);

        fps = (frameCount / delta) * 1000;
        poll();

        if (m_world) {
           m_world->update(delta);
        }

        ++frameCount;
    }
}

void Server::poll()
{
    ENetEvent event;
    int eventStatus;

    eventStatus = enet_host_service(m_server, &event, 100);

    if (eventStatus > 0) {

        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                Debug::log(Debug::Area::NetworkServer) << "Received a new peer, adding to client list, connection from host:  " << event.peer->address.host << " at port: " << event.peer->address.port << " client has not yet been validated.";
                Debug::log(Debug::Area::NetworkServer) << "client count, before adding: " << m_clients.size();
                //NOTE: we don't actually act on it, first we wait for them to send us a packet then we validate it and if so we add it to our client list
                //FIXME: probably should timeout if they're not validated within n seconds, that way they can't just keep piling on top of us

                //DEFAULT IS 5000
                event.peer->timeoutMinimum = 2000;
                //DEFAULT IS 30000
                event.peer->timeoutMaximum = 8000;
                event.peer->timeoutLimit = 20;
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                processMessage(event);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                Debug::log(Debug::Area::NetworkServer) << "Peer has disconnected:  " << event.peer->address.host << " at port: " << event.peer->address.port;
                printf("%s disconnected.\n", event.peer->data);
                for (auto& client : m_clients) {
                    if (client.first == event.peer) {
                       Debug::log(Debug::Area::NetworkServer) << "FOUND PEER for disconnect, deleting it";
                       m_clients.erase(client.first);
                    }
                }
                Debug::log(Debug::Area::NetworkServer) << "m_clients size: " << m_clients.size();

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
        case Packet::FromClientPacketContents::InitialConnectionDataFromClientPacket: {
            //version mismatch, can't let him connect or else we'll have assloads of problems
            uint32_t result = receiveInitialClientData(&ss, event);
            switch (result) {
                case Packet::ConnectionEventType::None: {
                    //he's good to go, validation succeeded
                    sendInitialPlayerData(m_clients[event.peer]);
                    break;
                }

                case Packet::ConnectionEventType::DisconnectedInvalidPlayerName:
                    //fall through

                case Packet::ConnectionEventType::DisconnectedVersionMismatch: {
                    enet_peer_disconnect_now(event.peer, result);
                    break;
                }
            }
            break;
        }

        case Packet::FromClientPacketContents::ChatMessageFromClientPacket:
            receiveChatMessage(&ss, m_clients[event.peer]->name());
            break;

        case Packet::FromClientPacketContents::PlayerMoveFromClientPacket:
            receivePlayerMove(&ss, m_clients[event.peer]);
            break;
    }

    enet_packet_destroy(event.packet);
}

uint32_t Server::receiveInitialClientData(std::stringstream* ss, ENetEvent& event)
{
    PacketBuf::ClientInitialConnection message;
    Packet::deserialize(ss, &message);

    Debug::log(Debug::Area::NetworkServer) << "client sent player name and version data name: " << message.playername() << " version major: " << message.versionmajor() << " minor: " << message.versionminor();

    if (message.versionmajor() != ore_infinium_VERSION_MAJOR || message.versionminor() != ore_infinium_VERSION_MINOR) {
        return Packet::ConnectionEventType::DisconnectedVersionMismatch;
    }

    //trying to trick us into using a blank name
    if (message.playername().empty()) {
        return Packet::ConnectionEventType::DisconnectedInvalidPlayerName;
    }

    m_clients[event.peer] = createPlayer(message.playername());

    return Packet::ConnectionEventType::None;;
}

void Server::receiveChatMessage(std::stringstream* ss, const std::string& playerName)
{
    PacketBuf::ChatMessageFromClient receiveMessage;
    Packet::deserialize(ss, &receiveMessage);

    sendChatMessage(ss->str(), playerName);
}

void Server::receivePlayerMove(std::stringstream* ss, Player* player)
{
    PacketBuf::PlayerMoveFromClient message;
    Packet::deserialize(ss, &message);

//    Debug::log(Debug::Area::NetworkServer) << " PLAYER MOVE RECEIVED, directionx: " <<  message.directionx() << " Y: " <<
    player->move(message.directionx(), message.directiony());
}

void Server::sendChatMessage(const std::string& message, const std::string& playerName)
{
    PacketBuf::ChatMessageFromServer sendMessage;
    sendMessage.set_playername(playerName);
    sendMessage.set_message(message);

    Packet::sendPacketBroadcast(m_server, &sendMessage, Packet::FromServerPacketContents::ChatMessageFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendInitialPlayerData(Player* player)
{
    PacketBuf::InitialPlayerDataFromServer message;
    message.set_playername(player->name());
    message.set_playerid(player->playerID());
    message.set_x(player->position().x);
    message.set_y(player->position().y);

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::InitialPlayerDataFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

Player* Server::createPlayer(const std::string& playerName)
{
    Player* player = new Player("testframe");
    player->setName(playerName);
    player->setPlayerID(m_freePlayerID);
    player->setPosition(2500, 1492);

    ++m_freePlayerID;

    return player;
}

void Server::sendPlayerMove(Player* player)
{
    PacketBuf::PlayerMoveFromServer message;
    message.set_playerid(player->playerID());
    message.set_x(player->position().x);
    message.set_y(player->position().y);

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::ChatMessageFromServerPacket, ENET_PACKET_FLAG_UNSEQUENCED);
}
