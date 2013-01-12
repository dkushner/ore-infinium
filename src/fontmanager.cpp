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

#include "fontmanager.h"
#include "debug.h"

#include <map>
#include <iostream>

static FontManager* s_instance = 0;

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
}

FontManager* FontManager::instance()
{
    if (!s_instance) {
        s_instance = new FontManager();
    }

    return s_instance;
}

FTPixmapFont* FontManager::loadFont(std::string fontPath)
{
    std::map <std::string, FontManager::Font>::iterator it = m_fonts.find(fontPath);

    if (it != m_fonts.end() && fontPath.compare(it->first) == 0) {
        it->second.refCount++;
        Debug::log(Debug::Area::System) << "font already loaded, increasing font refcount for fontPath: " << fontPath << " now at refcount: " << it->second.refCount;
        return it->second.font;
    }

    FTGLPixmapFont *pixmap = new FTGLPixmapFont(fontPath.c_str());
    Debug::fatal(!pixmap->Error(), Debug::Area::System, "Failure to load font at path" + fontPath);

    Font font;
    font.font = pixmap;
    font.refCount = 1;

    std::pair<std::string, Font> pair;
    pair.first = fontPath;
    pair.second = font;
    m_fonts.insert(pair);

    return pixmap;
}

void FontManager::unloadFont(std::string fontPath)
{

}
