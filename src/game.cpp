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

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include <vector>

#include <FreeImage.h>

#include <assert.h>

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
    if (*error.c_str() != '\0')
    {
        Debug::log(Debug::Area::System) << "SDL Error: " << error;
        SDL_ClearError();
    }
}

void Game::checkGLError()
{
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
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

    m_window = SDL_CreateWindow("Ore Chasm", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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
    glewInit();

    Debug::log(Debug::Area::Graphics) << "Platform: Driver Vendor: " << glGetString(GL_VENDOR);
    Debug::log(Debug::Area::Graphics) << "Platform: Renderer: " << glGetString(GL_RENDERER);
    Debug::log(Debug::Area::Graphics) << "OpenGL Version: " << glGetString(GL_VERSION);
    Debug::log(Debug::Area::Graphics) << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    checkGLError();

    GLint textureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
    Debug::log(Debug::Area::Graphics) << "Maximum OpenGL texture size allowed: " << textureSize;
    std::cout << "\n\n\n\n";

    m_font = FontManager::instance()->loadFont("../font/Ubuntu-L.ttf");

    glClearColor(1.f, 1.f, 1.f, 1.0f);

    glViewport(0, 0, SCREEN_W, SCREEN_H);

    checkGLError();

 //   m_camera = new Camera();

    checkGLError();

//    Sprite* sprite = new Sprite("testframe", SpriteSheetManager::SpriteSheetType::Character);

    //World::createInstance(m_display);
    //m_world = World::instance();
    m_font->FaceSize(12);

    tick();
    shutdown();
}

/* Each vertex is:
 * two floats for the 2d coordinate
 * four u8s for the color
 * two f32s for the texcoords
 * the vbo contains data of the aforementioned elements interleaved.
 * Each sprite has four vertices.
 */
typedef float spriteVertex[5];
    GLuint new_vao = 0;
    GLuint new_vbo = 0;
    GLuint new_ebo = 0;
     GLuint new_vs;
    GLuint new_fs;
    GLuint new_sp;
 
void initGL() {
   
    glGenVertexArrays(1, &new_vao);
    glBindVertexArray(new_vao);
    
    glGenBuffers(1,&new_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,new_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        max_batch_size * 4 * sizeof(spriteVertex)),
                 NULL,
                 GL_DYNAMIC_DRAW);
    
    GLenum err = glGetError();
    if (err)
    {
        DebugConsole::get().gfxerr()
        << "SpriteBatch::init::glBufferData "
        << gl_error_string(err) << std::endl;
        
        glDeleteVertexArrays(1,&new_vao);
        glDeleteBuffers(1,&new_vbo);
        return false;
    }
    
    std::vector<u32> indicesv;
    
    // prepare and upload indices as a one time deal
    const u32 indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (size_t j = 0; j < max_batch_size; j++)
    {
        for (size_t i = 0; i < sizeof(indices)/sizeof(*indices); i++)
        {
            indicesv.push_back(4*j + indices[i]);
        }
    }
    
    glGenBuffers(1,&new_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,new_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(u32),
                 indicesv.data(),
                 GL_STATIC_DRAW);
    
    err = glGetError();
    if (err)
    {
        
        glDeleteVertexArrays(1,&new_vao);
        glDeleteBuffers(1,&new_vbo);
        glDeleteBuffers(1,&new_ebo);
        assert(0);
    }
    
   
        static const char* vshader_src = 
        "#version 120\n"
        
        "attribute vec2 position;"
        
        "attribute vec2 texcoord;"
        "varying vec2 frag_texcoord;"
        
        "attribute vec4 color;"
        "varying vec4 frag_color;"
        
        "void main() {"
        "    gl_Position = vec4( position, 0.0, 1.0 );"
        "    frag_texcoord = texcoord;"
        "    frag_color = color;"
        "}";
        
        static const char* fshader_src =
        "#version 120\n"
        
        "varying vec2 frag_texcoord;"
        "varying vec4 frag_color;"
        
        "uniform sampler2D sampler;"
        
        "void main(void) {"
        "    gl_FragColor = frag_color * texture2D(sampler,frag_texcoord);"
        "}";
        
        GLint status;
        GLchar infolog[1024];
        GLsizei infolog_len;
        
        new_vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(new_vs, 1, &vshader_src, NULL);
        glCompileShader(new_vs);
        
        glGetShaderiv(new_vs,GL_COMPILE_STATUS,&status);
        
        if (status == GL_FALSE)
        {
            glGetShaderInfoLog(new_vs,sizeof(infolog),&infolog_len,infolog);
            std::cout
            << "Failed to compile SpriteBatch vertex shader."
            << std::endl
            << infolog << std::endl;
            
            glDeleteVertexArrays(1,&new_vao);
            glDeleteBuffers(1,&new_vbo);
            glDeleteBuffers(1,&new_ebo);
            glDeleteShader(new_vs);
        }
        
        new_fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(new_fs, 1, &fshader_src, NULL);
        glCompileShader(new_fs);
        
        glGetShaderiv(new_fs,GL_COMPILE_STATUS,&status);
        
        if (status == GL_FALSE)
        {
            glGetShaderInfoLog(new_fs,sizeof(infolog),&infolog_len,infolog);
            std::cout
            << "Failed to compile SpriteBatch fragment shader."
            << std::endl
            << infolog << std::endl;
            
            glDeleteVertexArrays(1,&new_vao);
            glDeleteBuffers(1,&new_vbo);
            glDeleteBuffers(1,&new_ebo);
            glDeleteShader(new_vs);
            glDeleteShader(new_fs);
            assert(0);
        }
        
        new_sp = glCreateProgram();
        glAttachShader(new_sp, new_vs);
        glAttachShader(new_sp, new_fs);
        glLinkProgram(new_sp);
        
        glGetProgramiv(new_sp,GL_LINK_STATUS,&status);
        
        if (status == GL_FALSE)
        {
            glGetProgramInfoLog(new_sp,sizeof(infolog),&infolog_len,infolog);
            std::cout << "Failed to link SpriteBatch shader program."
            << std::endl
            << infolog << std::endl;
            
            glDeleteVertexArrays(1,&new_vao);
            glDeleteBuffers(1,&new_vbo);
            glDeleteBuffers(1,&new_ebo);
            glDeleteShader(new_vs);
            glDeleteShader(new_fs);
            glDeleteProgram(new_sp);
            assert(0);
        }
        
        glValidateProgram(new_sp);
        glGetProgramiv(new_sp,GL_VALIDATE_STATUS,&status);
        
        if (status == GL_FALSE)
        {
            
            glDeleteVertexArrays(1,&new_vao);
            glDeleteBuffers(1,&new_vbo);
            glDeleteBuffers(1,&new_ebo);
            glDeleteShader(new_vs);
            glDeleteShader(new_fs);
            glDeleteProgram(new_sp);
            assert(0);
        }
        
        size_t buffer_offset = 0;
        
        GLint pos_attrib = glGetAttribLocation(new_sp, "position");
        glEnableVertexAttribArray(pos_attrib);
        glVertexAttribPointer(
            pos_attrib,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(spriteVertex),
                              (const GLvoid*)buffer_offset);
        buffer_offset += sizeof(f32) * 2;
        
        GLint color_attrib = glGetAttribLocation(new_sp, "color");
        
        glEnableVertexAttribArray(color_attrib);
        glVertexAttribPointer(
            color_attrib,
            4,
            GL_UNSIGNED_BYTE,
            GL_TRUE,
            sizeof(spriteVertex),
                              (const GLvoid*)buffer_offset);
        buffer_offset += sizeof(u32);
        
        GLint texcoord_attrib = glGetAttribLocation(new_sp, "texcoord");
        glEnableVertexAttribArray(texcoord_attrib);
        glVertexAttribPointer(
            texcoord_attrib,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(spriteVertex),
                              (const GLvoid*)buffer_offset);
        
        err = glGetError();
        if (err)
        {

            glDeleteVertexArrays(1,&new_vao);
            glDeleteBuffers(1,&new_vbo);
            glDeleteBuffers(1,&new_ebo);

            assert(0);
        }
        
        
}

