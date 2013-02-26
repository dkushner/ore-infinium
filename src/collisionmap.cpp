/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 * Code based off of Aidiakapi's code, http://nevereal.net/viewtopic.php?id=10&st=4#reply71
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

#include "collisionmap.h"

#include "entity.h"

CollisionMap::CollisionMap(unsigned int width, unsigned int height, unsigned int cellWidth, unsigned int cellHeight)
    :   m_width(width),
        m_height(height),
        m_cellWidth(cellWidth),
        m_cellHeight(cellHeight),
        m_totalWidth(width * cellWidth),
        m_totalHeight(height * cellHeight),
        m_nextEntryIndex(0)
{
    m_map = new GridNode**[m_width];

    for (unsigned int x = 0; x < m_width; x++) {
        m_map[x] = new GridNode*[m_height];
        for (unsigned int y = 0; y < m_height; y++) {
            m_map[x][y] = nullptr;
        }
    }
}

CollisionMap::~CollisionMap() {
    for (unsigned int x = 0; x < m_width; x++) {
        for (unsigned int y = 0; y < m_height; y++) {
            GridNode* next = m_map[x][y];
            GridNode* current;

            while (next != nullptr) {
                current = next;
                next = next->next;
                delete current;
            }

        }
        delete[] m_map[x];
    }
    delete[] m_map;

    /*
    for (unsigned int i = 0; i < m_nextEntryIndex; i++) {
        entry = m_entities[i];
        delete entry;
    }
    */
}

void CollisionMap::Add(Entity* entity) {
    unsigned int index = m_nextEntryIndex++;
    entity->m_index = index;

    m_entities.push_back(entity);

    addToGrid(entity);
}

void CollisionMap::Remove(Entity* entity) {
    removeFromGrid(entity);
    m_entities[entity->m_index] = nullptr;
}

void CollisionMap::Change(Entity* entity) {
//
//    unsigned int minX, maxX;
//
//    unsigned int oleft = rect.left / m_cellWidth;
//    unsigned int oright = (rect.left + rect.width) / m_cellWidth;
//    unsigned int otop = rect.top / m_cellHeight;
//    unsigned int obottom = (rect.top + rect.height) / m_cellHeight;
//
//    unsigned int nleft = newRect.left / m_cellWidth;
//    unsigned int nright = (newRect.left + newRect.width) / m_cellWidth;
//    unsigned int ntop = newRect.top / m_cellHeight;
//    unsigned int nbottom = (newRect.top + newRect.height) / m_cellHeight;
//
//    if (oleft < nleft) // Trim the left side
//        for (unsigned int x = oleft; x < nleft; x++)
//            for (unsigned int y = otop; y <= obottom; y++)
//                removeEntryFromCell(x, y, entry);
//
//    if (oright > nright) // Trim the right side
//        for (unsigned int x = nright + 1; x <= oright; x++)
//            for (unsigned int y = otop; y <= obottom; y++)
//                removeEntryFromCell(x, y, entry);
//
//    minX = nleft > oleft ? nleft : oleft;
//    maxX = nright < oright ? nright : oright;
//    if (otop < ntop) // Trim the top side
//        for (unsigned int x = minX; x <= maxX; x++)
//            for (unsigned int y = otop; y < ntop; y++)
//                removeEntryFromCell(x, y, entry);
//
//    if (obottom > nbottom) // Trim the bottom side
//        for (unsigned int x = minX; x <= maxX; x++)
//            for (unsigned int y = nbottom + 1; y <= obottom; y++)
//                removeEntryFromCell(x, y, entry);
//
//    if (nleft < oleft) // Expand the left side
//        for (unsigned int x = nleft; x < oleft; x++)
//            for (unsigned int y = ntop; y <= nbottom; y++)
//                addEntryToCell(x, y, entry);
//
//    if (nright > oright) // Expand the right side
//        for (unsigned int x = oright + 1; x <= nright; x++)
//            for (unsigned int y = ntop; y <= nbottom; y++)
//                addEntryToCell(x, y, entry);
//
//    if (ntop < otop) // Expand the top side
//        for (unsigned int x = minX; x <= maxX; x++)
//            for (unsigned int y = ntop; y < otop; y++)
//                addEntryToCell(x, y, entry);
//
//    if (nbottom > obottom) // Expand the bottom side
//        for (unsigned int x = minX; x <= maxX; x++)
//            for (unsigned int y = obottom + 1; y <= nbottom; y++)
//                addEntryToCell(x, y, entry);
//
//    entry->_region = newRect;

    // This is the quick and dirty alternative:
    // removeFromGrid(entry);
    // entry->_region = newRect;
    // addToGrid(entry);

    Remove(entity);
    Add(entity);
}

