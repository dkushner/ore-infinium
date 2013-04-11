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
#include "src/packet.pb.h"
#include "src/server/server.h"

#include "src/physicsdebugrenderer.h"

#include "gui/gui.h"
#include "gui/mainmenu.h"
#include "gui/chatdialog.h"
#include "gui/debugmenu.h"
#include "gui/quickbarmenu.h"
#include "gui/debugsettings.h"

#include "src/settings/settings.h"

#include "src/quickbarinventory.h"
#include "src/world.h"
#include "src/player.h"
#include "src/camera.h"
#include "src/debug.h"
#include <src/item.h>
#include <src/torch.h>
#include "src/../config.h"

#include <random>

#include <GL/glew.h>

Client::Client()
{
    initSDL();

    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
#endif

    m_gui = GUI::instance();
    m_mainMenu = new MainMenu(this);
    m_mainMenu->showMainMenu();

    m_debugMenu = new DebugMenu(this);
    m_debugMenu->show();

//dws    std::stringstream ss;
//    ss << "Player";
//    std::random_device device;
//    std::mt19937 rand(device());
//    std::uniform_int_distribution<> distribution(0, INT_MAX);
//
//    ss << distribution(rand);
//
//    startMultiplayerHost(ss.str());
}

Client::~Client()
{
    enet_host_destroy(m_client);

    delete m_mainPlayer;

    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise();
#endif

    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Client::initSDL()
{
    Debug::log(Debug::Area::StartupArea) << "SDL on platform: " << SDL_GetPlatform();

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    Debug::log(Debug::Area::StartupArea) << "Compiled against SDL version: " << int(compiled.major) << "." << int(compiled.minor) << "-" << int(compiled.patch) <<
                                    " Running (linked) against version: " << int(linked.major) << "." << int(linked.minor) << "-" << int(linked.patch);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        std::string error = SDL_GetError();
        Debug::fatal(false, Debug::Area::StartupArea, "failure to initialize SDL error: " + error);
    }

    m_window = SDL_CreateWindow("Ore Infinium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                Settings::instance()->windowWidth, Settings::instance()->windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (!m_window) {
        Debug::checkSDLError();
    }

    Debug::checkGLError();
//    glewExperimental = GL_TRUE;
    Debug::checkGLError();
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    Debug::checkGLError();
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    Debug::checkGLError();
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    Debug::checkGLError();

    // Request opengl 3.3 context.
    // FIXME: i *want 3.2, but Mesa 9 only has 3.0.. :(
    Debug::checkGLError();
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    Debug::checkGLError();
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    Debug::checkGLError();

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    Debug::checkGLError();
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    Debug::checkGLError();

//FIXME: doesn't do shit
    SDL_GL_SetSwapInterval(1);
    Debug::checkGLError();

    //TODO: we'll probably need some extension at some point in time..
    //SDL_GL_ExtensionSupported();

    Debug::checkGLError();
    m_GLcontext = SDL_GL_CreateContext(m_window);
    Debug::checkSDLError();
    Debug::checkGLError();

    SDL_ShowCursor(0);
    Debug::checkGLError();

    Debug::checkSDLError();
    Debug::checkGLError();
    Debug::assertf(glewInit() == GLEW_OK, "glewInit returned !GLEW_OK. No GL context can be formed..bailing out");

   Debug::log(Debug::Area::StartupArea) << "Platform: Driver Vendor: " << glGetString(GL_VENDOR);
   Debug::log(Debug::Area::StartupArea) << "Platform: Renderer: " << glGetString(GL_RENDERER);
   Debug::log(Debug::Area::StartupArea) << "OpenGL Version: " << glGetString(GL_VERSION);
   Debug::log(Debug::Area::StartupArea) << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    Debug::checkGLError();

    GLint textureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
    Debug::log(Debug::Area::StartupArea) << "Maximum OpenGL texture size allowed: " << textureSize << "\n\n\n";

#ifdef GLEW_KHR_debug
    if (!GLEW_KHR_debug) {
        Debug::log(Debug::Area::ClientRendererArea) << "FIXME: GLEW_KHR_debug is not available, disabling OpenGL debug mode";
        assert(0);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(&Debug::glDebugCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
#endif

    Debug::fatal(enet_initialize != 0, Debug::Area::StartupArea, "An error occurred during ENet init (network init failure");

    glClearColor(0.f, .5f, 0.f, 1.0f);

    glViewport(0, 0, Settings::instance()->windowWidth, Settings::instance()->windowHeight);

    Debug::checkGLError();
}

void Client::poll()
{
    ENetEvent event;
    int eventStatus;

    while (enet_host_service(m_client, &event, 0)) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            assert(0);
        }
        break;

        case ENET_EVENT_TYPE_RECEIVE:
            //Debug::log(Debug::Area::NetworkClient) << "Message from server, our client->server round trip latency is: " << event.peer->lastRoundTripTime;
            processMessage(event);;
            break;

        case ENET_EVENT_TYPE_DISCONNECT: {
            Debug::log(Debug::Area::NetworkClientContinuousArea) << "Peer disconnected: " << event.data;
            m_connected = false;

            switch (event.data) {
            case Packet::ConnectionEventType::DisconnectedVersionMismatch:
                Debug::log(Debug::Area::NetworkClientContinuousArea) << "Server booted us, client version does not match server version.";
                //FIXME: gracefully handle a version mismatch, obviously
                assert(0);
                break;
            case Packet::ConnectionEventType::DisconnectedInvalidPlayerName:
                Debug::log(Debug::Area::NetworkClientContinuousArea) << "Server booted us, invalid player name";
                assert(0);
                break;
            }

            char hostname[32];
            enet_address_get_host_ip(&event.peer->address, hostname, static_cast<size_t>(32));
            Debug::log(Debug::Area::NetworkClientContinuousArea) << "disconnected from server host IP: " << hostname;
            enet_peer_reset(m_peer);

            // Reset client's information
            event.peer->data = nullptr;
            delete m_peer;
            m_peer = nullptr;
        }
        break;

        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}

void Client::render(double frameTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_world && m_mainPlayer) {
        m_world->render();
    }

    // only a client-hosted server has a chance of seeing any debug shit
    if (m_server) {
        if (!m_physicsDebugRenderer && m_box2DWorld && m_world && m_world->spriteSheetRenderer()) {
            m_physicsDebugRenderer = new PhysicsDebugRenderer(m_world->spriteSheetRenderer()->camera());
            // physics debug renderer first init...
            m_box2DWorld->SetDebugDraw(m_physicsDebugRenderer);
        }

        if (m_box2DWorld && m_physicsDebugRenderer) {
            m_box2DWorld->DrawDebugData();

            int rendererFlags = 0;
            int settingsFlags = Settings::instance()->debugRendererFlags;

            if (settingsFlags & Debug::RenderingDebug::Box2DAABBRenderingDebug) {
                rendererFlags |= b2Draw::e_aabbBit;
            }

            if (settingsFlags & Debug::RenderingDebug::Box2DShapeRenderingDebug) {
                rendererFlags |= b2Draw::e_shapeBit;
            }

            if (settingsFlags & Debug::RenderingDebug::Box2DCenterOfMassRenderingDebug) {
                rendererFlags |= b2Draw::e_centerOfMassBit;
            }

            if (settingsFlags & Debug::RenderingDebug::Box2DJointRenderingDebug) {
                rendererFlags |= b2Draw::e_jointBit;
            }

            m_physicsDebugRenderer->SetFlags(rendererFlags);
            //finalize rendering to screen.
            m_physicsDebugRenderer->render();

            m_debugMenu->setPhysicsWorldBodyCount(m_box2DWorld->GetBodyCount());
        }
    }

    m_gui->render();
    drawDebugText(frameTime);

    SDL_GL_SwapWindow(m_window);
}

