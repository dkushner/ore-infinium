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

#include "physicsdebugrenderer.h"
#include "debug.h"

void PhysicsDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor4f(color.r, color.g, color.b,1);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void PhysicsDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);

    glColor4f(color.r, color.g, color.b,0.5f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

    glColor4f(color.r, color.g, color.b,1);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void PhysicsDebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    const float32 k_segments = 16.0f;
    int vertexCount=16;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;

    GLfloat                         glVertices[vertexCount*2];
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertices[i*2]=v.x;
        glVertices[i*2+1]=v.y;
        theta += k_increment;
    }

    glColor4f(color.r, color.g, color.b,1);
    glVertexPointer(2, GL_FLOAT, 0, glVertices);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void PhysicsDebugRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    const float32 k_segments = 16.0f;
    int vertexCount=16;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;

    GLfloat                         glVertices[vertexCount*2];
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertices[i*2]=v.x;
        glVertices[i*2+1]=v.y;
        theta += k_increment;
    }

    glColor4f(color.r, color.g, color.b,0.5f);
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    glColor4f(color.r, color.g, color.b,1);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);

    glDisableClientState(GL_VERTEX_ARRAY);

    // Draw the axis line
    DrawSegment(center,center+radius*axis,color);
}

void PhysicsDebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor4f(color.r, color.g, color.b,1);
    GLfloat                         glVertices[] = {
        p1.x,p1.y,p2.x,p2.y
    };
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void PhysicsDebugRenderer::DrawTransform(const b2Transform& xf)
{
    /*
    b2Vec2 p1 = xf.p, p2;
    const float32 k_axisScale = 0.4f;

    p2 = p1 + k_axisScale * xf.q.c;
    DrawSegment(p1,p2,b2Color(1,0,0));

    p2 = p1 + k_axisScale * xf.q.s;
    DrawSegment(p1,p2,b2Color(0,1,0));
    */
}

void PhysicsDebugRenderer::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor4f(color.r, color.g, color.b,1);
    glPointSize(size);
    GLfloat                         glVertices[] = {
        p.x,p.y
    };
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_POINTS, 0, 1);
    glPointSize(1.0f);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void PhysicsDebugRenderer::DrawString(int x, int y, const char *string, ...)
{

    /* Unsupported as yet. Could replace with bitmap font renderer at a later date */
}

void PhysicsDebugRenderer::DrawAABB(b2AABB* aabb, const b2Color& c)
{
    glEnableClientState(GL_VERTEX_ARRAY);

    glColor4f(c.r, c.g, c.b,1);

    GLfloat                         glVertices[] = {
        aabb->lowerBound.x, aabb->lowerBound.y,
        aabb->upperBound.x, aabb->lowerBound.y,
        aabb->upperBound.x, aabb->upperBound.y,
        aabb->lowerBound.x, aabb->upperBound.y
    };
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_LINE_LOOP, 0, 8);
    glDisableClientState(GL_VERTEX_ARRAY);

}
