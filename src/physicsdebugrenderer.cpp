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
#include "world.h"

PhysicsDebugRenderer::PhysicsDebugRenderer(Camera* camera)
{
    m_shader = new Shader("physicsdebugrenderer.vert", "physicsdebugrenderer.frag");
    m_shader->bindProgram();
    setCamera(camera);

    Debug::checkGLError();
    initGL();
    Debug::checkGLError();
}

PhysicsDebugRenderer::~PhysicsDebugRenderer()
{
    glDeleteBuffers(1, &m_vbo);

    glDeleteVertexArrays(1, &m_vao);
}

void PhysicsDebugRenderer::setCamera(Camera* camera)
{
    m_camera = camera;
//    m_camera->addShader(m_shader);
}

void PhysicsDebugRenderer::initGL()
{

}

void PhysicsDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    /*
    glColor4f(color.r, color.g, color.b,1);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
    */
}

void PhysicsDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
//glTranslatef(100, 100, 0);

/*    glViewport(500, 0, 1500, 900);
//    vertices[0].x = 2;
    glVertexPointer(2, GL_FLOAT, 0, vertices);

    glColor4f(color.r, color.g, color.b,0.5f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

    glColor4f(color.r, color.g, color.b,1);
    glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
    glViewport(0, 0, 1600, 900);
    */
//set up vertex array
/*
GLfloat glverts[16]; //allow for polygons up to 8 vertices
glVertexPointer(2, GL_FLOAT, 0, glverts); //tell OpenGL where to find vertices
glEnableClientState(GL_VERTEX_ARRAY); //use vertices in subsequent calls to glDrawArrays

//fill in vertex positions as directed by Box2D
for (int i = 0; i < vertexCount; i++) {
    glverts[i*2]   = vertices[i].x;
    glverts[i*2+1] = vertices[i].y;
}

//draw solid area
glColor4f( color.r, color.g, color.b, 1);
glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

//draw lines
glLineWidth(3); //fat lines
glColor4f( 1, 0, 1, 1 ); //purple
glDrawArrays(GL_LINE_LOOP, 0, vertexCount);

glPopMatrix();
*/


    m_shader->bindProgram();

    glm::mat4 view = glm::mat4(); // glm::translate(glm::mat4(), glm::vec3(translation.x, translation.y, 0.0f));

    glm::mat4 mvp = m_camera->ortho() * m_camera->view();

    int mvpLoc = glGetUniformLocation(m_shader->shaderProgram(), "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // vertices that will be uploaded.
    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Rocket::Core::Vertex),
                        (const GLvoid*)buffer_offset
    );

    buffer_offset += sizeof(f32)*2;

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Rocket::Core::Vertex),
                        (const GLvoid*)buffer_offset
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        num_indices*sizeof(indices),
                indices,
                GL_DYNAMIC_DRAW
    );

    // finally upload everything to the actual vbo
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Rocket::Core::Vertex) * num_vertices,
                vertices,
                GL_DYNAMIC_DRAW
    );

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bindProgram();

    glDrawElements(
        GL_TRIANGLES,
        num_indices,
        GL_UNSIGNED_INT,
        (const GLvoid*)0
    );

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
}

void PhysicsDebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    /*
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
    */
}

void PhysicsDebugRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    /*
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


    // Draw the axis line
    DrawSegment(center,center+radius*axis,color);
    */
}

void PhysicsDebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    /*
    glColor4f(color.r, color.g, color.b,1);
    GLfloat                         glVertices[] = {
        p1.x,p1.y,p2.x,p2.y
    };
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_LINES, 0, 2);
    */
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
    /*
    glColor4f(color.r, color.g, color.b,1);
    glPointSize(size);
    GLfloat                         glVertices[] = {
        p.x,p.y
    };
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_POINTS, 0, 1);
    glPointSize(1.0f);
    */
}

void PhysicsDebugRenderer::DrawString(int x, int y, const char *string, ...)
{

    /* Unsupported as yet. Could replace with bitmap font renderer at a later date */
}

void PhysicsDebugRenderer::DrawAABB(b2AABB* aabb, const b2Color& c)
{
    /*
    glColor4f(c.r, c.g, c.b,1);

    GLfloat                         glVertices[] = {
        aabb->lowerBound.x, aabb->lowerBound.y,
        aabb->upperBound.x, aabb->lowerBound.y,
        aabb->upperBound.x, aabb->upperBound.y,
        aabb->lowerBound.x, aabb->upperBound.y
    };
    glVertexPointer(2, GL_FLOAT, 0, glVertices);
    glDrawArrays(GL_LINE_LOOP, 0, 8);
    */
}


void PhysicsDebugRenderer::render()
{

}
