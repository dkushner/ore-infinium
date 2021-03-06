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

#include "decoratorspritesheetinstancer.h"
#include "decoratorspritesheet.h"
#include <src/debug.h>

#include <Rocket/Core/Math.h>
#include <Rocket/Core/String.h>

DecoratorSpriteSheetInstancer::DecoratorSpriteSheetInstancer()
{
    RegisterProperty("image-src", "").AddParser("string");
    RegisterProperty("image-x1", "0").AddParser("number");
    RegisterProperty("image-x2", "50").AddParser("number");
    RegisterProperty("image-y1", "0").AddParser("number");
    RegisterProperty("image-y2", "50").AddParser("number");
}

DecoratorSpriteSheetInstancer::~DecoratorSpriteSheetInstancer()
{
}

// Instances a decorator given the property tag and attributes from the RCSS file.
Rocket::Core::Decorator* DecoratorSpriteSheetInstancer::InstanceDecorator(const Rocket::Core::String& ROCKET_UNUSED(name), const Rocket::Core::PropertyDictionary& properties)
{
    DecoratorSpriteSheet* decorator = new DecoratorSpriteSheet();
    if (decorator->Initialise()) {
        return decorator;
    }

    decorator->RemoveReference();
    ReleaseDecorator(decorator);

    return NULL;
}

// Releases the given decorator.
void DecoratorSpriteSheetInstancer::ReleaseDecorator(Rocket::Core::Decorator* decorator)
{
    delete decorator;
}

// Releases the instancer.
void DecoratorSpriteSheetInstancer::Release()
{
    delete this;
}
