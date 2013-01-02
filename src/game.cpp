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

void checkSDLError()
{
    std::string error = SDL_GetError();
    if (*error.c_str() != '\0')
    {
        Debug::log(Debug::Area::System) << "SDL Error: " << error;
        SDL_ClearError();
    }
}



GLuint shaderProgram;
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

    /* Request opengl 3.2 context.
     * SDL doesn't have the ability to choose which profile at this time of writing,
     * but it should default to the core profile */
    //FIXME: i *want 3.2, but Mesa 9 only has 3.0.. :(
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //TODO: we'll probably need some extension at some point in time..
    //SDL_GL_ExtensionSupported();

    m_context = SDL_GL_CreateContext(m_window);
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }
    checkSDLError();
    glewInit();

    Debug::log(Debug::Area::Graphics) << "Platform: Driver Vendor: " << glGetString(GL_VENDOR);
    Debug::log(Debug::Area::Graphics) << "Platform: Renderer: " << glGetString(GL_RENDERER);
    Debug::log(Debug::Area::Graphics) << "OpenGL Version: " << glGetString(GL_VERSION);
    Debug::log(Debug::Area::Graphics) << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
     error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }
    GLint textureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
    Debug::log(Debug::Area::Graphics) << "Maximum OpenGL texture size allowed: " << textureSize;
    std::cout << "\n\n\n\n";

    m_font = new FTGLPixmapFont("../font/Ubuntu-L.ttf");
    Debug::fatal(!m_font->Error(), Debug::Area::System, "Failure to load font");
     error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);

    glViewport(0, 0, SCREEN_W, SCREEN_H);
    error = glGetError();

    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }

    initGL();

    float scale = 1.0f;
    modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale));

    float x = 0.0f;
    float y = 0.0f;
    viewMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, 0.0f));

    projectionMatrix = glm::ortho(0.0f, float(SCREEN_W), float(SCREEN_H), 0.0f, -1.0f, 1.0f);


    error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }

    glUseProgram(shaderProgram);

    // Get the location of our projection matrix in the shader
    int projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
       error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }
    // Get the location of our view matrix in the shader
    int viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");

    // Get the location of our model matrix in the shader
    int modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
      error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }

    // Send our projection matrix to the shader
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
         error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }

    // Send our view matrix to the shader  
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
         error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }

    // Send our model matrix to the shader  
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
     error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    ImageManager* manager = ImageManager::instance();
//    manager->addResourceDir("../textures/");

    //World::createInstance(m_display);
    //m_world = World::instance();
    m_font->FaceSize(12);

//    loadDefaultShaders();

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
char* Game::loadFile(char *fname, GLint &fSize)
{
    std::ifstream::pos_type size;
    char * memblock;
    std::string text;

    // file read based on example in cplusplus.com tutorial
    std::ifstream file (fname, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        fSize = (GLuint) size;
        memblock = new char [size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();
        std::cout << "file " << fname << " loaded" << std::endl;
        text.assign(memblock);
    }
    else
    {
        std::cout << "Unable to open file " << fname << std::endl;
        exit(1);
    }
    return memblock;
}

void Game::loadDefaultShaders()
{
    /*    GLuint p, f, v;

        char *vs,*fs;

        v = glCreateShader(GL_VERTEX_SHADER);
        f = glCreateShader(GL_FRAGMENT_SHADER);

        // load shaders & get length of each
        GLint vlen;
        GLint flen;
        vs = loadFile("sprite.vert",vlen);
        fs = loadFile("sprite.frag",flen);

        const char * vv = vs;
        const char * ff = fs;

        glShaderSource(v, 1, &vv,&vlen);
        glShaderSource(f, 1, &ff,&flen);

        GLint compiled;

        glCompileShader(v);
        glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            std::cout << "Vertex shader not compiled." << std::endl;
            printShaderInfoLog(v);
        }

        glCompileShader(f);
        glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            std::cout << "Fragment shader not compiled." << std::endl;
            printShaderInfoLog(f);
        }

        p = glCreateProgram();

        glBindAttribLocation(p, 0, "in_position");
        glBindAttribLocation(p, 1, "in_color");

        glAttachShader(p,v);
        glAttachShader(p,f);

        glLinkProgram(p);
        glUseProgram(p);
        shaderProgram = p;

        delete [] vs; // dont forget to free allocated memory
        delete [] fs; // we allocated this in the loadFile function...
        */


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

// helper to check and display for shader compiler errors
bool check_shader_compile_status(GLuint obj)
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

// helper to check and display for shader linker error
bool check_program_link_status(GLuint obj)
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
GLuint texture;

void Game::initGL()
{
    // program and shader handles
    GLuint vertex_shader, fragment_shader;

    // we need these to properly pass the strings
    const char *source;
    int length;

    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = vertex_source.c_str();
    length = vertex_source.size();
    glShaderSource(vertex_shader, 1, &source, &length);
    glCompileShader(vertex_shader);
    if(!check_shader_compile_status(vertex_shader))
    {
        assert(0);
    } else {
        Debug::log(Debug::Area::Graphics) << "vertex shader compiled!";
    }

    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_source.c_str();
    length = fragment_source.size();
    glShaderSource(fragment_shader, 1, &source, &length);
    glCompileShader(fragment_shader);
    if(!check_shader_compile_status(fragment_shader))
    {
        assert(0);
    } else {
        Debug::log(Debug::Area::Graphics) << "fragment shader compiled!";
    }

    // create program
    shaderProgram = glCreateProgram();

    // attach shaders
    glAttachShader(shaderProgram, vertex_shader);
    glAttachShader(shaderProgram, fragment_shader);

    // link the program and check for errors
    glLinkProgram(shaderProgram);
    if (check_program_link_status(shaderProgram)) {
        Debug::log(Debug::Area::Graphics) << "shader program linked!";
    } else {
        Debug::fatal(false, Debug::Area::Graphics, "shader program link FAILURE");
    }

    // get texture uniform location
    texture_location = glGetUniformLocation(shaderProgram, "tex");

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

    /*
        // original
        //  X     Y     Z           U     V
        1.0f, 1.0f, 0.0f,       1.0f, 1.0f, // vertex 0
        -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, // vertex 1
        1.0f,-1.0f, 0.0f,       1.0f, 0.0f, // vertex 2
        -1.0f,-1.0f, 0.0f,       0.0f, 0.0f, // vertex 3
    }; // 4 vertices with 5 components (floats) each
    */

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

    bool loaded = TextureManager::Inst()->LoadTexture("../textures/player.png", TextureID, GL_BGRA_EXT, GL_RGBA);
    assert(loaded);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Game::render()
{
//    glLoadIdentity();
    // use the shader program

    // bind texture to texture unit 0
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, TextureID);
    glUseProgram(shaderProgram);

    TextureManager::Inst()->BindTexture(TextureID);

    // set texture uniform
    glUniform1i(texture_location, 0);

    // bind the vao
    glBindVertexArray(vao);

    // draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUseProgram(0);

    // check for errors
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }
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