void Client::tick(double frameTime)
{
    handleInputEvents();

    if (m_peer) {
        poll();
    }

    if (m_world) {
        m_world->update(frameTime);
    }
}

void Client::drawDebugText(double frameTime)
{
    m_debugMenu->update(frameTime);
}

void Client::handleInputEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        m_gui->handleEvent(event);

        if (m_mainPlayer && m_peer && m_connected && m_gui->inputDemanded() == false) {
            handlePlayerInput(event);
            m_quickBarMenu->handleEvent(event);
        }

        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                //only if we are connected, do we allow hiding and showing (escape menu)
                if (m_peer) {
                    if (!m_mainMenu->escapeMenuVisible()) {
                        m_mainMenu->showEscapeMenu();
                    } else {
                        m_mainMenu->hideEscapeMenu();
                    }
                }
            } else if (event.key.keysym.sym == SDLK_F1) {
                m_debugMenu->setCollapsed(!m_debugMenu->collapsed());
            } else if (event.key.keysym.sym == SDLK_F8) {
                std::stringstream ss;
                ss << "Player";
                std::random_device device;
                std::mt19937 rand(device());
                std::uniform_int_distribution<> distribution(0, INT_MAX);

                ss << distribution(rand);

                startMultiplayerHost(ss.str());
            } else if (event.key.keysym.sym == SDLK_F11) {
               if (m_debugSettings == nullptr) {
                    m_debugSettings = new DebugSettings(this);
                    m_debugSettings->show();
               } else {
                   if (m_debugSettings->visible()) {
                        m_debugSettings->hide();
                   } else {
                        m_debugSettings->show();
                   }
               }
            } else if (event.key.keysym.sym == SDLK_EQUALS || event.key.keysym.sym == SDLK_l) {
                if (m_world) {
                    m_world->zoomIn();
                }
            } else if (event.key.keysym.sym == SDLK_MINUS || event.key.keysym.sym == SDLK_k) {
                if (m_world) {
                    m_world->zoomOut();
                }
            }
            break;

        case SDL_MOUSEWHEEL:
            //greater than 0 is moving mouse wheel *up*, so we want previous.
            if (m_quickBarMenu) {
                if (event.wheel.y > 0) {
                    m_quickBarMenu->previousSlot();
                } else if (event.wheel.y < 0) {
                    m_quickBarMenu->nextSlot();
                }
            }
            break;

        case SDL_WINDOWEVENT_CLOSE:
            //FIXME: fucking useless. doesn't get called for..gee, what would this event be called for? oh yeah, window closing. No, instead that's fucking SDL_QUIT, which is
            // also for some fucking reason, also ctrl-C/break. That shit don't make sense brah

            // if (m_peer) {
            //     m_mainMenu->toggleShown();
            // } else {
            //     shutdown();
            // }
            break;

        case SDL_QUIT:
