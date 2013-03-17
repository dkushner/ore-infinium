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

#ifndef ROCKETCOREDECORATORTILEDHORIZONTAL_H
#define ROCKETCOREDECORATORTILEDHORIZONTAL_H

//#include "RocDecoratorTiled.h"
#include <Rocket/Core/Decorator.h>

namespace Rocket {
namespace Core {

/**
	@author Peter Curry
 */

class DecoratorTiledHorizontal : public DecoratorTiled
{
public:
	DecoratorTiledHorizontal();
	virtual ~DecoratorTiledHorizontal();

	/// Initialises the tiles for the decorator.
	/// @param tiles[in] The declaration for all three tiles.
	/// @param texture_names[in] The application-specific path to the texture for the three tiles.
	/// @param rcss_paths[in] The paths to the RCSS files that defined the texture sources.
	/// @return True if all the images loaded (or are pending loading) and are of compatible sizes, false otherwise.
	bool Initialise(const Tile* tiles, const String* texture_names, const String* rcss_paths);

	/// Called on a decorator to generate any required per-element data for a newly decorated element.
	virtual DecoratorDataHandle GenerateElementData(Element* element);
	/// Called to release element data generated by this decorator.
	virtual void ReleaseElementData(DecoratorDataHandle element_data);

	/// Called to render the decorator on an element.
	virtual void RenderElement(Element* element, DecoratorDataHandle element_data);

private:
	enum
	{
		LEFT = 0,
		RIGHT = 1,
		CENTRE = 2
	};

	Tile tiles[3];
};

}
}

#endif
