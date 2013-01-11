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

//    glUseProgram(m_spriteShaderProgram);
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
    glBindTexture(GL_TEXTURE_2D, gl_texID);

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
//    glBindTexture(GL_TEXTURE_2D, m_spriteSheetTextures[type].textureID);
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
        
        spriteVertex vertices[4];
        // transform vertices and copy them to the buffer
        vertices[0][0] = vertices[0][1] = vertices[1][0] = vertices[3][1] = 0;
        vertices[1][1] = float(0.0f);
        vertices[2][0] = float(1.0f);
        vertices[2][1] = float(0.0f);
        vertices[3][0] = float(1.0f);

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
        vertices[0][3] = vertices[1][3] = 0.0f;
        vertices[0][4] = vertices[3][4] = 1.0f;
        vertices[1][4] = vertices[2][4] = 0.0f;
        vertices[2][3] = vertices[3][3] = 1.0f;
        
        // finally upload everything to the actual vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(
            GL_ARRAY_BUFFER,
           m_characterSprites.size() /*batch_size_*/ * sizeof(vertices),
                        sizeof(vertices),
                        vertices);
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
//        glEnable(GL_BLEND);
 //////       glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindVertexArray(m_vao);
        
        glUseProgram(m_spriteShaderProgram);
        
    
            glDrawElements(
                GL_POINTS,
                6* 1, //(end - start), // 6 indices per 2 triangles
                   GL_UNSIGNED_INT,
                   (const GLvoid*)(6* 1/*start*/ * sizeof(uint32_t)));
                
        
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        

}
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
//    loadDefaultShaders();
    //FIXME
    assert(sizeof(float) == sizeof(uint32_t));

    GLuint new_vao = 0;
    GLuint new_vbo = 0;
    GLuint new_ebo = 0;
    glGenVertexArrays(1, &new_vao);
    glBindVertexArray(new_vao);
    
    glGenBuffers(1,&new_vbo);
    glBindBuffer(GL_ARRAY_BUFFER,new_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxSpriteCount * 4 * sizeof(spriteVertex),
                 NULL,
                 GL_DYNAMIC_DRAW);
    
    GLenum err = glGetError();
    if (err)
    {
        
        glDeleteVertexArrays(1,&new_vao);
        glDeleteBuffers(1,&new_vbo);
        assert(0);
    }
    
    std::vector<uint32_t> indicesv;
    
    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (size_t j = 0; j < m_maxSpriteCount; j++)
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
        indicesv.size()*sizeof(uint32_t),
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
    
    GLuint new_vs;
    GLuint new_fs;
    GLuint new_sp;
    
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
//        "vec4 c = vec4(1.0, 1.0, 1.0, 1.0);"
        "    gl_FragColor = frag_color * texture2D(sampler,frag_texcoord) ;"
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
        assert(0);
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
            std::cout
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
            std::cout
            << "Failed to validate SpriteBatch shader program."
            << std::endl;
            
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
        buffer_offset += sizeof(float) * 2;

        GLint color_attrib = glGetAttribLocation(new_sp, "color");

        glEnableVertexAttribArray(color_attrib);
        glVertexAttribPointer(
            color_attrib,
            4,
            GL_UNSIGNED_BYTE,
            GL_TRUE,
            sizeof(spriteVertex),
                              (const GLvoid*)buffer_offset);
        buffer_offset += sizeof(uint32_t);
        
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
        
        m_vao = new_vao;
        m_vbo = new_vbo;
        m_ebo = new_ebo;
        m_spriteShaderProgram = new_sp;
        m_fragmentShader = new_fs;
        m_vertexShader = new_vs;

    checkGLError();
}
