/******************************************************************************
 *   Copyright (C) 2012 by Shaun Reich <sreich@kde.org>                       *
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

#include "game.h"

#include "debug.h"
#include "spritesheetmanager.h"
#include "fontmanager.h"
#include "sprite.h"
#include "src/gui/gui.h"

#include "gui/core/ShellRenderInterfaceOpenGL.h"
#include "gui/core/SystemInterfaceSDL2.h"
#include "gui/core/ShellFileInterface.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include <vector>

#include <FreeImage.h>

#include <assert.h>
#include <Rocket/Core.h>
#include <Rocket/Debugger.h>
#include <Rocket/Controls.h>

Game::Game()
{
}

Game::~Game()
{
}

void Game::abort_game(const char* message)
{
    printf("%s \n", message);
    shutdown();
    exit(1);
}

void Game::checkSDLError()
{
    std::string error = SDL_GetError();
    if (*error.c_str() != '\0') {
        Debug::log(Debug::Area::System) << "SDL Error: " << error;
        SDL_ClearError();
    }
}

void Game::checkGLError()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << gluErrorString(error);
        assert(0);
    }
}

void Game::init()
{
    Debug::log(Debug::Area::System) << "SDL on platform: " << SDL_GetPlatform();

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    Debug::log(Debug::Area::System) << "Compiled against SDL version: " << int(compiled.major) << "." << int(compiled.minor) << "-" << int(compiled.patch) <<
                                    " Running (linked) against version: " << int(linked.major) << "." << int(linked.minor) << "-" << int(linked.patch);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        std::string error = SDL_GetError();
        Debug::fatal(false, Debug::Area::System, "failure to initialize SDL error: " + error);
    }

    m_window = SDL_CreateWindow("Ore Infinium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (!m_window) {
        checkSDLError();
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    // Request opengl 3.3 context.
    // FIXME: i *want 3.2, but Mesa 9 only has 3.0.. :(
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //TODO: we'll probably need some extension at some point in time..
    //SDL_GL_ExtensionSupported();

    m_context = SDL_GL_CreateContext(m_window);
    checkGLError();

    checkSDLError();
    Debug::assertf(glewInit() == GLEW_OK, "glewInit returned !GLEW_OK. No GL context can be formed..bailing out");

    Debug::log(Debug::Area::Graphics) << "Platform: Driver Vendor: " << glGetString(GL_VENDOR);
    Debug::log(Debug::Area::Graphics) << "Platform: Renderer: " << glGetString(GL_RENDERER);
    Debug::log(Debug::Area::Graphics) << "OpenGL Version: " << glGetString(GL_VERSION);
    Debug::log(Debug::Area::Graphics) << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    checkGLError();

    GLint textureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
    Debug::log(Debug::Area::Graphics) << "Maximum OpenGL texture size allowed: " << textureSize;
    std::cout << "\n\n\n\n";

    if (!GLEW_KHR_debug) {
        Debug::log(Debug::Area::Graphics) << "GLEW_KHR_debug is not available, disabling OpenGL debug mode (TODO)";
        assert(0);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(&Debug::glDebugCallback, 0);
    glDebugMessageControl(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE,0,0,GL_TRUE);

    m_font = FontManager::instance()->loadFont("../font/Ubuntu-L.ttf");

//    glClearColor(.5f, 0.f, 0.f, 1.0f);

//    glViewport(0, 0, SCREEN_W, SCREEN_H);

    checkGLError();

//    m_gui = new GUI();

 //   World::createInstance();
 //   m_world = World::instance();

    m_font->FaceSize(12);

    tick();
    shutdown();
}

double fps = 0.0;

void Game::tick()
{    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1600, 900, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;
    SystemInterfaceSDL2 systemInterface;
    Rocket::Core::SetSystemInterface(&systemInterface);

    ShellRenderInterfaceOpenGL openglRenderer;
    Rocket::Core::SetRenderInterface(&openglRenderer);

    ShellFileInterface fileInterface("../gui/assets");
    Rocket::Core::SetFileInterface(&fileInterface);

    Debug::fatal(Rocket::Core::Initialise(), Debug::Area::System, "rocket init failure");
    Rocket::Controls::Initialise();

    Rocket::Core::Context *context = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(1600, 900));

    Rocket::Debugger::Initialise(context);
    Rocket::Debugger::SetVisible(true);

    bool success = Rocket::Core::FontDatabase::LoadFontFace("../gui/assets/Delicious-Roman.otf");
    assert(success);

    Rocket::Core::ElementDocument *doc = context->LoadDocument("../gui/assets/test.rml");
    doc->Show();
//    assert(0);

    while (m_running) {
        const double delta = static_cast<double>(SDL_GetTicks() - startTime);
        fps = (frameCount / delta) * 1000;

        handleEvents();

//        m_world->update(delta);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        m_world->render();

 //       drawDebugText();

        context->Update();
        context->Render();

        SDL_GL_SwapWindow(m_window);

        ++frameCount;
    }

shutdown:
    shutdown();
}

void Game::handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

//        m_world->handleEvent(event);

        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                m_running = false;
            }
            break;

        case SDL_WINDOWEVENT_CLOSE:
            m_running = false;
            break;

        case SDL_QUIT:
            exit(0);
            break;

        default:
            break;
        }
    }
}

void Game::drawDebugText()
{
    std::stringstream ss;
    std::string str;

    ss.str("");
    ss << "FPS: " << fps;
    str = ss.str();
    m_font->Render(str.c_str(), -1, FTPoint(0.0, 0.0, 0.0));
}

void Game::shutdown()
{
    SDL_Quit();
    exit(0);
}
