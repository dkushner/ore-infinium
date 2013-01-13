/******************************************************************************
 *   Copyright (C) 2012 by Shaun Reich <sreich@kde.org>                       *
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

#ifndef WORLD_H
#define WORLD_H

#include "block.h"
#include "player.h"

#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL_events.h>

class Sky;
class Camera;

//height
static constexpr unsigned short WORLD_ROWCOUNT = 8400;
//width
static constexpr unsigned short WORLD_COLUMNCOUNT = 2400;

/*
 e.g. [ ] [ ] [ ] [ ] [ ]  ... 8400
        [ ] [ ] [ ] [ ] [ ]  ... 8400
        ...
        ...
        ...
        2400
*/
class World
{
public:
    World(Camera* camera);

    static World* instance();
    static void createInstance(Camera *_camera);

    void update(const float elapsedTime);
    void render();

    void loadMap();

    void handleEvent(const SDL_Event& event);

    bool isBlockSolid(const glm::vec2& vecDest) const;

    char getBlockType(const glm::vec2& vecPoint) const;

    //create containers of various entities, and implement a tile system
    //game.cpp calls into this each tick, which this descends downward into each entity
private:

    ~World();

    void printShaderLog(GLuint shader);
    std::string loadShaderSource(const std::string& filename);

    /**
    * From scratch, create a randomly generated tileset and store it in our array
    */
    void generateMap();

    /**
    * FIXME: presently only calculates the center of the screen according to resolution.
    * i'm not sure how zooming will be affected with this..i don't *think* it would. but verify
    * if this is ideal or not
    * NOTE: doesn't *actually* use m_view->getViewport, just a simple SCREEN_W,H / 2
    */
    glm::vec2 viewportCenter() const;
    void calculateAttackPosition();
    void generatePixelTileMap();
    void performBlockAttack();
    void saveMap();

    glm::vec2 tileOffset() const;

    /**
     * Should be called AFTER the world has been fully processed in raw block form.
     * This translates each block type into a meshable tile frame
     */
    void generateTileMeshes();

    /**
     * Looks at @p tileX, @p tileY and looks at 4 sides and 4 corners of it.
     * Returns what is the resulting meshing type that this tile should now have.
     */
    unsigned char calculateTileMeshingType(const int tileX, const int tileY) const;

    /**
     * Decides whether or not a blend type of the tile at the source position matches a tile at a different position.
     *
     * Used for seeing if the tile we are looking at has any nearby ones of similar blendtypes.
     *
     * Presently, blend types are only (FIXME/TODO) applicable to tiles of same type. Need to expand to e.g.
     * blend stone and copper/gold together well.
     *
     * @p sourceTileX expected to be a position divided by Block::blockSize, aka 16, and already offset
     */
    bool tileBlendTypeMatch(const int sourceTileX, const int sourceTileY, const int nearbyTileX, const int nearbyTileY) const;

    std::vector<Entity*> m_entities;

    // it's faster and easier to manage with a linear array. access is trivial - array[y][x] simply becomes array[y*rowlength + x]
    // makes sure that the memory allocated is in fact contiguous.
    // [column * WORLD_ROWCOUNT + row]
    Block m_blocks[WORLD_ROWCOUNT * WORLD_COLUMNCOUNT];

    Player *m_player = nullptr;

//    Sky *m_sky = nullptr;

    //FIXME: just a ptr to the game.cpp one :(  same with window
//FIXME:    sf::View *m_view = nullptr;

    bool m_mouseLeftHeld = false;

    /**
     * In client window coordinates (relative)
     */
    glm::vec2 m_relativeVectorToAttack;
};

#endif
