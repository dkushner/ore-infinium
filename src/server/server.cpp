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

#include "src/packet.pb.h"

#include "src/player.h"

#include "src/client/client.h"

#include "src/debug.h"
#include <src/camera.h>
#include <src/world.h>
#include <src/chunk.h>
#include <src/torch.h>
#include <src/quickbarinventory.h>
#include "src/../config.h"

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <thread>

#include <SDL2/SDL.h>

Server::Server(unsigned int maxClients, unsigned int port, Client* client) :
    m_client(client)
{
    Debug::log(Debug::Area::NetworkServerInitialArea) << "creating server at port: " << port;

    m_address.host = ENET_HOST_ANY;
    m_address.port = port;

    m_server = enet_host_create(&m_address, maxClients /* allow up to maxClients and/or outgoing connections */,
                                2 /* allow up to 2 channels to be used, 0 and 1 */,
                                0 /* assume any amount of incoming bandwidth */,
                                0 /* assume any amount of outgoing bandwidth */);

    Debug::assertf(m_server, "failed to create ENet server");

    m_world = new World(nullptr, nullptr, this);
}

Server::~Server()
{
    enet_host_destroy(m_server);
}

void Server::tick()
{
    std::chrono::system_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

    double accumulator = 0.0;
    const double dt = (FIXED_TIMESTEP) * 1000.0; // runs at 30 hz
    double t = 0.0;

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
}

