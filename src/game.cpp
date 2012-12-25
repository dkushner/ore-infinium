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
//#include "entity.h"
//#include "imagemanager.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>


#include <GL/glew.h>
#include <GL/gl.h>

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

void Game::init()
{

//    version = al_get_opengl_version();
//    major = version >> 24;
//    minor = (version >> 16) & 255;
//    revision = (version >> 8) & 255;
//
//    std::cout << "\n\n\n\n";
//    Debug::log(Debug::Area::Graphics) << "Hardware we're running on...";
//    Debug::log(Debug::Area::Graphics) << major << "." << minor << "." << revision;
//
//    int glVariant = al_get_opengl_variant();
//
//    if (glVariant & ALLEGRO_DESKTOP_OPENGL) {
//        Debug::log(Debug::Area::Graphics) << "Using desktop OpenGL variant.";
//    } else if (glVariant & ALLEGRO_OPENGL_ES) {
//        Debug::log(Debug::Area::Graphics) << "Using OpenGL ES OpenGL variant.";
//    }
//
//    Debug::log(Debug::Area::Graphics) << "Platform: Driver Vendor: " << glGetString(GL_VENDOR);
//    Debug::log(Debug::Area::Graphics) << "Platform: Renderer: " << glGetString(GL_RENDERER);
//    Debug::log(Debug::Area::Graphics) << "OpenGL Version: " << glGetString(GL_VERSION);
//    Debug::log(Debug::Area::Graphics) << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
//
  //  GLint textureSize;
//    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
 //   Debug::log(Debug::Area::Graphics) << "Maximum OpenGL texture size allowed: " << textureSize;
    std::cout << "\n\n\n\n";

    Debug::fatal(m_font = al_load_ttf_font("../font/Ubuntu-L.ttf", 12, 0), Debug::Area::System, "Failure to load font");

//    ImageManager* manager = ImageManager::instance();
//    manager->addResourceDir("../textures/");

    //World::createInstance(m_display);
    //m_world = World::instance();

    tick();
    shutdown();
}

void Game::tick()
{

    std::stringstream ss;
    std::string str;

    int fps = 0;

    const int MAX_BENCH = 300;
    int benchTime = MAX_BENCH;

    double oldTime = al_get_time();

    while (m_running) {
        double newTime = al_get_time();
        double delta = newTime - oldTime;
        double fps = 1 / (delta);
        oldTime = newTime;

        ALLEGRO_EVENT event;

        while (al_get_next_event(m_events, &event)) {
            //m_world->handleEvent(event);
            switch (event.type) {
                // window closed
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                m_running = false;
                break;

                // key pressed
                case ALLEGRO_EVENT_KEY_DOWN:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    Debug::log(Debug::Area::System) << "shutting down...";
                    goto shutdown;
                }
                break;

                default:
                    break;
            }
        }

            ss.str("");
            ss << "FPS: " << fps;
            str = ss.str();

           // m_world->update(static_cast<float>(delta));
           // m_world->render();

            //rendering always before this
    }

shutdown:
    shutdown();
}

void Game::shutdown()
{
    exit(0);
}
