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

#include <map>

class World;
class Player;
class Chunk;

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

    /**
     * Broadcast to clients that this player's position has changed
     */
    void sendPlayerMove(Player* player);

    /**
     * Broadcast a chunk change.
     */
    void sendWorldChunk(Chunk* chunk);

    /**
     * Should be called when player's quickbarinventory has been modified at @p index,
     * as it will then send this to the client associated with this player, with the new up-to-date item.
     */
    void sendPlayerQuickBarInventory(Player* player, uint8_t index);

private:
    /**
     * If the client validated successfully, also creates a player and appends client and player
     * to @sa m_clients
     * @returns uint32_t the Packet::ConnectionEventType. which could possibly be, for instance,
     * a mismatching version, or a non-valid player name...returns Packet::ConnectionEventType::None
     * if nothing bad happened (it's okay to keep them connected).
     */
    uint32_t receiveInitialClientData(std::stringstream* ss, ENetEvent& event);
    void receiveChatMessage(std::stringstream* ss, const std::string& playerName);
    void receivePlayerMove(std::stringstream* ss, Player* player);
    void receivePlayerMouseState(std::stringstream* ss, Player* player);

    void sendChatMessage(const std::string& message, const std::string& playerName);
    void sendInitialPlayerData(ENetPeer* peer, Player* player);
    void sendInitialPlayerDataFinished(ENetPeer* peer);
    void sendInitialWorldChunk(ENetPeer* peer);


    Player* createPlayer(const std::string& playerName);

private:
    /**
     * Will always be the ID after the last used one, so it's always unique and
     * unused and will gradually rise as the player connection count history goes up.
     */
    uint32_t m_freePlayerID = 0;

    World* m_world = nullptr;

private:
    ENetHost* m_server = nullptr;
    ENetAddress m_address;
    std::map<ENetPeer*, Player*> m_clients;
};

#endif
