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

#ifndef CLIENT_H
#define CLIENT_H

#include <enet/enet.h>
#include <string>

#include "src/player.h"

#include <SDL2/SDL.h>
#include <SDL_log.h>
#include <thread>
#include <Box2D/Dynamics/b2World.h>

class DebugSettings;
class PhysicsDebugRenderer;
class QuickBarMenu;
class Player;
class GUI;
class MainMenu;
class ChatDialog;
class DebugMenu;
class World;
class Server;

class Client
{
public:
    Client();
    ~Client();

    void startSinglePlayer(const std::string& playername);
    /**
     * Starts a mutliplayer client only connection, aka connecting to a server, not hosting.
     * @p address the server address/IP to connect to
     */
    bool startMultiplayerClientConnection(const std::string& playername, const char* address, unsigned int port = 44543);
    /**
     * Similar to @sa startSinglePlayer
     * but instead hosts a multiplayer game, presumably visible to others through internet/LAN ip entering.
     */
    void startMultiplayerHost(const std::string& playername, unsigned int port = 44543);

    void tick(double frameTime);
    void render(double frameTime);

    void handlePlayerInput(SDL_Event& event);

    const float FPS = 60.0;

    void setBox2DWorld(b2World* world) {
        m_box2DWorld = world;
    }

    void disconnect();

    void shutdown();

    bool connected() {
        return m_connected;
    }

    bool hosting() {
        return m_server;
    }

    Entities::Player* mainPlayer() {
        return m_mainPlayer;
    }

///////////////// Network Communication ////////////////////
public:
    void sendChatMessage(const std::string& message);

    /**
     * Called each tick, informs server of the player mouse position and button state,
     * *AND* sets the m_mainPlayer's mouse state to the present state. The latter is important
     * for the world to be able to generalize things regardless if in client or server mode. (e.g. picking vs. crosshair rendering)
     * Sends mouse position in world coordinates, since client coordinates are useless to the server. (since it has no cameras and so
     * doesn't know how to unproject them)
     */
    void sendPlayerMouseState();

    void sendQuickBarInventorySlotSelectRequest(uint8_t index);

private:
    /**
     * blocks until it can connect within a timeout.
     * @returns true on success, false on failure
     */
    bool connect(const char* address = "127.0.0.1", unsigned int port = 44543);
    void sendInitialConnectionData();
    void sendPlayerMovement();

    void poll();

    void processMessage(ENetEvent& event);
    void receiveChatMessage(const std::string& packetContents);
    void receiveInitialPlayerData(const std::string& packetContents);
    void receivePlayerDisconnected(const std::string& packetContents);
    void receivePlayerMove(const std::string& packetContents);
    void receiveChunk(const std::string& packetContents);
    void receiveItemSpawned(const std::string& packetContents);
    void receiveQuickBarInventoryItem(const std::string& packetContents);
    void receiveQuickBarInventoryItemCountChanged(const std::string& packetContents);

///////////////////////////////////////////////

private:
    void initSDL();

    glm::vec2 mousePositionToWorldCoords();

    void handleInputEvents();
    void drawDebugText(double frameTime);

private:
    Server* m_server = nullptr;
    World* m_world = nullptr;

    GUI* m_gui = nullptr;
    MainMenu* m_mainMenu = nullptr;
    ChatDialog* m_chat = nullptr;
    DebugMenu* m_debugMenu = nullptr;
    QuickBarMenu* m_quickBarMenu = nullptr;
    DebugSettings* m_debugSettings = nullptr;

    SDL_Window *m_window = nullptr;
    SDL_GLContext m_GLcontext;

    std::string m_playerName;

    Entities::Player* m_mainPlayer = nullptr;

    std::thread* m_serverThread = nullptr;
    bool m_connected = false;

    int32_t m_playerInputDirectionX = 0;
    int32_t m_playerInputDirectionY = 0;

    b2World* m_box2DWorld = nullptr;
    PhysicsDebugRenderer* m_physicsDebugRenderer = nullptr;

    bool m_playerJumpRequested = false;

    bool m_initialPlayersReceivedFinished = false;

private:
    ENetHost* m_client = nullptr;

    /// server
    ENetPeer* m_peer = nullptr;
    /// server address
    ENetAddress m_address;
};

#endif