inline void CollisionMap::addEntryToCell(unsigned int x, unsigned int y, Entity* entity) {
    GridNode* node = new GridNode;
    node->entity = entity;
    node->next = m_map[x][y];
    m_map[x][y] = node;
}

void CollisionMap::addToGrid(Entity* entity) {
    const glm::vec2& position = entity->position();
    const glm::vec2& size = entity->size();

    unsigned int left = position.x / m_cellWidth;
    unsigned int right = (position.x + size.x) / m_cellWidth;
    unsigned int top = position.y / m_cellHeight;
    unsigned int bottom = (position.y + size.y) / m_cellHeight;

    // adds the entry to several cells, however many the position + size takes up.
    for (unsigned int x = left; x <= right; x++) {
        for (unsigned int y = top; y <= bottom; y++) {
            addEntryToCell(x, y, entity);
        }
    }
}

inline void CollisionMap::removeEntryFromCell(unsigned int x, unsigned int y, Entity* entity) {
    GridNode* parent = nullptr;
    GridNode* current = m_map[x][y];

    while (current != nullptr) {
        if (current->entity == entity) {
            if (parent != nullptr) {
                parent->next = current->next;
                m_map[x][y] = parent;
            }
            else {
                m_map[x][y] = current->next;
            }

            current = nullptr;
        } else {
            parent = current;
        }

        current = parent->next;
    }
}

void CollisionMap::removeFromGrid(Entity* entity) {
    const glm::vec2& position = entity->position();
    const glm::vec2& size = entity->size();

    unsigned int left = position.x / m_cellWidth;
    unsigned int right = (position.x + size.x) / m_cellWidth;
    unsigned int top = position.y / m_cellHeight;
    unsigned int bottom = (position.y + size.y) / m_cellHeight;

    for (unsigned int x = left; x <= right; x++) {
        for (unsigned int y = top; y <= bottom; y++) {
            removeEntryFromCell(x, y, entity);
        }
    }
}

/*
sf::Texture CollisionMap::RenderDebugTexture() {
    if (_tex == nullptr) {
        _tex = new sf::RenderTexture();
        _tex->create(_totalWidth, _totalHeight);
    }

    sf::RectangleShape rect((sf::Vector2f)sf::Vector2i(_cellWidth, _cellHeight));
    sf::Color gridCellCol1(255, 0, 0, 100), gridCellCol2(0, 255, 0, 100);
    bool usingColor1;

    _tex->clear(sf::Color::Transparent);

    // Draw the grid
    for (unsigned int x = 0; x < _width; x++) {
        usingColor1 = x % 2 == 0;
        for (unsigned int y = 0; y < _height; y++) {
            rect.setPosition((sf::Vector2f)sf::Vector2i(x * _cellWidth, y * _cellHeight));
            rect.setFillColor(usingColor1 ? gridCellCol1 : gridCellCol2);
            _tex->draw(rect);
            usingColor1 = !usingColor1;
        }
    }

    rect.setFillColor(sf::Color(128, 128, 128, 160));
    CollisionEntry* entryPtr;
    sf::Rect<unsigned int> region;
    for (unsigned int i = 0; i < _nextEntryIndex; i++) {
        entryPtr = _entries[i];
        if (entryPtr == nullptr) continue;
        region = entryPtr->_region;
        rect.setPosition((sf::Vector2f)sf::Vector2i(region.left, region.top));
        rect.setSize((sf::Vector2f)sf::Vector2i(region.width, region.height));
        _tex->draw(rect);
    }

    _tex->display();

    return _tex->getTexture();
}
*/

//FIXME: impleament when i feel like doing entity->entity collision..
bool CollisionMap::collidesAt(const glm::vec2& position) {
    if (position.x < 0 || position.y < 0) {
        return false;
    }

    return CheckCollision(position);
}

bool CollisionMap::CheckCollision(const glm::vec2& position) {
    /*
    if (position.x >= m_totalWidth || position.y >= m_totalHeight) return false;

    GridNode* node = m_map[position.x / m_cellWidth][position.y / m_cellHeight];
    sf::Rect<unsigned int> region;
    while (node != nullptr) {
        region = node->_entry->_region;
        if (position.x >= region.left && position.y >= region.top &&
                position.x < region.left + region.width &&
                position.y < region.top + region.height)
            return true;
        node = node->_next;
    }

    */
    return false;
}
