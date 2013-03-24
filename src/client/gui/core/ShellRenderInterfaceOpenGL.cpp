/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "ShellRenderInterfaceOpenGL.h"

#include "src/image.h"
#include "src/debug.h"
#include "src/shader.h"
#include <src/texture.h>

#include <Rocket/Core.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GL_CLAMP_TO_EDGE 0x812F

ShellRenderInterfaceOpenGL::ShellRenderInterfaceOpenGL()
{
    m_shader = new Shader("guirenderer.vert", "guirenderer.frag");
    m_shader->bindProgram();

    m_tempTexture = new Texture("../textures/error.png");
    m_tempTexture->generate();
    m_tempTexture->bind();

    Debug::checkGLError();
    initGL();
}

void ShellRenderInterfaceOpenGL::initGL()
{
    ///////////CHARACTERS////////////////
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_ebo);
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxSpriteCount * 4 * sizeof(Rocket::Core::Vertex),
                 NULL,
                 GL_DYNAMIC_DRAW);

    Debug::checkGLError();


    Debug::checkGLError();


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Debug::checkGLError();

}


void ShellRenderInterfaceOpenGL::SetViewport(int width, int height)
{
    m_width = width;
    m_height = height;
}

// Called by Rocket when it wants to render geometry that it does not wish to optimise.
void ShellRenderInterfaceOpenGL::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
   if (!texture) {
       //HACK
       return;
   }

   m_shader->bindProgram();

   /*
   Debug::log() << "VERT count! " << num_vertices;
   Debug::log() << "INDICE count! " << num_indices;

   for (int i = 0; i < num_vertices; ++i) {
        Debug::log() << "VERT i: " << i << " pos x: " << vertices[i].position.x << " Y: " << vertices[i].position.y;
   }

    for (int i = 0; i < num_indices; ++i) {
        Debug::log() << "INDICE i: " << i << " INDICE: " << indices[i];
   }
   */


   glm::mat4 view = glm::mat4(); // glm::translate(glm::mat4(), glm::vec3(x, y, 0.0f));
   glm::mat4 ortho = glm::ortho(0.0f, float(1600), float(900), 0.0f, -1.0f, 1.0f);

   glm::mat4 mvp =  ortho * view;

   int mvpLoc = glGetUniformLocation(m_shader->shaderProgram(), "mvp");
   glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

//   glBindTexture(GL_TEXTURE_2D, GLuint(texture));

   glActiveTexture(GL_TEXTURE0);
    m_tempTexture->bind();

   Debug::checkGLError();


   glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // vertices that will be uploaded.
    size_t buffer_offset = 0;
    Debug::checkGLError();

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

    buffer_offset += sizeof(f32) * 2;

    Debug::checkGLError();
    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Rocket::Core::Vertex),
        (const GLvoid*)buffer_offset
    );
    buffer_offset += sizeof(u32);

    Debug::checkGLError();

    GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Rocket::Core::Vertex),
        (const GLvoid*)buffer_offset
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        num_indices*sizeof(indices),
                    indices,
                    GL_DYNAMIC_DRAW);


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

//    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

   Debug::checkGLError();

   m_shader->bindProgram();

   Debug::checkGLError();

   glDrawElements(
       GL_TRIANGLES,
       //num_vertices * (num_indices),
       2000,
                  GL_UNSIGNED_INT,
                  (const GLvoid*)0);

   m_shader->unbindProgram();
   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   glDisable(GL_BLEND);

   Debug::checkGLError();
}

// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
Rocket::Core::CompiledGeometryHandle ShellRenderInterfaceOpenGL::CompileGeometry(Rocket::Core::Vertex* ROCKET_UNUSED(vertices), int ROCKET_UNUSED(num_vertices), int* ROCKET_UNUSED(indices), int ROCKET_UNUSED(num_indices), const Rocket::Core::TextureHandle ROCKET_UNUSED(texture))
{
    return (Rocket::Core::CompiledGeometryHandle) NULL;
}

// Called by Rocket when it wants to render application-compiled geometry.
void ShellRenderInterfaceOpenGL::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry), const Rocket::Core::Vector2f& ROCKET_UNUSED(translation))
{
}

// Called by Rocket when it wants to release application-compiled geometry.
void ShellRenderInterfaceOpenGL::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry))
{
}

// Called by Rocket when it wants to enable or disable scissoring to clip content.
void ShellRenderInterfaceOpenGL::EnableScissorRegion(bool enable)
{
    if (enable)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

// Called by Rocket when it wants to change the scissor region.
void ShellRenderInterfaceOpenGL::SetScissorRegion(int x, int y, int width, int height)
{
    glScissor(x, m_height - (y + height), width, height);
}

// Called by Rocket when a texture is required by the library.
bool ShellRenderInterfaceOpenGL::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
    Debug::log(Debug::Area::Graphics) << "Rocket OpenGL Renderer, loading texture name: " << source.CString() << " TEX WIDTH :" << texture_dimensions.x << " y: " << texture_dimensions.y;

    Image* image = new Image(source.CString());
    image->flipVertically();

    texture_dimensions.x = image->width();
    texture_dimensions.y = image->height();

    bool success = GenerateTexture(texture_handle, image->bytes(), texture_dimensions);

    return success;
}

// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
bool ShellRenderInterfaceOpenGL::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    if (texture_id == 0) {
        printf("Failed to generate textures\n");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_BGRA, GL_UNSIGNED_BYTE, source);

    texture_handle = (Rocket::Core::TextureHandle) texture_id;

    return true;
}

// Called by Rocket when a loaded texture is no longer required.
void ShellRenderInterfaceOpenGL::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
    glDeleteTextures(1, (GLuint*) &texture_handle);
}