void render() {
    // vertices that will be uploaded.
    spriteVertex vertices[4];

    // transform vertices and copy them to the buffer
    vertices[0][0] = vertices[0][1] = vertices[1][0] = vertices[3][1] = 0;
/*    vertices[1][1] = f32(tex.size().y()) * std::abs(uvrect.height);
    vertices[2][0] = f32(tex.size().x()) * std::abs(uvrect.width);
    vertices[2][1] = f32(tex.size().y()) * std::abs(uvrect.height);
    vertices[3][0] = f32(tex.size().x()) * std::abs(uvrect.width);
    */

/*    vertices[1][1] = f32(tex.size().y()) * std::abs(uvrect.height);
    vertices[2][0] = f32(tex.size().x()) * std::abs(uvrect.width);
    vertices[2][1] = f32(tex.size().y()) * std::abs(uvrect.height);
    vertices[3][0] = f32(tex.size().x()) * std::abs(uvrect.width);
    */
   
    // copy color to the buffer
    // copy color to the buffer
    for (size_t i = 0; i < sizeof(vertices)/sizeof(*vertices); i++)
    {
        uint32_t* colorp = reinterpret_cast<uint32_t*>(&vertices[i][2]);
        //        *colorp = color.bgra;
        uint8_t red = 255;
        uint8_t blue = 0;
        uint8_t green = 255;
        uint8_t alpha = 255;
        int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
        *colorp = color;
    }

    // copy texcoords to the buffer
    vertices[0][3] = vertices[1][3] = uvrect.left;
    vertices[0][4] = vertices[3][4] = uvrect.top + uvrect.height;
    vertices[1][4] = vertices[2][4] = uvrect.top;
    vertices[2][3] = vertices[3][3] = uvrect.left + uvrect.width;
    
    // finally upload everything to the actual vbo
    glBindBuffer(GL_ARRAY_BUFFER,new_vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        batch_size_ * sizeof(vertices),
                    sizeof(vertices),
                    vertices);
    
}

double fps = 0.0;

void Game::tick()
{
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;

        initGL();
    while (m_running) {
        fps =(frameCount / float(SDL_GetTicks() - startTime)) * 1000;

        // m_world->update(static_cast<float>(delta));
        // m_world->render();

        //rendering always before this

        handleEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();

//        SpriteSheetManager::instance()->renderCharacters();
//        drawDebugText();

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
