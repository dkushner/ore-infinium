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

#include "debug.h"

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

    glDeleteTextures(1, &m_textureID);
    FreeImage_Unload(m_bitmap);
}

void Image::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Image::generate()
{
    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //FIXME: do i want this behavior?    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    BYTE* bits = FreeImage_GetBits(m_bitmap);
    Debug::assertf(bits, "Image::generate, could not gen texture, image bits are empty.");

    glTexImage2D(GL_TEXTURE_2D, m_level, m_internal_format, m_width, m_height, m_border, m_image_format, GL_UNSIGNED_BYTE, bits);
}


GLuint Image::textureHandle()
{
    return m_textureID;
}

unsigned int Image::width() const
{
    return m_width;
}

unsigned int Image::height() const
{
    return m_height;
}

void Image::loadImage(const std::string& filename, GLenum image_format, GLint internal_format, GLint level, GLint border)
{
    FREE_IMAGE_FORMAT imageFormat = FIF_UNKNOWN;

    imageFormat = FreeImage_GetFileType(filename.c_str());

    //if still unknown, try to guess the file format from the file extension
    if (imageFormat == FIF_UNKNOWN) {
        imageFormat = FreeImage_GetFIFFromFilename(filename.c_str());
    }

    Debug::fatal(imageFormat != FIF_UNKNOWN, Debug::Area::Graphics, "failure to load image, type unknown");

    //check that the plugin has reading capabilities for this file and load the file
    if (FreeImage_FIFSupportsReading(imageFormat)) {
        m_bitmap = FreeImage_Load(imageFormat, filename.c_str());
    }

    Debug::fatal(m_bitmap, Debug::Area::Graphics, "failure to load image, bitmap pointer invalid");


    m_width = FreeImage_GetWidth(m_bitmap);
    m_height = FreeImage_GetHeight(m_bitmap);

    if (m_width == 0 || m_height == 0) {
        Debug::fatal(false, Debug::Area::Graphics, "failure to load image, bitmap sizes invalid or bits invalid");
    }
}
