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

#include <GL/glew.h>
#include <Box2D/Box2D.h>
#include <glm/core/type.hpp>

#include "src/camera.h"
#include "src/shader.h"

class PhysicsDebugRenderer : public b2Draw {
public:
    PhysicsDebugRenderer();
    virtual ~PhysicsDebugRenderer();

    virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
    virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

    virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
    virtual void DrawTransform(const b2Transform& xf);

    virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

    virtual void DrawString(int x, int y, const char* string, ...);

    virtual void DrawAABB(b2AABB* aabb, const b2Color& color);

    void render();

private:
    void initGL();

private:
    typedef uint32_t u32;
    typedef float f32;

    /* Each vertex is:
     * two floats for the 2d coordinate
     * four u8s for the color
     * two f32s for the texcoords
     * the vbo contains data of the aforementioned elements interleaved.
     * Each sprite has four vertices.
     * */
    struct Vertex {
        float x, y;
        unsigned int color; // packed with 4 u8s (unsigned chars) for color

        float u = 0.0f;
        float v = 0.0f;
    };

    struct Box2DQuad {
        b2Vec2* vec;
        int32_t vertexCount;
        b2Color color;
    };

    std::vector<Box2DQuad> m_solidPolygons;

    GLuint m_vao; // vertex array object
    GLuint m_vbo; // vertex buffer object
    GLuint m_ebo; // element buffer object

    GLuint m_vaoEntities; // vertex array object
    GLuint m_vboEntities; // vertex buffer object
    GLuint m_eboEntities; // element buffer object

    glm::mat4 m_modelMatrix;
    glm::mat4 m_projectionMatrix;

    int m_maxSpriteCount = 2200;

    Camera* m_camera = nullptr;
    Shader* m_shader = nullptr;
};

#endif
