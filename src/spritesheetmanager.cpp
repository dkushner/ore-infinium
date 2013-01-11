/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *   Based on original code from Ben English <benjamin.english@oit.edu>       *
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

#include "spritesheetmanager.h"

#include "sprite.h"
#include "debug.h"
#include "game.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


SpriteSheetManager* SpriteSheetManager::s_instance(0);

SpriteSheetManager* SpriteSheetManager::instance()
{
    if(!s_instance)
        s_instance = new SpriteSheetManager();

    return s_instance;
}

SpriteSheetManager::SpriteSheetManager()
{
    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
#endif

    initGL();

    float scale = 1.0f;
    m_modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    m_projectionMatrix = glm::ortho(0.0f, float(SCREEN_W), float(SCREEN_H), 0.0f, -1.0f, 1.0f);

    glUseProgram(m_spriteShaderProgram);
//
//    // Get the location of our projection matrix in the shader
//    int projectionMatrixLocation = glGetUniformLocation(m_spriteShaderProgram, "projectionMatrix");
//
//    // Get the location of our model matrix in the shader
//    int modelMatrixLocation = glGetUniformLocation(m_spriteShaderProgram, "modelMatrix");
//
//    // Send our projection matrix to the shader
//    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &m_projectionMatrix[0][0]);
//
//    // Send our model matrix to the shader
//    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &m_modelMatrix[0][0]);
//
    loadAllSpriteSheets();
    parseAllSpriteSheets();

//FIXME    glEnable(GL_BLEND);
//FIXME glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

SpriteSheetManager::~SpriteSheetManager()
{
    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise();
#endif

    unloadAllSpriteSheets();

    glDeleteProgram(m_spriteShaderProgram);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);

    glDeleteVertexArrays(1, &m_vao);

    s_instance = 0;
}

void SpriteSheetManager::loadAllSpriteSheets()
{
    loadSpriteSheet("../textures/characters.png", SpriteSheetType::Character);
}

void SpriteSheetManager::loadSpriteSheet(const std::string& filename, SpriteSheetType type, GLenum image_format, GLint internal_format, GLint level, GLint border)
{
    //image format
    FREE_IMAGE_FORMAT imageFormat = FIF_UNKNOWN;
    //pointer to the image, once loaded
    FIBITMAP *bitmap(0);
    //pointer to the image data
    BYTE* bits(0);
    //image width and height
    unsigned int width(0), height(0);
    //OpenGL's image ID to map to
    GLuint gl_texID;

    //check the file signature and deduce its format
    imageFormat = FreeImage_GetFileType(filename.c_str(), 0);

    //if still unknown, try to guess the file format from the file extension
    if(imageFormat == FIF_UNKNOWN) {
        imageFormat = FreeImage_GetFIFFromFilename(filename.c_str());
    }

    Debug::fatal(imageFormat != FIF_UNKNOWN, Debug::Area::Graphics, "failure to load font, type unknown");

    //check that the plugin has reading capabilities and load the file
    if(FreeImage_FIFSupportsReading(imageFormat)) {
        bitmap = FreeImage_Load(imageFormat, filename.c_str());
    }

    Debug::fatal(bitmap, Debug::Area::Graphics, "failure to load font, bitmap pointer invalid");

    //retrieve the image data
    bits = FreeImage_GetBits(bitmap);
    //get the image width and height
    width = FreeImage_GetWidth(bitmap);
    height = FreeImage_GetHeight(bitmap);

    //if somehow one of these failed (they shouldn't), return failure
    if((bits == 0) || (width == 0) || (height == 0)) {
        Debug::fatal(false, Debug::Area::Graphics, "failure to load font, bitmap sizes invalid or bits invalid");
    }

    //generate an OpenGL texture ID for this texture
    glGenTextures(1, &gl_texID);

    auto& wrapper = m_spriteSheetTextures[type];
    wrapper.textureID = gl_texID;

    //bind to the new texture ID
    glBindTexture(GL_TEXTURE_2D, gl_texID);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//FIXME: do i want this behavior?    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height, border, image_format, GL_UNSIGNED_BYTE, bits);

    //Free FreeImage's copy of the data
    FreeImage_Unload(bitmap);
}

void SpriteSheetManager::unloadSpriteSheet(SpriteSheetManager::SpriteSheetType type)
{
    glDeleteTextures(1, &(m_spriteSheetTextures[type].textureID));
    m_spriteSheetTextures.erase(type);
}

void SpriteSheetManager::unloadAllSpriteSheets()
{
    std::map<SpriteSheetManager::SpriteSheetType, SpriteSheet>::const_iterator i = m_spriteSheetTextures.begin();

    while(i != m_spriteSheetTextures.end()) {
        unloadSpriteSheet(i->first);
    }

    m_spriteSheetTextures.clear();
}

void SpriteSheetManager::bindSpriteSheet(SpriteSheetManager::SpriteSheetType type)
{
    glBindTexture(GL_TEXTURE_2D, m_spriteSheetTextures[type].textureID);
}

