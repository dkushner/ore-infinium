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
    ///////////CHARACTERS////////////////
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    Debug::checkGLError();
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxSpriteCount * 4 * sizeof(Vertex),
                 NULL,
                 GL_DYNAMIC_DRAW);

    Debug::checkGLError();
    Debug::checkGLError();

    std::vector<u32> indicesv;

    // prepare and upload indices as a one time deal
    const u32 indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (size_t j = 0; j < m_maxSpriteCount; j++) {
        for (size_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    Debug::checkGLError();


    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(f32) * 2;

    Debug::checkGLError();
    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    Debug::checkGLError();
    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(u32);

    Debug::checkGLError();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Debug::checkGLError();

    Debug::checkGLError();
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

    Box2DQuad box2DQuad;
    box2DQuad.vec = vertices;
    box2DQuad.vertexCount = vertexCount;
    box2DQuad.color = color;

    m_solidPolygons.push_back(box2DQuad);
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
    if (m_solidPolygons.size() == 0) {
        Debug::log() << "POLYGON SET EMPTY!";
        return;
    }

    m_shader->bindProgram();

//    glm::mat4 ortho = m_camera->ortho();
//    glm::mat4 view = m_camera->view();

    glm::mat4 view = glm::mat4();
    glm::mat4 ortho = glm::ortho(0.0f, float(1), float(1), 0.0f, -1.0f, 1.0f);
    glm::mat4 mvp = ortho;// * view;

    int mvpLoc = glGetUniformLocation(m_shader->shaderProgram(), "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    uint32_t index = 0;
//    for (Box2DQuad box2DQuad: m_solidPolygons) {
    // vertices that will be uploaded.
    std::vector<Vertex> vertices;

    Debug::checkGLError();
    /*
    for (int i = 0; i < box2DQuad.vertexCount; ++i) {
        Vertex vert;
        vert.u = 0.0f;
        vert.v = 0.0f;

        uint8_t red = 255;
        uint8_t green = 255;
        uint8_t blue = 0;
        uint8_t alpha = 255;
        uint32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);

        vert.color = color;

        vert.x = box2DQuad.vec[i].x;
        vert.y = box2DQuad.vec[i].y;
       vertices.push_back(vert);
    }
    */
            uint8_t red = 255;
        uint8_t green = 255;
        uint8_t blue = 0;
        uint8_t alpha = 255;
        uint32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);

    Vertex vert1;
    vert1.color = color;
    vert1.x = 0.1;
    vert1.y = 0.1;
    Vertex vert2;
    vert2.color = color;
    vert2.x = 1;
    vert2.y = 1;
    Vertex vert3;
    vert3.color = color;
    vert3.x = 0.5;
    vert3.y = 0.5;
    Vertex vert4;
    vert4.color = color;
    vert4.x = 0.8;
    vert4.y = 0.8;
    Vertex vert5;
    vert4.color = color;
    vert4.x = 0.2;
    vert4.y = 0.2;
    Vertex vert6;
    vert4.color = color;
    vert4.x = 0.4;
    vert4.y = 0.4;

    vertices.push_back(vert1);
    vertices.push_back(vert2);
    vertices.push_back(vert3);
    vertices.push_back(vert4);
    vertices.push_back(vert5);
    vertices.push_back(vert6);

    Debug::log() << "VERT COUNT:  " << vertices.size();

    /*
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        vertices.size()  * sizeof(Vertex),
                 vertices.data(),
                 GL_DYNAMIC_DRAW);
                 */

    /*
    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(Vertex) * index,
                    sizeof(Vertex),
                    &vertices[0]);
                    */

    Debug::checkGLError();
    /*
    // copy color to the buffer
    for (size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); i++) {
        //        *colorp = color.bgra;
        uint8_t red = 255;
        uint8_t green = 255;
        uint8_t blue = 255;
        uint8_t alpha = 255;
        int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
        vertices[i].color = color;
    }
    */

    // finally upload everything to the actual vbo
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        1,
        sizeof(Vertex) * 1,//index,
                    &vertices[0]
    );

    Debug::checkGLError();
    ++index;
//    }

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);

    Debug::checkGLError();

    m_shader->bindProgram();

    Debug::checkGLError();

    glDrawArrays(GL_TRIANGLES, 0, 20);

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    Debug::checkGLError();

    m_solidPolygons.clear();
}
