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
}

SpriteSheetManager::~SpriteSheetManager()
{
    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise();
#endif

    unloadAllSpriteSheets();
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

    //store the texture data for OpenGL use
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
    for (Sprite* sprite: m_characterSprites) {
        auto frameIdentifier = m_spriteSheetCharactersDescription.find(sprite->frameName());
        SpriteFrameIdentifier& frame = frameIdentifier->second;
        frame.x; //FIXME:
    }
}

void SpriteSheetManager::renderEntitites()
{

}

