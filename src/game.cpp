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

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_physfs.h>
#include <allegro5/allegro_primitives.h>
//#include <allegro5/allegro_shader.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/keyboard.h>
#include <allegro5/mouse.h>

#include <allegro5/allegro_shader_glsl.h>

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
    uint32_t version = al_get_allegro_version();
    int major = version >> 24;
    int minor = (version >> 16) & 255;
    int revision = (version >> 8) & 255;
    int release = version & 255;
    Debug::log(Debug::Area::System) << "Using allegro version: " << major << "." << minor << "." << revision << "." << release;

    Debug::fatal(al_init(), Debug::Area::System, "Failure to init allegro");

    al_init_font_addon();
    Debug::fatal(al_init_acodec_addon(), Debug::Area::System, "Failure to init acodec addon");
////FIXME:    Debug::fatal(al_init_native_dialog_addon(), Debug::Area::System, "Failure to init native dialog addon");
    Debug::fatal(al_init_primitives_addon(), Debug::Area::System, "Failure to init primitives addon");
    Debug::fatal(al_init_ttf_addon(), Debug::Area::System, "Failure to init ttf addon");
    Debug::fatal(al_init_image_addon(), Debug::Area::System, "Failure to init image addon");
    Debug::fatal(al_install_keyboard(), Debug::Area::System, "Failure to install keyboard");
    Debug::fatal(al_install_mouse(), Debug::Area::System, "Failure to install mouse");
//
    al_set_new_display_flags( ALLEGRO_OPENGL | ALLEGRO_OPENGL_FORWARD_COMPATIBLE);
//    al_set_new_display_option( ALLEGRO_VSYNC, 2, ALLEGRO_REQUIRE );

    m_display = al_create_display(SCREEN_W, SCREEN_H);
    Debug::fatal(m_display, Debug::Area::Graphics, "display creation failed");


    //FIXME: WHY THE FUCK DO I NEED THIS? ALLEGRO SHOULD GIVE ME A FUCKING OPENGL CONTEXT BECAUSE I MOTHERFUCKING ASKED FOR IT
    //FUCKING HELL..
    glewInit();

    if (!al_have_opengl_extension("GL_EXT_framebuffer_object")
        && !al_have_opengl_extension("GL_ARB_fragment_shader")) {
        Debug::fatal(true, Debug::Area::Graphics, ("Fragment shaders not supported.\n"));
    }

//    glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
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

    al_set_app_name("ore-chasm");
    al_set_window_title(m_display, "Ore Chasm");

    Debug::fatal(m_events = al_create_event_queue(), Debug::Area::System, "event queue init");

    al_register_event_source(m_events, al_get_display_event_source(m_display));
    al_register_event_source(m_events, al_get_mouse_event_source());
    al_register_event_source(m_events, al_get_keyboard_event_source());

    Debug::fatal(m_font = al_load_ttf_font("../font/Ubuntu-L.ttf", 12, 0), Debug::Area::System, "Failure to load font");

//    ImageManager* manager = ImageManager::instance();
//    manager->addResourceDir("../textures/");

    World::createInstance(m_display);
    m_world = World::instance();

    // ----------------------------------------------------- initial render --------------------------------------------------
    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_flip_display();

    tick();
    shutdown();
}


ALLEGRO_BITMAP *mysha;
ALLEGRO_BITMAP *buffer;

const char *tinter_shader_src[] = {
    "uniform sampler2D backBuffer;",
    "uniform float r;",
    "uniform float g;",
    "uniform float b;",
    "uniform float ratio;",
    "void main() {",
    " vec4 color;",
    " float avg, dr, dg, db;",
    " color = texture2D(backBuffer, gl_TexCoord[0].st);",
    " avg = (color.r + color.g + color.b) / 3.0;",
    " dr = avg * r;",
    " dg = avg * g;",
    " db = avg * b;",
    " color.r = color.r - (ratio * (color.r - dr));",
    " color.g = color.g - (ratio * (color.g - dg));",
    " color.b = color.b - (ratio * (color.b - db));",
    " gl_FragColor = color;",
    "}"
    };




void Game::tick()
{

    Debug::log() << glGetString(GL_VERSION);


    //FIXME: needed??
    //    al_set_opengl_program_object(m_display, al_get_opengl_program_object(m_shader));
//    int loc = glGetUniformLocation(program, "tile_types_super_texture");
//    glUniform1i(loc, 0);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, al_get_opengl_texture(m_tileMapFinalTexture));
//


    ////////////////////////////////////////////////// END ISSUE FIXME


    GLhandleARB tinter;
    GLhandleARB tinter_shader;
    

    
    const int TINTER_LEN = 18;
    double start;
    GLint loc;
    

    buffer = al_create_bitmap(320, 200);
    mysha = al_load_bitmap("mysha.png");
    
    if (!al_have_opengl_extension("GL_EXT_framebuffer_object")
        && !al_have_opengl_extension("GL_ARB_fragment_shader")) {
        assert(0);
        }
        
        tinter_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    
    glShaderSourceARB(tinter_shader, TINTER_LEN, tinter_shader_src, NULL);
    glCompileShaderARB(tinter_shader);
    tinter = glCreateProgramObjectARB();
    glAttachObjectARB(tinter, tinter_shader);
    glLinkProgramARB(tinter);
    loc = glGetUniformLocationARB(tinter, "backBuffer");
    glUniform1iARB(loc, al_get_opengl_texture(buffer));
    
    

    float r = 0.5, g = 0.5, b = 1, ratio = 0;
    



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
            m_world->handleEvent(event);
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

                case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
                    //FIXME: pause game
                break;

                case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
                    //FIXME: resume game
                break;

//                case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
                    //FIXME: for android
//                    break;

//                case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
                    //FIXME: for android
//                    break;

                default:
                    break;
            }
        }

            // if there are events to process, lets suspend drawing for a tick
 //           al_clear_to_color(al_map_rgb(0,0,0));
 
 al_set_target_bitmap(buffer);
 
 glUseProgramObjectARB(tinter);
 loc = glGetUniformLocationARB(tinter, "ratio");
 glUniform1fARB(loc, ratio);
 loc = glGetUniformLocationARB(tinter, "r");
 glUniform1fARB(loc, r);
 loc = glGetUniformLocationARB(tinter, "g");
 glUniform1fARB(loc, g);
 loc = glGetUniformLocationARB(tinter, "b");
 glUniform1fARB(loc, b);
 al_draw_bitmap(mysha, 0, 0, 0);
 glUseProgramObjectARB(0);
 
 
 
 al_set_target_backbuffer(m_display);
 al_draw_bitmap(buffer, 0, 0, 0);
 al_flip_display();
 al_rest(0.001);
 

            ss.str("");
            ss << "FPS: " << fps;
            str = ss.str();
//            al_draw_text(m_font, al_map_rgb(255, 255, 0), 0, 0, ALLEGRO_ALIGN_LEFT, str.c_str());

  //          m_world->update(static_cast<float>(delta));
   //         m_world->render();
            //rendering always before this
//            al_flip_display();
    }

shutdown:
    shutdown();
}

void Game::shutdown()
{
    al_destroy_display(m_display);
    al_destroy_event_queue(m_events);
    exit(0);
}