glm::vec2 SpriteSheetManager::spriteSheetSize(SpriteSheetManager::SpriteSheetType type)
{
    auto texture = m_spriteSheetTextures.find(type);
    glm::vec2 imageSize(float(texture->second.width), float(texture->second.height));

    return imageSize;
}

void SpriteSheetManager::registerSprite(SpriteSheetManager::SpriteSheetType spriteSheetType, Sprite* sprite)
{
    switch (spriteSheetType) {
    case SpriteSheetType::Character:
        m_characterSprites.insert(m_characterSprites.end(), sprite);
        Debug::log(Debug::Area::Graphics) << "sprite registered, new sprite count: " << m_characterSprites.size();
        break;

    case SpriteSheetType::Entity:
        break;
    }
}

void SpriteSheetManager::parseAllSpriteSheets()
{
    m_spriteSheetCharactersDescription = parseSpriteSheet("../textures/characters.yaml");
}

std::map<std::string, SpriteSheetManager::SpriteFrameIdentifier> SpriteSheetManager::parseSpriteSheet(const std::string& filename)
{
    std::map<std::string, SpriteFrameIdentifier> descriptionMap;

    //FIXME hardcoded to "load" 1 character for now.
    SpriteFrameIdentifier frame;
    frame.x = 0;
    frame.y = 0;
    frame.width = 80;
    frame.height = 80;
    descriptionMap["player_frame1"] = frame;

    // load the filename (yaml file), populate the map with the information and return it.
    return descriptionMap;
}

void SpriteSheetManager::renderCharacters()
{
    glUseProgram(m_spriteShaderProgram);
    bindSpriteSheet(SpriteSheetType::Character);

//FIXME    glUniform1i(m_texture_location, 0);

//FIXME:    glBindVertexArray(m_vao);

    // Get the location of our model matrix in the shader
//FIXME:   int modelMatrixLocation = glGetUniformLocation(m_spriteShaderProgram, "modelMatrix");

    //FIXME:iformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &m_modelMatrix[0][0]);

for (Sprite* sprite: m_characterSprites) {
        auto frameIdentifier = m_spriteSheetCharactersDescription.find(sprite->frameName());
        SpriteFrameIdentifier& frame = frameIdentifier->second;
        frame.x; //FIXME:

        // use the shader program
        glUseProgram(m_spriteShaderProgram);

        // bind the vao
        glBindVertexArray(m_vao);

        // draw
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glUseProgram(0);
        glBindVertexArray(0);

    }

    glUseProgram(0);
    checkGLError();
}

void SpriteSheetManager::renderEntitites()
{

}

void SpriteSheetManager::checkGLError()
{
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cerr << gluErrorString(error);
        assert(0);
    }
}

void SpriteSheetManager::printShaderInfoLog(GLuint shader)
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
//        std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
        std::cout << infoLog;
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
char* SpriteSheetManager::loadFile(const char* fname, GLint* fSize)
{
    std::ifstream::pos_type size;
    char * memblock = 0;
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

void SpriteSheetManager::loadDefaultShaders()
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

    m_vertexShader = vertex_shader;
    m_fragmentShader = fragment_shader;

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

bool SpriteSheetManager::checkShaderCompileStatus(GLuint obj)
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

bool SpriteSheetManager::checkProgramLinkStatus(GLuint obj)
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

void SpriteSheetManager::initGL()
{
    loadDefaultShaders();


    // bind the attribute locations (inputs)
    GLint posLoc = glGetAttribLocation(m_spriteShaderProgram, "vposition");
    GLint colorLoc = glGetAttribLocation(m_spriteShaderProgram, "vcolor");

    // bind the FragDataLocation (output)
    //FIXME: not needed?
//    glBindFragDataLocation(m_spriteShaderProgram, 0, "FragColor");

    // generate and bind the vao
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // generate and bind the buffer object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // data for a fullscreen quad
    GLfloat vertexData[] = {
        //  X     Y     Z           R     G     B         U  V
        1.0f, 1.0f, 0.0f,       1.0f, 0.0f, 0.0f,     0.0f, 0.0f,  // vertex 0
        -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,      0.0f, 0.0f, // vertex 1
        1.0f,-1.0f, 0.0f,       0.0f, 0.0f, 1.0f,      0.0f, 0.0f, // vertex 2
        1.0f,-1.0f, 0.0f,       0.0f, 0.0f, 1.0f,      0.0f, 0.0f, // vertex 3
        -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,      0.0f, 0.0f, // vertex 4
        -1.0f,-1.0f, 0.0f,       1.0f, 0.0f, 0.0f,      0.0f, 0.0f, // vertex 5
    }; // 6 vertices with 6 components (floats) each

    // fill with data
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8*6, vertexData, GL_STATIC_DRAW);

    // set up generic attrib pointers
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));

    // "unbind" vao
    glBindVertexArray(0);
    checkGLError();
}