//            if (m_peer) {
//                m_mainMenu->showEscapeMenu();
//            } else {
            //NOTE: so far this seems to be ctrl-C as well as window close button. Who the fuck knows why the above one does nothing
            shutdown();
            //}
            break;

        default:
            break;
        }
    }
}

void Client::handlePlayerInput(SDL_Event& event)
{
    int32_t originalX = m_playerInputDirectionX;
    int32_t originalY = m_playerInputDirectionY;

    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT) {
            m_playerInputDirectionX = 1;
        }

        if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT) {
            m_playerInputDirectionX = -1;
        }

        if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN) {
            m_playerInputDirectionY = 1;
        }

        if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP) {
            m_playerInputDirectionY = -1;
        }

        if (event.key.keysym.sym == SDLK_SPACE) {
            m_playerJumpRequested = true;
        }
        break;

    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT) {
            m_playerInputDirectionX = 0;
        }

        if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT) {
            m_playerInputDirectionX = 0;
        }

        if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN) {
            m_playerInputDirectionY = 0;
        }

        if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP) {
            m_playerInputDirectionY = 0;
        }
        break;
    }

    if (m_playerInputDirectionX != originalX || m_playerInputDirectionY != originalY || m_playerJumpRequested) {
        sendPlayerMovement();
    }

    sendPlayerMouseState();
}

void Client::shutdown()
{
    if (m_connected) {
        disconnect();
    }
    //FIXME: graceful client shutdown needed..
    exit(0);
}

