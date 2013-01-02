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

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <FTGL/ftgl.h>

#include <stdlib.h>
#include <string>
#include <map>

class FontManager
{
public:
    FontManager();
    static FontManager* instance();

    /**
     * Loads the font from disk, or if it has already been loaded,
     * then it just returns the pointer to the existing one.
     *
     * You can call FTGLPixmapFont::Render on it at different positions,
     * as well as different strings to render.
     * 
     * Font is refcounted. Call unloadFont to deref (when no longer used)
     * @sa unloadFont
     */
    FTGLPixmapFont* loadFont(std::string fontPath);

    //TODO: implement :)
    void unloadFont(std::string fontPath);

private:
    ~FontManager();

    struct Font {
        FTGLPixmapFont* font;
        uint refCount;
    };

    std::map<std::string, Font> m_fonts;
};
#endif
