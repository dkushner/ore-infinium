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

#ifndef CHUNK_H
#define CHUNK_H

#include <map>
#include <string>
#include <vector>
#include "block.h"

///this class is pretty much a kludge from the beginning, but my hope is that by using it
///i will have a nice measure of preventing or solving some implementation leakage..
class Chunk
{
public:
    Chunk(uint32_t startX, uint32_t startY, uint32_t endX, uint32_t endY, std::vector<Block> *blocks);
    ~Chunk();

    uint32_t startX() {
        return m_startX;
    }
    uint32_t endX() {
        return m_endX;
    }

    uint32_t startY() {
        return m_startY;
    }
    uint32_t endY() {
        return m_endY;
    }

    std::vector<Block>* blocks() {
        return m_blocks;
    }

private:
    std::vector<Block> *m_blocks = nullptr;

    /// left-most x value (column)
    uint32_t m_startX = 0;
    /// top-most y value (row)
    uint32_t m_startY = 0;

    /// right-most x value (column)
    uint32_t m_endX = 0;
    /// bottom-most y value (row)
    uint32_t m_endY = 0;
};

#endif
