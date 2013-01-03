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

#include "camera.h"
#include "game.h"

Camera::Camera(GLint _spriteShaderProgram) :
m_vector(glm::vec3()),
m_spriteShaderProgram(_spriteShaderProgram)
{
    float x = 0.0f;
    float y = 0.0f;
    m_viewMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, 0.0f));
}

void Camera::pushViewMatrix()
{
    glUseProgram(m_spriteShaderProgram);

    // Get the location of our view matrix in the shader
    int viewMatrixLocation = glGetUniformLocation(m_spriteShaderProgram, "viewMatrix");

    // Send our view matrix to the shader
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &m_viewMatrix[0][0]);
}
