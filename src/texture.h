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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "resourcemanager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Texture
{
public:

    /**
     * A wrapper around ALLEGRO_BITMAP which automagically handles loading and
     * using the resourcemanager.
     * The default origin is the center of the texture.
     * @see setOrigin origin
     */
    Texture(const char* texture);

    /**
     * Pass a string into @p texture and it will automatically ask the ResourceManager
     * for an already-loaded version, if possible. If not, it will load it.
     *
     */
    void setTexture(const char* texture);

    /**
     * Returns the width and height of the bitmap of this Texture, as a Vector2i.
     */
    Eigen::Vector2i size() const;

    /**
     * Sets the origin of this texture, to which to offset drawing.
     * Default is the center of the texture.
     */
    void setOrigin(const Eigen::Vector2f origin);

    glm::mat4 origin() const { return m_origin; };

    glm::mat4 position() const { return m_position; };
    void setPosition(const Eigen::Vector2f& vector) { m_position = vector; };
    void setPosition(float x, float y) { m_position = Eigen::Vector2f(x, y); };

private:
    Texture();

    void renderDebugDrawing();

private:
    Eigen::Vector2f m_position;
    Eigen::Vector2f m_origin;
};

#endif
