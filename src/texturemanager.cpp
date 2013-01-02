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

#include "texturemanager.h"
#include "debug.h"

TextureManager* TextureManager::s_instance(0);

TextureManager* TextureManager::instance()
{
    if(!s_instance)
        s_instance = new TextureManager();

    return s_instance;
}

TextureManager::TextureManager()
{
    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
#endif
}

TextureManager::~TextureManager()
{
    // call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise();
#endif

    unloadAllTextures();
    s_instance = 0;
}

void TextureManager::loadTexture(std::string filename, const unsigned int texID, GLenum image_format, GLint internal_format, GLint level, GLint border)
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

    //if this somehow one of these failed (they shouldn't), return failure
    if((bits == 0) || (width == 0) || (height == 0)) {
        Debug::fatal(false, Debug::Area::Graphics, "failure to load font, bitmap sizes invalid or bits invalid");
    }

    //if this texture ID is in use, unload the current texture
    if(m_texID.find(texID) != m_texID.end()) {
        glDeleteTextures(1, &(m_texID[texID]));
    }

    //generate an OpenGL texture ID for this texture
    glGenTextures(1, &gl_texID);

    //store the texture ID mapping
    m_texID[texID] = gl_texID;

    //bind to the new texture ID
    glBindTexture(GL_TEXTURE_2D, gl_texID);

    //store the texture data for OpenGL use
    glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height, border, image_format, GL_UNSIGNED_BYTE, bits);

    //Free FreeImage's copy of the data
    FreeImage_Unload(bitmap);
}

bool TextureManager::unloadTexture(const unsigned int texID)
{
    bool result(true);
    //if this texture ID mapped, unload it's texture, and remove it from the map
    if(m_texID.find(texID) != m_texID.end()) {
        glDeleteTextures(1, &(m_texID[texID]));
        m_texID.erase(texID);
    } else {
        result = false;
    }

    return result;
}

bool TextureManager::bindTexture(const unsigned int texID)
{
    bool result(true);

    //if this texture ID mapped, bind it's texture as current
    if(m_texID.find(texID) != m_texID.end()) {
        glBindTexture(GL_TEXTURE_2D, m_texID[texID]);
    } else {
        Debug::fatal(false, Debug::Area::Graphics, "bind texture attempted on a nonloaded textureID");
    }

    return result;
}

void TextureManager::unloadAllTextures()
{
    std::map<unsigned int, GLuint>::iterator i = m_texID.begin();

    while(i != m_texID.end()) {
        unloadTexture(i->first);
    }

    m_texID.clear();
}
