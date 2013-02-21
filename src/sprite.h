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

#ifndef SPRITE_H
#define SPRITE_H

#include "spritesheetrenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl.h>

#include <string>

class Sprite
{
public:

    /**
     * Creates a sprite with the requested framename (required)
     * and with the category (also required)
     * @see setOrigin origin
     */
    explicit Sprite(const std::string& frameName, SpriteSheetRenderer::SpriteSheetType spriteSheetType);

    /**
     * Sets the name of the sprite's frame to use.
     * Used for animation.
     */
    void setFrameName(const std::string& frameName) {
        m_frameName = frameName;
    }
    std::string frameName() {
        return m_frameName;
    }

    /// TODO: NOT IMPLEMENTED OR NEEDED RIGHT NOW.
    /// THIS MAY REQUIRE SOME CHANGES ON THE MANAGER FRONT, TO GET IT TO CHANGE THE INTERNAL MAPPING
    /// THIS WOULD ONLY EVER BE NEEDED TO e.g. MAKE AN ENTITY SUDDENLY LOOK LIKE A CHARACTER, aka a mimic/illusionist
    /// enemy
//    void setSpriteSheetType(SpriteSheetManager::SpriteSheetType spriteSheetType);

    /**
     * Returns the width and height of the bitmap of this Texture
     */
    glm::vec2 size() const {
        return m_size;
    }

    /**
     * Sets the origin of this texture, to which to offset drawing.
     * Default is the center of the texture.
     */
    void setOrigin(const glm::vec2& origin) {
        m_origin = origin;
    }

    glm::vec2 origin() const {
        return m_origin;
    }

    void setPosition(const glm::vec2& vector) {
        m_position = vector;
    }
    void setPosition(float x, float y) {
        m_position = glm::vec2(x, y);
    }

    glm::vec2 position() const {
        return m_position;
    }

private:
    Sprite() {};

private:
    SpriteSheetRenderer::SpriteSheetType m_spriteSheetType;

    std::string m_frameName;

    glm::vec2 m_size;
    glm::vec2 m_position;
    glm::vec2 m_origin;

    friend SpriteSheetRenderer;
};

#endif
