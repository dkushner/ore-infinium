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

#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera();

    /**
     * Translates the camera's current position by the given vec2.
     */
    void translate(const glm::vec2 vec);

    /**
     * Zooms/scales the view (z) by the given factor.
     * 0.5 is zoom out, 1.5 is zoom in, etc.
     */
    void zoom(const float factor);

    void centerOn(const glm::vec2 vec);

    glm::mat4 ortho() const { return m_orthoMatrix; }
    glm::mat4 view() const { return m_viewMatrix; }

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_orthoMatrix;
    glm::vec3 m_vector;
};

#endif
