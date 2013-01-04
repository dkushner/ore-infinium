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

#include "texture.h"
#include "texturemanager.h"
#include "game.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

Texture::Texture(std::string fileName)
{
    Texture::setTexture(fileName);

    const glm::vec2 texSize = size();
    m_origin = glm::vec2(texSize.x() * 0.5, texSize.y() * 0.5);
}

void Texture::setTexture(std::string fileName)
{
    TextureManager::instance()->loadTexture(fileName, &m_textureID);
}

void Texture::render()
{
//    al_draw_bitmap(m_bitmap, m_position.x() - m_origin.x(), m_position.y() - m_origin.y(), flags);

    if (DEBUG_RENDERING) {
        renderDebugDrawing();
    }
}

void Texture::renderDebugDrawing()
{
    //ALLEGRO_COLOR color = al_map_rgb(0, 255, 0);
    //(m_position.x() - m_origin.x(), m_position.y() - m_origin.y(), m_position.x() + size().x() - m_origin.x(), m_position.y() + size().y() - m_origin.y(), color, 1.0f);
}

glm::vec2 Texture::size() const
{
    //FIXME:
    return glm::vec2(0, 0);
}

void Texture::setOrigin(const glm::vec2 origin)
{
    m_origin = origin;
}
