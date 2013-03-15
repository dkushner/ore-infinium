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

#include <SDL2/SDL.h>
#include <SDL_log.h>
#include <thread>

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

    void tick(double elapsedTime, double fps);
    void render(double elapsedTime);

    void handlePlayerInput(SDL_Event& event);

    const float FPS = 60.0;

    void disconnect();

    void shutdown();

    bool connected() {
        return m_connected;
    }
    double fps() {
        return m_fps;
    }
    bool hosting() {
        return m_server;
    }
    Player* mainPlayer() {
        return m_mainPlayer;
    }

///////////////// Network Communication ////////////////////
public:
    void sendChatMessage(const std::string& message);

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
    void receiveChatMessage(std::stringstream* ss);
    void receiveInitialPlayerData(std::stringstream* ss);
    void receiveQuickBarInventoryItem(std::stringstream* ss);
    void receivePlayerDisconnected(std::stringstream* ss);
    void receivePlayerMove(std::stringstream* ss);
    void receiveChunk(std::stringstream* ss);

///////////////////////////////////////////////

private:
    void initSDL();

    void handleInputEvents();
    void drawDebugText(double frametime);

private:
    Server* m_server = nullptr;
    World* m_world = nullptr;

    GUI* m_gui = nullptr;
    MainMenu* m_mainMenu = nullptr;
    ChatDialog* m_chat = nullptr;
    DebugMenu* m_debugMenu = nullptr;
    QuickBarMenu* m_quickBarMenu = nullptr;

    SDL_Window *m_window = nullptr;
    SDL_GLContext m_GLcontext;

    double m_fps = 0.0;

    std::string m_playerName;

    Player* m_mainPlayer = nullptr;

    std::thread* m_serverThread = nullptr;
    bool m_connected = false;

    int32_t m_playerInputDirectionX = 0;
    int32_t m_playerInputDirectionY = 0;

    bool m_initialPlayersReceivedFinished = false;

private:
    ENetHost* m_client = nullptr;

    /// server
    ENetPeer* m_peer = nullptr;
    /// server address
    ENetAddress m_address;
};

#endif
