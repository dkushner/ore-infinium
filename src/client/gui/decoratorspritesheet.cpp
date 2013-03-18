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

#include <Rocket/Core/Element.h>
#include <Rocket/Core/Texture.h>
#include <Rocket/Core/Math.h>

#include "core/ShellOpenGL.h"
#include <src/debug.h>

DecoratorSpriteSheet::~DecoratorSpriteSheet()
{
}

bool DecoratorSpriteSheet::Initialise()
{
    /*
    Debug::log() << "DECORATOR LOADING TEX: " << image_source.CString() << " imagepath: " << image_path.CString();

    if (image_source == "") {
        return false;
    }

    image_index = LoadTexture(image_source, image_path);
    if (image_index == -1)
    {
        return false;
    }
    */
    return true;
}

/// Called on a decorator to generate any required per-element data for a newly decorated element.
Rocket::Core::DecoratorDataHandle DecoratorSpriteSheet::GenerateElementData(Rocket::Core::Element* ROCKET_UNUSED(element))
{
    return NULL;
}

// Called to release element data generated by this decorator.
void DecoratorSpriteSheet::ReleaseElementData(Rocket::Core::DecoratorDataHandle ROCKET_UNUSED(element_data))
{
}

// Called to render the decorator on an element.
void DecoratorSpriteSheet::RenderElement(Rocket::Core::Element* element, Rocket::Core::DecoratorDataHandle ROCKET_UNUSED(element_data))
{
    Debug::log() << "DECORATOR SPRITESHEET, rendering element id: " << element->GetId().CString();
    Rocket::Core::Property* imageSourceProperty = element->GetProperty("image-src");

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

    Rocket::Core::Colourb colour = element->GetProperty<Rocket::Core::Colourb>("color");

    glColor4ubv(element->GetProperty<Rocket::Core::Colourb>("color"));
    glBegin(GL_QUADS);

    glVertex2f(position.x, position.y);
    glTexCoord2f(0, 1);

    glVertex2f(position.x, position.y + size.y);
    glTexCoord2f(1, 1);

    glVertex2f(position.x + size.x, position.y + size.y);
    glTexCoord2f(1, 0);

    glVertex2f(position.x + size.x, position.y);
    glTexCoord2f(0, 0);

    glEnd();
    glColor4ub(255, 255, 255, 255);
}
