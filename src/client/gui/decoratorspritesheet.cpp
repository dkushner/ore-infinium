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

#include "decoratorspritesheet.h"

#include <GL/glew.h>
#include "core/ShellOpenGL.h"

#include <src/debug.h>
#include <src/settings/settings.h>

#include <Rocket/Core/Element.h>
#include <Rocket/Core/Texture.h>
#include <Rocket/Core/Math.h>

DecoratorSpriteSheet::~DecoratorSpriteSheet()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);

    delete m_shader;
}

bool DecoratorSpriteSheet::Initialise()
{
    m_shader = new Shader("decoratorspritesheet.vert", "decoratorspritesheet.frag");
    m_shader->bindProgram();

    m_ortho = glm::ortho(0.0f, float(Settings::instance()->screenResolutionWidth), float(Settings::instance()->screenResolutionHeight), 0.0f, -1.0f, 1.0f);
    glm::mat4 mvp =  m_ortho;

    int mvpLoc = glGetUniformLocation(m_shader->shaderProgram(), "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    initGL();

    return true;
}

void DecoratorSpriteSheet::initGL()
{
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
    buffer_offset += sizeof(u32);

    Debug::checkGLError();

    GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Debug::checkGLError();
}

/// Called on a decorator to generate any required per-element data for a newly decorated element.
Rocket::Core::DecoratorDataHandle DecoratorSpriteSheet::GenerateElementData(Rocket::Core::Element* ROCKET_UNUSED(element))
{
    return static_cast<uintptr_t>(NULL);
}

// Called to release element data generated by this decorator.
void DecoratorSpriteSheet::ReleaseElementData(Rocket::Core::DecoratorDataHandle ROCKET_UNUSED(element_data))
{
}

// Called to render the decorator on an element.
void DecoratorSpriteSheet::RenderElement(Rocket::Core::Element* element, Rocket::Core::DecoratorDataHandle ROCKET_UNUSED(element_data))
{
    const Rocket::Core::Property* imageSourceProperty = element->GetProperty("image-src");

    //don't render it, property has not been set.
    if (!imageSourceProperty) {
        return;
    }

    Rocket::Core::String imageSource = imageSourceProperty->ToString();

    //don't render it, the property image source has been set to null, in other words they don't want anything renderered on it.
    if (imageSource == "") {
       return;
    }

    if (m_textureName != imageSource) {
        //requested a new texture
        //FIXME:: this doesn't unload the texture because it makes an assumption..which i am not yet certain is correct/valid.

        image_index = LoadTexture(imageSource, imageSourceProperty->source);

        if ( image_index == -1) {
            Debug::fatal(false, Debug::Area::Graphics, "librocket inventory decorator render element texture load failure. likely invalid path specified, or something.");
        }
    }

    m_textureName = imageSource;

    Rocket::Core::Vector2f position = element->GetAbsoluteOffset(Rocket::Core::Box::PADDING);
    Rocket::Core::Vector2f size = element->GetBox().GetSize(Rocket::Core::Box::PADDING);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, (GLuint) GetTexture(image_index)->GetHandle(element->GetRenderInterface()));

    // vertices that will be uploaded.
    Vertex vertices[4];

    Rocket::Core::Colourb colour = element->GetProperty<Rocket::Core::Colourb>("color");

    Rocket::Core::Vector2i textureDimensions = GetTexture(image_index)->GetDimensions(element->GetRenderInterface());

    glm::vec4 rect = glm::vec4(position.x, position.y, position.x + size.x, position.y + size.y);

    float x = rect.x;
    float width = rect.z;
    float y = rect.y;
    float height = rect.w;

    // vertices[n][0] -> X, and [1] -> Y
    // vertices[0] -> top left
    // vertices[1] -> bottom left
    // vertices[2] -> bottom right
    // vertices[3] -> top right

    vertices[0].x = x; // top left X
    vertices[0].y = y; //top left Y

    vertices[1].x = x; // bottom left X
    vertices[1].y = height; // bottom left Y

    vertices[2].x = width; // bottom right X
    vertices[2].y = height; //bottom right Y

    vertices[3].x = width; // top right X
    vertices[3].y = y; // top right Y

    Debug::checkGLError();

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

    const int frameX = element->GetProperty<int>("image-x1");
    const int frameY = element->GetProperty<int>("image-y1");
    const int frameWidth = element->GetProperty<int>("image-x2");
    const int frameHeight = element->GetProperty<int>("image-y2");

    const float textureWidth = float(frameWidth) / float(textureDimensions.x);
    const float textureHeight = float(frameHeight) / float(textureDimensions.y);
    const float textureX = float(frameX) / float(textureDimensions.x);
    const float textureY = float(frameY) / float(textureDimensions.y);

    const float spriteLeft = textureX;
    const float spriteRight = spriteLeft + textureWidth;
    const float spriteTop = (textureY);
    const float spriteBottom = textureHeight;

    // copy texcoords to the buffer
    vertices[0].u = vertices[1].u = spriteLeft;
    vertices[0].v = vertices[3].v = spriteTop;
    vertices[1].v = vertices[2].v = spriteBottom;
    vertices[2].u = vertices[3].u = spriteRight;

    // finally upload everything to the actual vbo
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
                    sizeof(vertices),
                    vertices);

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
        12, // 1 quad
                GL_UNSIGNED_INT,
                (const GLvoid*)0);

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    Debug::checkGLError();
}
