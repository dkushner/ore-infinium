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

#include "texture.h"
#include "image.h"

#include "debug.h"

#include <FreeImage.h>

Texture::Texture(const std::string& fileName)
{
    loadImage(fileName);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
    delete m_image;
}

void Texture::loadImage(const std::string& filename)
{
    m_image = new Image(filename);
    m_height = m_image->height();
    m_width = m_image->width();
}

void Texture::bind()
{
    Debug::assertf(m_textureID, "bind called before generate on Image");

    Debug::checkGLError();
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    Debug::checkGLError();
}

void Texture::generate()
{
    Debug::checkGLError();
    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //FIXME: do i want this behavior?    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    BYTE* bits = m_image->bytes();
    Debug::assertf(bits, "Image::generate, could not gen texture, image bits are empty.");
    //GLenum image_format = GL_BGRA, GLint internal_format = GL_RGBA, GLint level = 0, GLint border = 0
    m_internal_format = GL_RGBA;
    m_image_format = GL_BGRA;
    m_level = 0;
    m_border = 0;
    glTexImage2D(GL_TEXTURE_2D, m_level, m_internal_format, m_width, m_height, m_border, m_image_format, GL_UNSIGNED_BYTE, bits);

    Debug::checkGLError();
}

GLuint Texture::textureHandle()
{
    return m_textureID;
}

unsigned int Texture::width() const
{
    return m_width;
}

unsigned int Texture::height() const
{
    return m_height;
}
