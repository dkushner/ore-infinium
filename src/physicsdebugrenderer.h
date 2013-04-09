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

#ifndef PHYSICSDEBUGRENDERER_H
#define PHYSICSDEBUGRENDERER_H

#include "src/camera.h"
#include "src/shader.h"

#include <GL/glew.h>
#include <Box2D/Box2D.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>

class PhysicsDebugRenderer : public b2Draw
{
public:
    PhysicsDebugRenderer(Camera* camera);
    virtual ~PhysicsDebugRenderer();

    virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
    virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

    virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
    virtual void DrawTransform(const b2Transform& xf);

    void setCamera(Camera* camera);

    void render();

private:
    void initGL();
    void initGLSolidPolygons();
    void initGLSegment();

    void renderSolidPolygons();

    /* Each vertex is:
     * two floats for the 2d coordinate
     * four uint8_t's for the color (packed in 1 uint32_t)
     * */
    struct Vertex {
        float x, y;
        uint32_t color; // packed with 4 u8s (unsigned chars) for color
    };

private:
    GLuint m_iboSolidPolygons; // index buffer object
    GLuint m_vaoSolidPolygons; // vertex array object
    GLuint m_vboSolidPolygons; // vertex buffer object

    std::vector<Vertex> m_verticesSolidPolygons;
    std::vector<uint16_t> m_indicesSolidPolygons;
    size_t m_maxVBOSizeSolidPolygons = 0;
    size_t m_highestIBOSizeSolidPolygons = 0;

    GLuint m_iboSegment; // index buffer object
    GLuint m_vaoSegment; // vertex array object
    GLuint m_vboSegment; // vertex buffer object

    std::vector<Vertex> m_verticesSegment;
    std::vector<uint16_t> m_indicesSegment;
    size_t m_maxVBOSizeSegment = 0;
    size_t m_highestIBOSizeSegment = 0;

    glm::mat4 m_ortho;

    Camera* m_camera = nullptr;
    Shader* m_shader = nullptr;
};

#endif
