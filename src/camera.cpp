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
#include "shader.h"
#include "settings/settings.h"

Camera::Camera()
{
    float x = 0.0f;
    float y = 0.0f;
    m_viewMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, 0.0f));
    Debug::log(Debug::Area::ClientRendererArea) << "camera init, screen at width: " << Settings::instance()->screenResolutionWidth << " height: " << Settings::instance()->screenResolutionHeight;
//    m_orthoMatrix = glm::ortho(0.0f, float(Settings::instance()->screenResolutionWidth), float(Settings::instance()->screenResolutionHeight), 0.0f, -1.0f, 1.0f);
    m_orthoMatrix = glm::ortho(0.0f, float(1600.0f/PIXELS_PER_METER), float(900.0f/PIXELS_PER_METER), 0.0f, -1.0f, 1.0f);
}

void Camera::translate(const glm::vec2 vec)
{
    m_viewMatrix = glm::translate(m_viewMatrix, glm::vec3(vec, 0.0f));
    pushMatrix();
}

void Camera::zoom(const float factor)
{
    m_scaleFactor *= factor;

    m_viewMatrix = glm::scale(m_viewMatrix, glm::vec3(m_scaleFactor));

    pushMatrix();
}

void Camera::centerOn(const glm::vec2 vec)
{
    if (Settings::instance()->debugAreas & Debug::Area::ClientRendererArea) {
        return;
    }
    glm::vec2 position = glm::vec2((vec.x), (vec.y));

    glm::vec2 halfScreen((Settings::instance()->screenResolutionWidth)/PIXELS_PER_METER / 2.0f, (Settings::instance()->screenResolutionHeight/PIXELS_PER_METER) / 2.0f);

    m_viewMatrix =  glm::translate(glm::mat4(), -glm::vec3(position - halfScreen, 1.0f));
    m_viewMatrix = glm::scale(m_viewMatrix, glm::vec3(glm::vec2(m_scaleFactor), 1.0f));
    pushMatrix();
}

void Camera::setPosition(const glm::vec2 vec)
{
    m_viewMatrix = glm::translate(glm::mat4(), -glm::vec3(vec, 0.0f));
    m_viewMatrix = glm::scale(m_viewMatrix, glm::vec3(m_scaleFactor));
    pushMatrix();
}

void Camera::addShader(Shader* shader)
{
    assert(shader);
    m_shaders.push_back(shader);
    pushMatrix();
}

glm::mat4 Camera::ortho() const
{
    return m_orthoMatrix;
}
glm::mat4 Camera::view() const
{
    return m_viewMatrix;
}

void Camera::pushMatrix()
{
    Debug::assertf(m_shaders.size() > 0, "no shader to push the camera matrix too. This is INVALID");

    for (auto * shader : m_shaders) {
        shader->bindProgram();

        glm::mat4 mvp =  m_orthoMatrix * m_viewMatrix;

        int mvpLoc = glGetUniformLocation(shader->shaderProgram(), "mvp");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

        shader->unbindProgram();
    }
}

void Camera::setOrtho(const glm::mat4& ortho)
{
    m_orthoMatrix = ortho;
    pushMatrix();
}

void Camera::setView(const glm::mat4& view)
{
    m_viewMatrix = view;
    pushMatrix();
}
