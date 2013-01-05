/******************************************************************************
 *   Copyright (C) 2012, 2013 by Shaun Reich <sreich@kde.org>                 *
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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl.h>

#include <string>

class Texture
{
public:

    /**
     * A wrapper around an OpenGL texture.
     * Automatically uses the texturemanager.
     * The default origin is the center of the texture.
     * @see setOrigin origin
     */
    Texture(std::string fileName);

    /**
     * Pass a string into @p texture and it will automatically ask the TextureManager
     * for an already-loaded version, if possible. If not, it will load it.
     *
     */
    void setTexture(std::string fileName);

    /**
     * Returns the width and height of the bitmap of this Texture, as a Vector2i.
     */
    glm::vec2 size() const;

    /**
     * Sets the origin of this texture, to which to offset drawing.
     * Default is the center of the texture.
     */
    void setOrigin(const glm::vec2& origin);

    glm::mat4 origin() const { return m_origin; };

    glm::mat4 position() const { return m_position; };

    void setPosition(const glm::vec2& vector) { m_position = vector; };
    void setPosition(float x, float y) { m_position = glm::vec2(x, y); };

private:
    Texture();

    void renderDebugDrawing();

private:
    GLint m_textureID;

    glm::vec2 m_position;
    glm::vec2 m_origin;
};

#endif
