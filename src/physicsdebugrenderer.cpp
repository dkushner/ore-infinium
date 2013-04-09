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
#include "settings/settings.h"

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
    glDeleteBuffers(1, &m_vboSolidPolygons);
    glDeleteVertexArrays(1, &m_vaoSolidPolygons);

    // NOTE: cam is not ours to delete.
    delete m_shader;
}

void PhysicsDebugRenderer::setCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->addShader(m_shader);
}

void PhysicsDebugRenderer::initGL()
{
    initGLSolidPolygons();
    initGLPolygons();
    initGLSegments();
}

void PhysicsDebugRenderer::initGLPolygons()
{
    glGenVertexArrays(1, &m_vaoPolygons);
    glBindVertexArray(m_vaoPolygons);

    glGenBuffers(1, &m_vboPolygons);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboPolygons);
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
                          (const GLvoid*)0
    );
    buffer_offset += sizeof(float) * 2;
    Debug::checkGLError();

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    glGenBuffers(1, &m_iboPolygons);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Debug::checkGLError();
}

void PhysicsDebugRenderer::initGLSolidPolygons()
{
    glGenVertexArrays(1, &m_vaoSolidPolygons);
    glBindVertexArray(m_vaoSolidPolygons);

    glGenBuffers(1, &m_vboSolidPolygons);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSolidPolygons);
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
                          (const GLvoid*)0
    );
    buffer_offset += sizeof(float) * 2;
    Debug::checkGLError();

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    glGenBuffers(1, &m_iboSolidPolygons);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Debug::checkGLError();
}

void PhysicsDebugRenderer::initGLSegments()
{
    glGenVertexArrays(1, &m_vaoSegments);
    glBindVertexArray(m_vaoSegments);

    glGenBuffers(1, &m_vboSegments);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSegments);
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
                          (const GLvoid*)0
    );
    buffer_offset += sizeof(float) * 2;
    Debug::checkGLError();

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    glGenBuffers(1, &m_iboSegments);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Debug::checkGLError();
}

void PhysicsDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
        const size_t iboOffset = m_verticesPolygons.size();

    for (size_t i = 0; i < vertexCount; i++) {
        Vertex vertex;
        vertex.x = vertices[i].x;
        vertex.y = vertices[i].y;

        uint8_t red = static_cast<uint8_t>(ceil(color.r * 255));
        uint8_t green = static_cast<uint8_t>(ceil(color.g * 255));
        uint8_t blue = static_cast<uint8_t>(ceil(color.b * 255));
        uint8_t alpha = 80;
        int32_t colorPacked = red | (green << 8) | (blue << 16) | (alpha << 24);
        vertex.color = colorPacked;

        m_verticesPolygons.push_back(vertex);
    }

    for (int i = 0; i < vertexCount ; i++) {
        m_indicesPolygons.push_back(iboOffset);
        m_indicesPolygons.push_back(iboOffset + i);
        m_indicesPolygons.push_back(iboOffset + i + 1);
    }
}

void PhysicsDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    const size_t iboOffset = m_verticesSolidPolygons.size();

    for (size_t i = 0; i < vertexCount; i++) {
        Vertex vertex;
        vertex.x = vertices[i].x;
        vertex.y = vertices[i].y;

        uint8_t red = static_cast<uint8_t>(ceil(color.r * 255));
        uint8_t green = static_cast<uint8_t>(ceil(color.g * 255));
        uint8_t blue = static_cast<uint8_t>(ceil(color.b * 255));
        uint8_t alpha = 80;
        int32_t colorPacked = red | (green << 8) | (blue << 16) | (alpha << 24);
        vertex.color = colorPacked;

        m_verticesSolidPolygons.push_back(vertex);
    }

    for (int i = 1; i < vertexCount - 1; i++) {
        m_indicesSolidPolygons.push_back(iboOffset);
        m_indicesSolidPolygons.push_back(iboOffset + i);
        m_indicesSolidPolygons.push_back(iboOffset + i + 1);
    }
}

//FIXME: the circle calls need to be fixed to not run a draw call each polygon count
void PhysicsDebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
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
}

void PhysicsDebugRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
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
    DrawSegment(center, center + radius * axis, color);
}

void PhysicsDebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    std::vector<b2Vec2> vertices;
    vertices.push_back(p1);
    vertices.push_back(p2);

    size_t vertexCount = vertices.size();

    for (size_t i = 0; i < vertexCount; i++) {
        Vertex vertex;
        vertex.x = vertices[i].x;
        vertex.y = vertices[i].y;

        uint8_t red = static_cast<uint8_t>(ceil(color.r * 255));
        uint8_t green = static_cast<uint8_t>(ceil(color.g * 255));
        uint8_t blue = static_cast<uint8_t>(ceil(color.b * 255));
        uint8_t alpha = 255;
        int32_t colorPacked = red | (green << 8) | (blue << 16) | (alpha << 24);
        vertex.color = colorPacked;

        m_verticesSegments.push_back(vertex);
    }

    const size_t iboOffset = m_verticesSegments.size();

    m_indicesSegments.push_back(iboOffset);
    m_indicesSegments.push_back(iboOffset + 1);
}

void PhysicsDebugRenderer::DrawTransform(const b2Transform& xf)
{
    b2Vec2 p1 = xf.p, p2;
    const float32 k_axisScale = 0.4f;
    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    DrawSegment(p1, p2, b2Color(1.0f, 0.0f, 0.0f));

    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    DrawSegment(p1, p2, b2Color(0.0f, 1.0f, 0.0f));
}

void PhysicsDebugRenderer::render()
{
    renderSolidPolygons();
    renderPolygons();
    renderSegments();
}

void PhysicsDebugRenderer::renderPolygons()
{
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bindProgram();
    Debug::checkGLError();

    glBindVertexArray(m_vaoPolygons);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboPolygons);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboPolygons);

    if (m_verticesPolygons.size() > m_maxVBOSizePolygons) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verticesPolygons.size(), m_verticesPolygons.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_verticesPolygons.size(), m_verticesPolygons.data());
    }

    if (m_indicesPolygons.size() > m_highestIBOSizePolygons) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * m_indicesPolygons.size(), m_indicesPolygons.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint16_t) * m_indicesPolygons.size(), m_indicesPolygons.data());
    }

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_LINES, m_indicesPolygons.size(), GL_UNSIGNED_SHORT, (GLvoid*)0);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_maxVBOSizePolygons = m_verticesPolygons.size();
    m_highestIBOSizePolygons = m_indicesPolygons.size();
    m_verticesPolygons.clear();
    m_indicesPolygons.clear();
}

void PhysicsDebugRenderer::renderSolidPolygons()
{
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bindProgram();
    Debug::checkGLError();

    glBindVertexArray(m_vaoSolidPolygons);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSolidPolygons);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSolidPolygons);

    if (m_verticesSolidPolygons.size() > m_maxVBOSizeSolidPolygons) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verticesSolidPolygons.size(), m_verticesSolidPolygons.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_verticesSolidPolygons.size(), m_verticesSolidPolygons.data());
    }

    if (m_indicesSolidPolygons.size() > m_highestIBOSizeSolidPolygons) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * m_indicesSolidPolygons.size(), m_indicesSolidPolygons.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint16_t) * m_indicesSolidPolygons.size(), m_indicesSolidPolygons.data());
    }

    glDrawElements(GL_TRIANGLES, m_indicesSolidPolygons.size(), GL_UNSIGNED_SHORT, (GLvoid*)0);

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_maxVBOSizeSolidPolygons = m_verticesSolidPolygons.size();
    m_highestIBOSizeSolidPolygons = m_indicesSolidPolygons.size();
    m_verticesSolidPolygons.clear();
    m_indicesSolidPolygons.clear();
}

void PhysicsDebugRenderer::renderSegments()
{
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bindProgram();
    Debug::checkGLError();

    glBindVertexArray(m_vaoSegments);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSegments);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSegments);

    if (m_verticesSegments.size() > m_maxVBOSizeSegments) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verticesSegments.size(), m_verticesSegments.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_verticesSegments.size(), m_verticesSegments.data());
    }

    if (m_indicesSegments.size() > m_highestIBOSizeSegments) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * m_indicesSegments.size(), m_indicesSegments.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint16_t) * m_indicesSegments.size(), m_indicesSegments.data());
    }

    glDrawElements(GL_LINES, m_indicesSegments.size(), GL_UNSIGNED_SHORT, (GLvoid*)0);

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Debug::log(Debug::ClientRendererArea) << " DREW ALL SEGMENTS";
    m_maxVBOSizeSegments = m_verticesSegments.size();
    m_highestIBOSizeSegments = m_indicesSegments.size();
    m_verticesSegments.clear();
    m_indicesSegments.clear();
}
