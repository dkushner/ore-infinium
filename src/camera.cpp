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
#include "debug.h"
#include "spritesheetmanager.h"

Camera::Camera() :
    m_vector(glm::vec3())
{
    float x = 0.0f;
    float y = 0.0f;
    m_viewMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, 0.0f));
    m_orthoMatrix = glm::ortho(0.0f, float(SCREEN_W), float(SCREEN_H), 0.0f, -1.0f, 1.0f);
}

void Camera::translate(const glm::vec2 vec)
{
    m_viewMatrix = glm::translate(m_viewMatrix, glm::vec3(vec, 0.0f));
}

void Camera::zoom(const float factor)
{
    m_viewMatrix = glm::scale(m_viewMatrix, glm::vec3(factor));
}

void Camera::centerOn(const glm::vec2 vec)
{
    m_viewMatrix = glm::translate(glm::mat4(), glm::vec3(vec, 0.0f));
}

void Camera::pushMatrix()
{
    glUseProgram(m_shaderProgram);

    glm::mat4 mvp = m_viewMatrix * m_orthoMatrix;

    int mvpLoc = glGetUniformLocation(m_shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    glUseProgram(0);
}

