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
#ifndef COLLISIONMAP_H
#define COLLISIONMAP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Entity;

class CollisionMap {
public:

    CollisionMap(unsigned int width, unsigned int height, unsigned int cellWidth, unsigned int cellHeight);
    ~CollisionMap();

    void Add(Entity* entity);
    void Change(Entity* entity);
    void Remove(Entity* entity);

    bool collidesAt(const glm::vec2& position);
    bool CheckCollision(const glm::vec2& position);

private:
    class GridNode {
        friend class CollisionMap;
        Entity* entity;
        GridNode* next;
    };

    unsigned int m_width, m_height, m_cellWidth, m_cellHeight, m_totalWidth, m_totalHeight;
    unsigned int m_nextEntryIndex;
    std::vector<Entity*> m_entities;
    GridNode*** m_map;

    void addToGrid(Entity* entity);
    void removeFromGrid(Entity* entity);
    inline void addEntryToCell(unsigned int x, unsigned int y, Entity* entity);
    inline void removeEntryFromCell(unsigned int x, unsigned int y, Entity* entity);
};

#endif