void Server::poll()
{
    ENetEvent event;
    int eventStatus;

    while (enet_host_service(m_server, &event, 0)) {

        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            Debug::log(Debug::Area::NetworkServerContinuousArea) << "Received a new peer, adding to client list, connection from host:  " << event.peer->address.host << " at port: " << event.peer->address.port << " client has not yet been validated.";
            Debug::log(Debug::Area::NetworkServerContinuousArea) << "client count, before adding: " << m_clients.size();
            //NOTE: we don't actually act on it, first we wait for them to send us a packet then we validate it and if so we add it to our client list
            //FIXME: probably should timeout if they're not validated within n seconds, that way they can't just keep piling on top of us

            //DEFAULT IS 5000
//                event.peer->timeoutMinimum = 2000;
//                //DEFAULT IS 30000
//                event.peer->timeoutMaximum = 8000;
//                event.peer->timeoutLimit = 20;
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            processMessage(event);
            break;

        case ENET_EVENT_TYPE_DISCONNECT: {
            Debug::log(Debug::Area::NetworkServerContinuousArea) << "Peer has disconnected:  " << event.peer->address.host << " at port: " << event.peer->address.port;
            printf("%s disconnected.\n", event.peer->data);

            for (auto & client : m_clients) {
                if (client.first == event.peer) {
                    Debug::log(Debug::Area::NetworkServerContinuousArea) << "Found peer for disconnect, deleting it";
                    m_clients.erase(client.first);
                }
            }
            Debug::log(Debug::Area::NetworkServerContinuousArea) << "m_clients size: " << m_clients.size();

            // Reset client's information
            event.peer->data = NULL;
            break;
        }

        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}

void Server::processMessage(ENetEvent& event)
{
//    std::cout << "(Server) Message from client : " << event.packet->data << "\n";
//    std::cout << "(Server) Message from client, our client->server round trip latency is: " << event.peer->roundTripTime  << "\n";
//    std::cout << "(Server) latency is: " << event.peer->lowestRoundTripTime  << "\n";

    std::stringstream ss(std::string((char*)(event.packet->data), event.packet->dataLength));

    uint32_t packetType = Packet::deserializePacketType(ss);

    switch (packetType) {
    case Packet::FromClientPacketContents::InitialConnectionDataFromClientPacket: {
        //check for version mismatch, can't let him connect or else we'll have assloads of problems
        uint32_t result = receiveInitialClientData(&ss, event);
        switch (result) {
        case Packet::ConnectionEventType::None: {

            //he's good to go, validation succeeded, tell everyone, including himself that he joined
            for (auto & client : m_clients) {
                sendInitialPlayerData(client.first, m_clients[event.peer]);
            }

            for (auto & client : m_clients) {
                // now we have to send this new client every player we know about so far, except not himself (don't send his own player, obviously,
                // he already knows what it is) since we already sent that first.
                if (client.first != event.peer) {
                    sendInitialPlayerData(event.peer, client.second);
                }
            }

            sendInitialPlayerDataFinished(event.peer);
            sendInitialWorldChunk(event.peer);

            // tell our (this) player/client what his quickbar inventory contains (send all items within it)
            uint8_t maxIndex = m_clients[event.peer]->quickBarInventory()->maxEquippedSlots();
            for (uint8_t index = 0; index < maxIndex; ++index) {
                sendPlayerQuickBarInventory(m_clients[event.peer], index);
            }
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

    case Packet::FromClientPacketContents::PlayerMouseStateFromClient:
        receivePlayerMouseState(&ss, m_clients[event.peer]);
        break;

    case Packet::FromClientPacketContents::QuickBarInventorySelectSlotRequestFromClient:
        receiveQuickBarInventorySelectSlotRequest(&ss, m_clients[event.peer]);
        break;
    }

    enet_packet_destroy(event.packet);
}

uint32_t Server::receiveInitialClientData(std::stringstream* ss, ENetEvent& event)
{
    PacketBuf::ClientInitialConnection message;
    Packet::deserialize(ss, &message);

    Debug::log(Debug::Area::NetworkServerInitialArea) << "receiving client's player name and version data. Name: " << message.playername() << " version major: " << message.versionmajor() << " minor: " << message.versionminor();

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

    sendChatMessage(receiveMessage.message(), playerName);
}

void Server::receivePlayerMove(std::stringstream* ss, Entities::Player* player)
{
    PacketBuf::PlayerMoveFromClient message;
    Packet::deserialize(ss, &message);

//    Debug::log(Debug::Area::NetworkServer) << " PLAYER MOVE RECEIVED, directionx: " <<  message.directionx() << " Y: " <<
    player->move(message.directionx(), message.directiony());
    if (message.jump()) {
        player->jump();
    }
}

void Server::receivePlayerMouseState(std::stringstream* ss, Entities::Player* player)
{
    PacketBuf::PlayerMouseStateFromClient message;
    Packet::deserialize(ss, &message);

    player->setMouseLeftButtonHeld(message.leftbuttonheld());
    player->setMouseRightButtonHeld(message.rightbuttonheld());
    player->setMousePosition(message.x(), message.y());
}

void Server::receiveQuickBarInventorySelectSlotRequest(std::stringstream* ss, Entities::Player* player)
{
    PacketBuf::QuickBarInventorySelectSlotRequestFromClient message;
    Packet::deserialize(ss, &message);

    const uint32_t index = message.index();

    if (index > player->quickBarInventory()->maxEquippedSlots()) {
        Debug::log(Debug::Area::ServerInventoryArea) << "server told to equip a quickbar inventory slot, but index was greater than maxEquippedSlots. this is likely a sync failure(or malicious intent).";
        return;
    }

    player->quickBarInventory()->selectSlot(index);
}

void Server::sendChatMessage(const std::string& message, const std::string& playerName)
{
    PacketBuf::ChatMessageFromServer sendMessage;
    sendMessage.set_playername(playerName);
    sendMessage.set_message(message);

    Packet::sendPacketBroadcast(m_server, &sendMessage, Packet::FromServerPacketContents::ChatMessageFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendInitialPlayerData(ENetPeer* peer, Entities::Player* player)
{
    PacketBuf::InitialPlayerDataFromServer message;
    message.set_playername(player->name());
    message.set_playerid(player->playerID());
    message.set_x(player->position().x);
    message.set_y(player->position().y);

    Packet::sendPacket(peer, &message, Packet::FromServerPacketContents::InitialPlayerDataFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendInitialPlayerDataFinished(ENetPeer* peer)
{
    PacketBuf::InitialPlayerDataFinishedFromServer message;

    Packet::sendPacket(peer, &message, Packet::FromServerPacketContents::InitialPlayerDataFinishedFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendInitialWorldChunk(ENetPeer* peer)
{
    PacketBuf::Chunk message;

    Entities::Player* player = m_clients[peer];

    //FIXME: use a nice value, maybe constant or dynamic..constant is easier though
    // it needs to be bigger than the player's viewport, obviously
    //NOTE: divide the value by block size, then get range between left and right and that's how many blocks we are sending
    uint32_t startX = (player->position().x - World::pixelsToMeters(1024)) / Block::BLOCK_SIZE;
    uint32_t endX = (player->position().x + World::pixelsToMeters(2024)) / Block::BLOCK_SIZE;

    uint32_t startY = (player->position().y - World::pixelsToMeters(1024))/ Block::BLOCK_SIZE;
    uint32_t endY = (player->position().y + World::pixelsToMeters(2024)) / Block::BLOCK_SIZE;

    message.set_startx(startX);
    message.set_endx(endX);

    message.set_starty(startY);
    message.set_endy(endY);

    Chunk chunk = m_world->createChunk(startX, startY, endX, endY);

    for (int row = startY; row < endY; ++row) {
        for (int column = startX; column < endX; ++column) {

            uint32_t index = column * WORLD_ROWCOUNT + row;
            Block& block = chunk.blocks()->at(index);

            message.add_meshtype(block.meshType);
            message.add_primitivetype(block.primitiveType);
            message.add_walltype(block.wallType);
        }
    }

    Packet::sendPacket(peer, &message, Packet::FromServerPacketContents::ChunkFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendWorldChunk(Chunk* chunk)
{
    PacketBuf::Chunk message;

    message.set_startx(chunk->startX());
    message.set_endx(chunk->endX());

    message.set_starty(chunk->startY());
    message.set_endy(chunk->endY());

    for (int row = chunk->startY(); row < chunk->endY(); ++row) {
        for (int column = chunk->startX(); column < chunk->endX(); ++column) {

            uint32_t index = column * WORLD_ROWCOUNT + row;
            Block& block = chunk->blocks()->at(index);

            message.add_meshtype(block.meshType);
            message.add_primitivetype(block.primitiveType);
            message.add_walltype(block.wallType);
        }
    }

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::ChunkFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendItemSpawned(Item* item)
{
    PacketBuf::Item message;

    message.set_x(item->position().x);
    message.set_y(item->position().y);
    message.set_itemtype(item->type());
    message.set_itemdetails(item->details());
    message.set_itemname(item->name());
    message.set_itemstate(item->state());
    message.set_stacksize(item->stackSize());
    //NOTE: index is not set, as it is not in a container of any kidn.

    switch (item->type()) {
    case Item::ItemType::Torch:
        Torch* torch = dynamic_cast<Torch*>(item);
        message.set_radius(torch->radius());
        break;
    }

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::ItemSpawnedFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendQuickBarInventoryItemCountChanged(Entities::Player* player, uint8_t index, uint8_t newCount)
{
    PacketBuf::ItemCountChanged message;
    message.set_index(index);
    message.set_newcount(newCount);

    // search for the client associated with this player
    ENetPeer* peer = nullptr;
    for (auto c : m_clients) {
        if (c.second == player) {
            peer = c.first;
            break;
        }
    }

    Packet::sendPacket(peer, &message, Packet::FromServerPacketContents::QuickBarInventoryItemCountChangedFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

Entities::Player* Server::createPlayer(const std::string& playerName)
{
    Entities::Player* player = new Entities::Player("player1Standing1");
    player->setName(playerName);
    player->setPlayerID(m_freePlayerID);

    //HACK FIXME: HOLY FUCK BACKMAN this is fucked horribly until physics integration is 100% complete. both of these have to be at the same position, and that simpyl shouldn't be needed..
    // if you don't set oen of them, BAD SHIT HAPPENS
    float posX = 2500.0f/PIXELS_PER_METER;
    float posY = 1492.0f/PIXELS_PER_METER;
    Debug::log(Debug::Area::NetworkServerInitialArea) << "CREATING PLAYER, SETTING PLAYER POS X : " << posX << " Y : " << posY;

    player->setPosition(posX, posY);
    player->createPhysicsBody(m_world, glm::vec2(posX, posY));

    Debug::log(Debug::Area::NetworkServerInitialArea) << " POS IS: " << player->position().x << " Y: " << player->position().y;

    QuickBarInventory* quickBarInventory = new QuickBarInventory();
    player->setQuickBarInventory(quickBarInventory);

    std::random_device device;
    std::mt19937 rand(device());
    //FIXME: convert to 1, n
    std::uniform_int_distribution<> distribution(1, 64);

    //TODO: load the player's inventory from file..for now, initialize *the whole thing* with bullshit
    for (uint8_t i = 0; i < quickBarInventory->maxEquippedSlots(); ++i) {
        Torch *torch = new Torch(glm::vec2(0, 0));
        torch->setStackSize(distribution(rand));
        quickBarInventory->setSlot(i, torch);
    }

    m_world->addPlayer(player);

    ++m_freePlayerID;

    return player;
}

void Server::sendPlayerMove(Entities::Player* player)
{
    PacketBuf::PlayerMoveFromServer message;
    message.set_playerid(player->playerID());
    message.set_x(player->position().x);
    message.set_y(player->position().y);

    Debug::log(Debug::NetworkServerContinuousArea) << "Sending player move, position x: " << player->position().x << " y: " << player->position().y;

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::PlayerMoveFromServerPacket, ENET_PACKET_FLAG_UNSEQUENCED);
}

void Server::sendPlayerQuickBarInventory(Entities::Player* player, uint8_t index)
{
    Item* item = player->quickBarInventory()->item(index);

    if (item == nullptr) {
        Debug::log(Debug::Area::ServerInventoryArea) << "warning, BAD SHIT HAPPENED, server tried sending a player's quickbar inventory but an element was nullptr, which means we didn't send as much as we should have, so the client is empty for this element index..VERY BAD SHIT";
        return;
    }

    PacketBuf::Item message;
    //NOTE: position and such are not sent, as client doesn't need to know that for inventories.
    message.set_itemtype(item->type());
    message.set_itemdetails(item->details());
    message.set_itemname(item->name());
    message.set_itemstate(item->state());
    message.set_stacksize(item->stackSize());
    message.set_index(index);

    switch (item->type()) {
    case Item::ItemType::Torch:
        Torch* torch = dynamic_cast<Torch*>(item);
        message.set_radius(torch->radius());
        break;
    }

    // search for the client associated with this player
    ENetPeer* peer = nullptr;
    for (auto c : m_clients) {
        if (c.second == player) {
            peer = c.first;
            break;
        }
    }

    Packet::sendPacket(peer, &message, Packet::QuickBarInventoryItemFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

