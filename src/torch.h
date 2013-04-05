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

#ifndef TORCH_H
#define TORCH_H

#include "item.h"

#include <vector>
#include <string>

/**
 */
class Torch : public Item
{
public:
    Torch(const glm::vec2& position);
    ~Torch();

    /**
     * NOOP: torches do nothing other than be placed.
     */
    virtual void activatePrimary();

    /**
     * NOOP: torches do nothing other than be placed.
     */
    virtual void activateSecondary();

    virtual Item* duplicate();

    float radius() {
        return m_radius;
    }
    void setRadius(float radius) {
        m_radius = radius;
    }

private:
    /// meters
    float m_radius = 10.0f;
};

#endif
