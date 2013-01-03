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
#include "texturemanager.h"
#include "fontmanager.h"

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

GLuint TextureID = 0;

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

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);

    glViewport(0, 0, SCREEN_W, SCREEN_H);

    checkGLError();

    initGL();

    m_camera = new Camera(m_spriteShaderProgram);

    float scale = 1.0f;
    modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale));

    projectionMatrix = glm::ortho(0.0f, float(SCREEN_W), float(SCREEN_H), 0.0f, -1.0f, 1.0f);

    glUseProgram(m_spriteShaderProgram);

    checkGLError();

    // Get the location of our projection matrix in the shader
    int projectionMatrixLocation = glGetUniformLocation(m_spriteShaderProgram, "projectionMatrix");

    // Get the location of our model matrix in the shader
    int modelMatrixLocation = glGetUniformLocation(m_spriteShaderProgram, "modelMatrix");

    // Send our projection matrix to the shader
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Send our model matrix to the shader
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

    //World::createInstance(m_display);
    //m_world = World::instance();
    m_font->FaceSize(12);

    tick();
    shutdown();
}

void Game::printShaderInfoLog(GLint shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
char* Game::loadFile(const char* fname, GLint* fSize)
{
    std::ifstream::pos_type size;
    char * memblock;
    std::string text;

    // file read based on example in cplusplus.com tutorial
    std::ifstream file (fname, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        *fSize = (GLuint) size;
        memblock = new char [size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();
        Debug::log(Debug::Area::Graphics) << "shader : " << fname << " loaded successfully";
        text.assign(memblock);
    } else {
        Debug::fatal(false,  Debug::Area::Graphics, "failed to load shader: " + std::string(fname));
    }
    return memblock;
}

void Game::loadDefaultShaders()
{
    // program and shader handles
    GLuint vertex_shader, fragment_shader;

    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    GLint vertLength;
    GLint fragLength;

    char* vertSource;
    char* fragSource;

    vertSource = loadFile("sprite.vert", &vertLength);
    fragSource = loadFile("sprite.frag", &fragLength);

    const char* vertSourceConst = vertSource;
    const char* fragSourceConst = fragSource;

    glShaderSource(vertex_shader, 1, &vertSourceConst, &vertLength);
    glCompileShader(vertex_shader);

    if(!checkShaderCompileStatus(vertex_shader)) {
        assert(0);
    } else {
        Debug::log(Debug::Area::Graphics) << "vertex shader compiled!";
    }

    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragSourceConst, &fragLength);
    glCompileShader(fragment_shader);

    if(!checkShaderCompileStatus(fragment_shader)) {
        assert(0);
    } else {
        Debug::log(Debug::Area::Graphics) << "fragment shader compiled!";
    }

    // create program
    m_spriteShaderProgram = glCreateProgram();

    // attach shaders
    glAttachShader(m_spriteShaderProgram, vertex_shader);
    glAttachShader(m_spriteShaderProgram, fragment_shader);

    // link the program and check for errors
    glLinkProgram(m_spriteShaderProgram);

    if (checkProgramLinkStatus(m_spriteShaderProgram)) {
        Debug::log(Debug::Area::Graphics) << "shader program linked!";
    } else {
        Debug::fatal(false, Debug::Area::Graphics, "shader program link FAILURE");
    }

    delete [] vertSource;
    delete [] fragSource;
}

bool Game::checkShaderCompileStatus(GLuint obj)
{
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;
    }
    return true;
}

bool Game::checkProgramLinkStatus(GLuint obj)
{
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;
    }
    return true;
}

GLint texture_location;
GLuint vao;

void Game::initGL()
{
    loadDefaultShaders();

    // get texture uniform location
    texture_location = glGetUniformLocation(m_spriteShaderProgram, "tex");

    // vao and vbo handle
    GLuint vbo, ibo;

    // generate and bind the vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // generate and bind the vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float width = 50.0f;
    float height = 80.0f;

    // data for a fullscreen quad (this time with texture coords)
    GLfloat vertexData[] = {
        //  X     Y     Z           U     V
        width, height, 0.0f,       1.0f, 0.0f, // vertex 0
        0.0f, height, 0.0f,       0.0f, 0.0f, // vertex 1
        width, 0.0f, 0.0f,       1.0f, 1.0f, // vertex 2
        0.0f, 0.0f, 0.0f,       0.0f, 1.0f, // vertex 3
    }; // 4 vertices with 5 components (floats) each

    // fill with data
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*5, vertexData, GL_STATIC_DRAW);

    // set up generic attrib pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));

    // generate and bind the index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    GLuint indexData[] = {
        0,1,2, // first triangle
        2,1,3, // second triangle
    };

    // fill with data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, indexData, GL_STATIC_DRAW);

    // "unbind" vao
    glBindVertexArray(0);

    TextureManager::instance()->loadTexture("../textures/player.png", TextureID);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::render()
{
    glUseProgram(m_spriteShaderProgram);

    TextureManager::instance()->bindTexture(TextureID);

    glUniform1i(texture_location, 0);

    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUseProgram(0);

    checkGLError();
}

double fps = 0.0;

void Game::tick()
{
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;

    while (m_running) {
        fps =(frameCount / float(SDL_GetTicks() - startTime)) * 1000;

        // m_world->update(static_cast<float>(delta));
        // m_world->render();

        //rendering always before this

        handleEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render();
        //render some crap
        drawDebugText();

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
