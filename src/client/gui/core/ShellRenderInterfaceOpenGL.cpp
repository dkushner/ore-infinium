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

#include <Rocket/Core.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GL_CLAMP_TO_EDGE 0x812F

ShellRenderInterfaceOpenGL::ShellRenderInterfaceOpenGL()
{
    m_shader = new Shader("guirenderer.vert", "guirenderer.frag");
    m_shader->bindProgram();


    Debug::checkGLError();
    initGL();
}

void ShellRenderInterfaceOpenGL::initGL()
{
    ///////////CHARACTERS////////////////
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxSpriteCount * 4 * sizeof(Vertex),
                 NULL,
                 GL_DYNAMIC_DRAW);

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

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(u32),
                 indicesv.data(),
                 GL_STATIC_DRAW);

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


   Debug::checkGLError();

   int index = 0;

       // vertices that will be uploaded.
       size_t buffer_offset = 0;

       GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
       glEnableVertexAttribArray(pos_attrib);
       glVertexAttribPointer(
           pos_attrib,
           2,
           GL_FLOAT,
           GL_FALSE,
           sizeof(Rocket::Core::Vertex)
                             &vertices[0].position);

       GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

       Debug::checkGLError();

       glEnableVertexAttribArray(color_attrib);
       glVertexAttribPointer(
           color_attrib,
           4,
           GL_UNSIGNED_BYTE,
           GL_TRUE,
           sizeof(Rocket::Core::Vertex),
                             &vertices[0].colour);

       Debug::checkGLError();

       GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
       glEnableVertexAttribArray(texcoord_attrib);
       glVertexAttribPointer(
           texcoord_attrib,
           2,
           GL_FLOAT,
           GL_FALSE,
           sizeof(Rocket::Core::Vertex),
                             &vertices[0].tex_coord);

       // vertices[n][0] -> X, and [1] -> Y
       // vertices[0] -> top left
       // vertices[1] -> bottom left
       // vertices[2] -> bottom right
       // vertices[3] -> top right

       glm::vec2 spritePosition = glm::vec2(0, 0);

       glm::vec2 spriteSize = glm::vec2(1600, 900);

       glm::vec4 rect = glm::vec4(spritePosition.x, spritePosition.y, spritePosition.x + spriteSize.x, spritePosition.y + spriteSize.y);

       float x = rect.x;
       float width = rect.z;
       float y = rect.y;
       float height = rect.w;

       // finally upload everything to the actual vbo
       glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
       glBufferSubData(
           GL_ARRAY_BUFFER,
           sizeof(vertices) * index,
                       sizeof(vertices),
                       vertices);

       ++index;

   ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
   glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
   glBindVertexArray(m_vao);

   Debug::checkGLError();

   m_shader->bindProgram();

   Debug::checkGLError();

   glDrawElements(
       GL_TRIANGLES,
       6 * (m_characterSprites.size()), // 6 indices per 2 triangles
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

