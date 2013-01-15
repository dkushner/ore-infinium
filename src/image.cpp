/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
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

#include "image.h"

#include <FreeImage.h>

Image::Image(const std::string& fileName)
{
    loadImage(fileName);
}

Image::~Image()
{
    // call this ONLY when linking with FreeImage as a static library
    #ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise();
    #endif
}

void Image::loadSpriteSheet(const std::string& filename, GLenum image_format, GLint internal_format, GLint level, GLint border)
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
    if (imageFormat == FIF_UNKNOWN) {
        imageFormat = FreeImage_GetFIFFromFilename(filename.c_str());
    }

    Debug::fatal(imageFormat != FIF_UNKNOWN, Debug::Area::Graphics, "failure to load font, type unknown");

    //check that the plugin has reading capabilities and load the file
    if (FreeImage_FIFSupportsReading(imageFormat)) {
        bitmap = FreeImage_Load(imageFormat, filename.c_str());
    }

    Debug::fatal(bitmap, Debug::Area::Graphics, "failure to load font, bitmap pointer invalid");

    //retrieve the image data
    bits = FreeImage_GetBits(bitmap);
    //get the image width and height
    width = FreeImage_GetWidth(bitmap);
    height = FreeImage_GetHeight(bitmap);

    //if somehow one of these failed (they shouldn't), return failure
    if ((bits == 0) || (width == 0) || (height == 0)) {
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
