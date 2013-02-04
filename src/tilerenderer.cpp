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

#include "tilerenderer.h"
#include "world.h"
#include "shader.h"

TileRenderer::TileRenderer(World* world)
    : m_world(world)
{
    m_shader = new Shader("tilerenderer.vert", "tilerenderer.frag");

    initGL();
}

TileRenderer::~TileRenderer()
{
    delete m_shader;
}

void TileRenderer::initGL()
{

}

void TileRenderer::render()
{
    m_shader->bindProgram();


    m_shader->unbindProgram();
}
