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

#include <ft2build.h>
#include FT_FREETYPE_H

#include <FTGL/ftgl.h>

#include <enet/enet.h>
#include <string>

#include <SDL2/SDL.h>
#include <SDL_log.h>

class Player;
class GUI;
class MainMenu;
class ChatDialog;
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
    void startMultiplayerClientConnection(const std::string& playername, const char* address, unsigned int port = 44543);

    void tick(double elapsedTime, double fps);
    void render(double elapsedTime);

    const float FPS = 60.0;

    void disconnect();

    void shutdown();

///////////////// Network Communication ////////////////////
public:
    void sendChatMessage(const std::string& message);

private:
    /**
     * blocks until it can connect within a timeout.
     * @returns true on success, false on failure
     */
    bool connect(const char* address = "127.0.0.1", unsigned int port = 44543);
    void sendInitialConnectionData();
///////////////////////////////////////////////

private:
    void initSDL();

    void handleInputEvents();
    void drawDebugText(double frametime);
    void poll();

private:
    Server* m_server = nullptr;
    World* m_world = nullptr;

    GUI* m_gui = nullptr;
    MainMenu* m_mainMenu = nullptr;
    ChatDialog* m_chat = nullptr;

    FTGLPixmapFont *m_font = nullptr;

    SDL_Window *m_window = nullptr;
    SDL_GLContext m_GLcontext;

    double m_fps = 0.0;

    std::string m_playerName;

    Player* m_player = nullptr;

private:
    ENetHost* m_client = nullptr;

    /// server
    ENetPeer* m_peer = nullptr;
    /// server address
    ENetAddress m_address;
};

#endif
