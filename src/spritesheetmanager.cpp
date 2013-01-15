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
#include "camera.h"
#include "shader.h"
#include "image.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

SpriteSheetManager* SpriteSheetManager::s_instance(0);

SpriteSheetManager* SpriteSheetManager::instance()
{
    if (!s_instance)
        s_instance = new SpriteSheetManager();

    return s_instance;
}

SpriteSheetManager::SpriteSheetManager()
{
    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
#endif

    m_shader = new Shader("sprite.vert", "sprite.frag");

    initGL();

    float scale = 1.0f;
    m_modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    m_projectionMatrix = glm::ortho(0.0f, float(SCREEN_W), float(SCREEN_H), 0.0f, -1.0f, 1.0f);

    loadAllSpriteSheets();
    parseAllSpriteSheets();
}

SpriteSheetManager::~SpriteSheetManager()
{
    unloadAllSpriteSheets();

    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);

    glDeleteVertexArrays(1, &m_vao);

    s_instance = 0;
}

void SpriteSheetManager::setCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->setShader(m_shader);
}

void SpriteSheetManager::loadAllSpriteSheets()
{
    loadSpriteSheet("../textures/characters.png", SpriteSheetType::Character);
}

void SpriteSheetManager::loadSpriteSheet(const std::string& fileName, SpriteSheetManager::SpriteSheetType type)
{
    auto& wrapper = m_spriteSheetTextures[type];
    wrapper.image = new Image(fileName);
    wrapper.image->generate();
}

void SpriteSheetManager::unloadSpriteSheet(SpriteSheetManager::SpriteSheetType type)
{
    delete m_spriteSheetTextures.at(type).image;
    m_spriteSheetTextures.erase(type);
}

void SpriteSheetManager::unloadAllSpriteSheets()
{
    std::map<SpriteSheetManager::SpriteSheetType, SpriteSheet>::const_iterator i = m_spriteSheetTextures.begin();

    while (i != m_spriteSheetTextures.end()) {
        unloadSpriteSheet(i->first);
    }

    m_spriteSheetTextures.clear();
}

void SpriteSheetManager::bindSpriteSheet(SpriteSheetManager::SpriteSheetType type)
{
    m_spriteSheetTextures[type].image->bind();
}

glm::vec2 SpriteSheetManager::spriteSheetSize(SpriteSheetManager::SpriteSheetType type)
{
    auto texture = m_spriteSheetTextures.find(type);
    glm::vec2 imageSize(float(texture->second.image->width()), float(texture->second.image->height()));

    return imageSize;
}

void SpriteSheetManager::registerSprite(SpriteSheetManager::SpriteSheetType spriteSheetType, Sprite* sprite)
{
    switch (spriteSheetType) {
    case SpriteSheetType::Character:
        m_characterSprites.insert(m_characterSprites.end(), sprite);
        // TODO: look up the size of the graphic/frame, in the spritesheet map.
        sprite->m_size = glm::vec2(100.0f, 100.0f);
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
    m_shader->bindProgram();

    bindSpriteSheet(SpriteSheetType::Character);

    int index = 0;
    for (Sprite * sprite: m_characterSprites) {
        auto frameIdentifier = m_spriteSheetCharactersDescription.find(sprite->frameName());
        SpriteFrameIdentifier& frame = frameIdentifier->second;
        frame.x; //FIXME:

        // vertices that will be uploaded.
        spriteVertex vertices[4];

        // vertices[n][0] -> X, and [1] -> Y
        // vertices[0] -> top left
        // vertices[1] -> bottom left
        // vertices[2] -> bottom right
        // vertices[3] -> top right

        glm::vec2 spritePosition = sprite->position();

        glm::vec2 spriteSize = sprite->size();

        glm::vec4 rect = glm::vec4(spritePosition.x, spritePosition.y, spritePosition.x + spriteSize.x, spritePosition.y + spriteSize.y);

        float x = rect.x;
        float width = rect.z;

        float y = rect.y;
        float height = rect.w;

        vertices[0][0] = x; // top left X
        vertices[0][1] = y; //top left Y

        vertices[1][0] = x; // bottom left X
        vertices[1][1] = height; // bottom left Y

        vertices[2][0] = width; // bottom right X
        vertices[2][1] = height; //bottom right Y

        vertices[3][0] = width; // top right X
        vertices[3][1] = y; // top right Y

        checkGLError();

        // copy color to the buffer
        for (size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); i++) {
            uint32_t* colorp = reinterpret_cast<uint32_t*>(&vertices[i][2]);
            //        *colorp = color.bgra;
            uint8_t red = 255;
            uint8_t blue = 255;
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
            sizeof(vertices) * index,
            sizeof(vertices),
            vertices);

        ++index;
    }

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);

    checkGLError();
    m_shader->bindProgram();

    checkGLError();
    glDrawElements(
        GL_TRIANGLES,
        6 * (m_characterSprites.size()), // 6 indices per 2 triangles
        GL_UNSIGNED_INT,
        (const GLvoid*)0);

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    checkGLError();
}

void SpriteSheetManager::renderEntities()
{

}

void SpriteSheetManager::checkGLError()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << gluErrorString(error);
        assert(0);
    }
}

GLuint tex;
void SpriteSheetManager::initGL()
{
    checkGLError();

    //////////////////////

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxSpriteCount * 4 * sizeof(spriteVertex),
        NULL,
        GL_DYNAMIC_DRAW);

    checkGLError();

    std::vector<u32> indicesv;

    // prepare and upload indices as a one time deal
    const u32 indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (size_t j = 0; j < m_maxSpriteCount; j++) {
        for (size_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(u32),
        indicesv.data(),
        GL_STATIC_DRAW);

    checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(spriteVertex),
        (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(f32) * 2;

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(spriteVertex),
        (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(u32);

    checkGLError();

    GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(spriteVertex),
        (const GLvoid*)buffer_offset);

    checkGLError();
}