bool Client::connect(const char* address, unsigned int port)
{
    m_client = enet_host_create(nullptr /* create a client host */,
                                1 /* only allow 1 outgoing connection */,
                                2 /* allow up 2 channels to be used, 0 and 1 */,
                                57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                                14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

    Debug::assertf(m_client, "failed to create ENet client host");

    enet_address_set_host(&m_address, address);
    m_address.port = port;

    m_peer = enet_host_connect(m_client, &m_address, 2, 0);

    if (m_peer == NULL) {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "Client failed to connect to server";
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    if (enet_host_service(m_client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "Client connection to server succeeded!";
        m_mainMenu->hideMainMenu();

        m_chat = new ChatDialog(this, m_mainMenu);
        m_chat->show();

        //NOTE: no world is created yet. we now wait for the server to receive our initial connection data, and give us back a
        //player id, which we then create as the main player and finally, create the world.
        m_connected = true;
        sendInitialConnectionData();
        return true;
    } else {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "Client connection to server failed!";
        Debug::log(Debug::Area::NetworkClientInitialArea) << "Client failed to connect to server within timeout";
        enet_peer_reset(m_peer);
        return false;
    }
}

void Client::disconnect()
{
    Debug::log(Debug::Area::NetworkClientInitialArea) << "attempting disconnect from server";
    enet_peer_disconnect(m_peer, 0);
}

void Client::startSinglePlayer(const std::string& playername)
{
    Debug::log(Debug::Area::NetworkClientInitialArea) << "starting singleplayer! Entities::Playername: " << playername;
    m_playerName = playername;

    //create a local server, and connect to it.
    m_server = new Server(1);
    m_serverThread = new std::thread(&Server::tick, m_server);
    connect();
}

bool Client::startMultiplayerClientConnection(const std::string& playername, const char* address, unsigned int port)
{
    Debug::log(Debug::Area::NetworkClientInitialArea) << "starting multiplayer joining address: " << address << "! Entities::Playername: " << playername;
    m_playerName = playername;

    if (connect(address, port)) {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "connection success!";
        return true;
    } else {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "connection failure!";
        return false;
    }
}

void Client::startMultiplayerHost(const std::string& playername, unsigned int port)
{
    Debug::log(Debug::Area::NetworkClientInitialArea) << "starting multiplayer, hosting! Entities::Playername: " << playername << " port: " << port;
    if (!m_server) {
        m_playerName = playername;

        m_server = new Server(8 /* 8 players (max) */, port, this);
        m_serverThread = new std::thread(&Server::tick, m_server);
        connect();
    } else {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "error, attempted to create player-hosted a server but we're still connected to this one";
    }
}

void Client::sendInitialConnectionData()
{
    PacketBuf::ClientInitialConnection message;
    message.set_playername(m_playerName);
    message.set_versionmajor(ore_infinium_VERSION_MAJOR);
    message.set_versionminor(ore_infinium_VERSION_MINOR);

    Packet::sendPacket(m_peer, &message, Packet::FromClientPacketContents::InitialConnectionDataFromClientPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Client::sendChatMessage(const std::string& message)
{
    PacketBuf::ChatMessageFromClient messagestruct;
    messagestruct.set_message(message);

    Packet::sendPacket(m_peer, &messagestruct, Packet::FromClientPacketContents::ChatMessageFromClientPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Client::sendPlayerMovement()
{
    PacketBuf::PlayerMoveFromClient message;
    message.set_directionx(m_playerInputDirectionX);
    message.set_directiony(m_playerInputDirectionY);
    message.set_jump(m_playerJumpRequested);
    m_playerJumpRequested = false;

    Packet::sendPacket(m_peer, &message, Packet::FromClientPacketContents::PlayerMoveFromClientPacket, ENET_PACKET_FLAG_RELIABLE);
}

glm::vec2 Client::mousePositionToWorldCoords()
{
    assert(m_world);

    int x; int y;
    SDL_GetMouseState(&x, &y);

    glm::vec2 mouse = glm::vec2(x, Settings::instance()->windowHeight - y);

    glm::vec4 viewport = glm::vec4(0, 0, Settings::instance()->windowWidth, Settings::instance()->windowHeight);
    glm::vec3 wincoord = glm::vec3(mouse.x, mouse.y, 0);
    glm::vec3 unproject = glm::unProject(wincoord, m_world->camera()->view(), m_world->camera()->ortho(), viewport);

    mouse = glm::vec2(unproject.x, unproject.y);

    return mouse;
}

void Client::sendPlayerMouseState()
{
    glm::vec2 mousePosition = mousePositionToWorldCoords();

    //set the main player's mouse position, which we can then use for generalized lookups regardless if we're in server or client mode
    // (e.g. rendering crosshair vs. networked picking/block selection..both would otherwise require two different solutions but now do not)
    m_mainPlayer->setMousePositionWorldCoords(mousePosition.x, mousePosition.y);

    bool leftHeld = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(1);
    bool rightHeld = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(3);

    PacketBuf::PlayerMouseStateFromClient message;
    message.set_x(mousePosition.x);
    message.set_y(mousePosition.y);
    message.set_leftbuttonheld(leftHeld);
    message.set_rightbuttonheld(rightHeld);


    //FIXME: make UNRELIABLE, and verify it is safe to do so..
    Packet::sendPacket(m_peer, &message, Packet::FromClientPacketContents::PlayerMouseStateFromClient, ENET_PACKET_FLAG_RELIABLE);
}

void Client::sendQuickBarInventorySlotSelectRequest(uint8_t index)
{
    PacketBuf::QuickBarInventorySelectSlotRequestFromClient message;
    message.set_index(index);

    Packet::sendPacket(m_peer, &message, Packet::FromClientPacketContents::QuickBarInventorySelectSlotRequestFromClient, ENET_PACKET_FLAG_RELIABLE);
}

void Client::processMessage(ENetEvent& event)
{
    std::stringstream ss(std::string((char*)(event.packet->data), event.packet->dataLength));

    uint32_t packetType = Packet::deserializePacketType(ss);

    switch (packetType) {
    case Packet::FromServerPacketContents::ChatMessageFromServerPacket:
        receiveChatMessage(&ss);
        break;

    case Packet::FromServerPacketContents::InitialPlayerDataFromServerPacket:
        receiveInitialPlayerData(&ss);
        break;

    case Packet::FromServerPacketContents::PlayerDisconnectedFromServerPacket:
        receivePlayerDisconnected(&ss);
        break;

    case Packet::FromServerPacketContents::PlayerMoveFromServerPacket:
        receivePlayerMove(&ss);
        break;

    case Packet::FromServerPacketContents::InitialPlayerDataFinishedFromServerPacket:
        m_initialPlayersReceivedFinished = true;
        break;

    case Packet::FromServerPacketContents::ChunkFromServerPacket:
        receiveChunk(&ss);
        break;

    case Packet::QuickBarInventoryItemFromServerPacket:
        receiveQuickBarInventoryItem(&ss);
        break;

    case Packet::QuickBarInventoryItemCountChangedFromServerPacket:
        receiveQuickBarInventoryItemCountChanged(&ss);
        break;

    case Packet::ItemSpawnedFromServerPacket:
        receiveItemSpawned(&ss);
        break;
    }

    enet_packet_destroy(event.packet);
}

void Client::receiveChatMessage(std::stringstream* ss)
{
    PacketBuf::ChatMessageFromServer chatMessage;
    Packet::deserialize(ss, &chatMessage);
    m_chat->addChatLine(chatMessage.playername(), chatMessage.message());
}

void Client::receiveInitialPlayerData(std::stringstream* ss)
{
    PacketBuf::InitialPlayerDataFromServer message;
    Packet::deserialize(ss, &message);
    Debug::log(Debug::Area::NetworkClientInitialArea) << "initial player data received";

    Entities::Player* player = new Entities::Player("player1Standing1");
    std::stringstream chatMessage;
    if (!m_mainPlayer) {
        //this is must be *our* player, so create it
        m_mainPlayer = player;
        m_mainPlayer->setName(message.playername());
        m_mainPlayer->setPlayerID(message.playerid());
        m_mainPlayer->setPosition(message.x(), message.y());

        Debug::log(Debug::Area::NetworkClientInitialArea) << "initial player data received, pos: x: " << m_mainPlayer->position().x << " Y: " << m_mainPlayer->position().y;

        QuickBarInventory* quickBarInventory = new QuickBarInventory();
        m_mainPlayer->setQuickBarInventory(quickBarInventory);

        chatMessage << m_mainPlayer->name() << " has joined";
        m_chat->addChatLine("", chatMessage.str());

        // this is us, the first player so this means the world creation is up to us
        m_world = new World(m_mainPlayer, this, nullptr);

        m_quickBarMenu = new QuickBarMenu(this, quickBarInventory, m_world->spriteSheetRenderer());
        m_quickBarMenu->show();

    } else {
        player->setName(message.playername());
        player->setPlayerID(message.playerid());
        player->setPosition(message.x(), message.y());

        chatMessage << player->name() << " has joined";

        if (m_initialPlayersReceivedFinished) {
            m_chat->addChatLine("", chatMessage.str());
        } else {
            Debug::log(Debug::Area::NetworkClientInitialArea) << "Player name: " << player->name() << " we're not adding the chat line because we haven't finished receiving initial client data";
        }
    }

    m_world->addPlayer(player);
}

void Client::receivePlayerDisconnected(std::stringstream* ss)
{
    PacketBuf::PlayerDisconnectedFromServer message;
    Packet::deserialize(ss, &message);

    Debug::assertf(m_world, "WARNING:, player disconnected, client attempted to remove him from the worlds player list..but we dont' have a world yet here? seems odd");
    m_world->findPlayer(message.playerid());
}

void Client::receivePlayerMove(std::stringstream* ss)
{
    PacketBuf::PlayerMoveFromServer message;
    Packet::deserialize(ss, &message);

    Entities::Player* player = m_world->findPlayer(message.playerid());
    player->setPosition(message.x(), message.y());
    Debug::log(Debug::Area::NetworkClientContinuousArea) << "Setting player position to: " << player->position().x << " Y: " << player->position().y;
}

void Client::receiveChunk(std::stringstream* ss)
{
    PacketBuf::Chunk message;
    Packet::deserialize(ss, &message);

    std::vector<Block> blocks;

    int index = 0;
    for (int row = message.starty(); row < message.endy(); ++row) {
        for (int column = message.startx(); column < message.endx(); ++column) {
            Block block;
            block.meshType = message.meshtype(index);
            block.primitiveType = message.primitivetype(index);
            block.wallType = message.walltype(index);
            blocks.push_back(block);

            ++index;
        }
    }

    Chunk chunk(message.startx(), message.starty(), message.endx(), message.endy(), &blocks);

    m_world->loadChunk(&chunk);
}

void Client::receiveQuickBarInventoryItemCountChanged(std::stringstream* ss)
{
    PacketBuf::ItemCountChanged message;
    Packet::deserialize(ss, &message);

    if (message.newcount() == 0) {
        m_quickBarMenu->inventory()->deleteItem(message.index());
    } else {
        m_quickBarMenu->inventory()->item(message.index())->setStackSize(message.newcount());
    }

    m_quickBarMenu->reloadSlot(message.index());
}

void Client::receiveQuickBarInventoryItem(std::stringstream* ss)
{
    PacketBuf::Item message;
    Packet::deserialize(ss, &message);

    //position isn't used even..but who cares.
    const glm::vec2 position = glm::vec2(message.x(), message.y());

    Item *baseItem = nullptr;
    switch (message.itemtype()) {
    case Item::ItemType::Block:
        break;

    case Item::ItemType::Torch: {
        Torch *torch = new Torch(position);
        torch->setRadius(message.radius());

        baseItem = torch;
        break;
    }
    }

    //NOTE: we don't give a shit about position and such, the server doesn't even send that
    //in inventory scenarios, since it doesn't matter.
    baseItem->setStackSize(message.stacksize());
    baseItem->setName(message.itemname());
    baseItem->setDetails(message.itemdetails());
    baseItem->setState(message.itemstate());

    uint32_t index = message.index();

    //delete the old one as we'll get resent it in whatever new form it is in (whether it's moving to the world, or to a different inventory)
    m_quickBarMenu->inventory()->deleteItem(index);

    m_quickBarMenu->inventory()->setSlot(index, baseItem);
    m_quickBarMenu->reloadSlot(index);
}

void Client::receiveItemSpawned(std::stringstream* ss)
{
    PacketBuf::Item message;
    Packet::deserialize(ss, &message);

    const glm::vec2 position = glm::vec2(message.x(), message.y());

    Item *baseItem = nullptr;
    switch (message.itemtype()) {
    case Item::ItemType::Block:
        break;

    case Item::ItemType::Torch: {
        Torch *torch = new Torch(position);
        torch->setRadius(message.radius());

        baseItem = torch;
        break;
    }
    }

    baseItem->setStackSize(message.stacksize());
    baseItem->setName(message.itemname());
    baseItem->setDetails(message.itemdetails());
    baseItem->setState(message.itemstate());

    m_world->spawnItem(baseItem);
}
