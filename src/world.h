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
#include "chunk.h"

#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL_events.h>
#include <list>
#include <Box2D/Common/b2Math.h>

class ContactListener;
class b2Body;
class b2World;
class LightRenderer;
class Server;
class Client;
class TileRenderer;
class Sky;
class Camera;
class SpriteSheetRenderer;
class CollisionMap;
class Torch;
class Item;
class PhysicsDebugRenderer;

//height
static constexpr unsigned short WORLD_ROWCOUNT = 8400;
//width
static constexpr unsigned short WORLD_COLUMNCOUNT = 2400;

static constexpr double FIXED_TIMESTEP = 1.0 / 60.0; // hertz
static constexpr int32_t VELOCITY_ITERATIONS = 6;
static constexpr int32_t POSITION_ITERATIONS = 2;

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
    /**
     * @p mainEntities::Player If this world is owned by a client, then the mainEntities::Player should point
     * to a player instance which is 'us'/'me'. It is null if we are on a server.
     */
    World(Entities::Player* main, Client* client, Server* server);
    ~World();

    void update(double elapsedTime);
    void render();

    void loadWorld();

    static float pixelsToMeters(float pixels);
    static float metersToPixels(float meters);

    bool isBlockSolid(const glm::vec2& vecDest) const;

    unsigned char blockType(const glm::vec2& vecPoint) const;

    /**
     * Used only by the client for unprojection of mouse coordinates, for example.
     * Remember that the server has no concept of cameras or views.
     */
    Camera* camera() {
        assert(m_camera);
        return m_camera;
    }

    /**
     * Adds the player to the world, but will not take ownership of (you delete it when you're done)
     */
    void addPlayer(Entities::Player* player);

    /**
     * Removes player from the world, but will *NOT* delete it. That's your job, bro
     */
    void removePlayer(Entities::Player* player);
    Entities::Player* findPlayer(uint32_t playerID);

    void itemPrimaryActivated(Entities::Player* player, Item* item);
    void itemSecondaryActivated(Entities::Player* player, Item* item);

    /**
     * An item contained within the quickbar inventory is attempting to be dropped
     * @p player The player that this item belongs to (inventory)
     * @p item pointer to the item to drop
     * @p amount how much of said item to drop, amounts greater than the item's stack size is OK and will be handled.
     */
    void itemQuickBarInventoryDropped(Entities::Player* player, Item* item, uint32_t amount);

    SpriteSheetRenderer* spriteSheetRenderer() {
        return m_spriteSheetRenderer;
    }

    void zoomIn();
    void zoomOut();

    Chunk createChunk(uint32_t startX, uint32_t startY, uint32_t endX, uint32_t endY);
    void loadChunk(Chunk* chunk);

    void spawnItem(Item* item);

    b2World* box2DWorld() {
        return m_box2DWorld;
    }

    //create containers of various entities, and implement a tile system
    //game.cpp calls into this each tick, which this descends downward into each entity
private:
    void attemptItemPlacement(Entities::Player* player);
    void attemptItemPrimaryAttack(Entities::Player* player);

    /**
     * Spawns the static tile physics objects (1 phys object per tile) in some area around
     * the player. Future updates need to call @sa updateTilePhysicsObjects
     * Should be called when a player spawns/teleports to a new area in the world
     */
    void createInitialTilePhysicsObjects(Entities::Player* player);

    /**
     *
     * Similar in nature to createInitialTilePhysicsObjects, but this is meant to update ones near the edges of the screen, aka
     * as the player moves. In other words it creates new ones near the screen and deletes old ones outside of the range of the screen
     * @sa createInitialTilePhysicsObjects
     */
    void updateTilePhysicsObjects(Entities::Player* player);

    void renderCrosshair();

    /**
    * From scratch, create a randomly generated tileset and store it in our array
    */
    void generateWorld();

    /**
    * FIXME: presently only calculates the center of the screen according to resolution.
    * i'm not sure how zooming will be affected with this..i don't *think* it would. but verify
    * if this is ideal or not
    * NOTE: doesn't *actually* use m_camera->getViewport, just a simple Settings::instance()->screenResolutionWidth,H / 2
    */
    glm::vec2 viewportCenter() const;

    void calculateAttackPosition();

    /**
     * Attempts to pick a block at a position. Assumes caller checked inventory to see if it's possible.
     */
    void performBlockAttack(Entities::Player* player);

    /**
     * Calls various functions determining which sort of action should be taken. Should be called on each update()
     * if the player mouse is held.
     */
    void handlePlayerLeftMouse(Entities::Player* player);

    void saveWorld();

    /**
     * Should be called AFTER the world has been fully processed in raw block form.
     * This translates each block type into a meshable tile frame
     */
    void generateTileMeshes();

    /**
     * Looks at @p tileX, @p tileY and looks at 4 sides and 4 corners of it.
     * Returns what is the resulting meshing type that this tile should now have.
     */
    uint8_t calculateTileMeshingType(int tileX, int tileY) const;

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
    bool tileBlendTypeMatch(int sourceTileX, int sourceTileY, int nearbyTileX, int nearbyTileY) const;

    std::vector<Entity*> m_entities;
    std::vector<Torch*> m_torches;

    Sprite* m_blockPickingCrosshair = nullptr;

    // it's easier to manage with a linear array. access is trivial - array[y][x] simply becomes array[y*rowlength + x]
    // [column * WORLD_ROWCOUNT + row]
    std::vector<Block> m_blocks;

    TileRenderer* m_tileRenderer = nullptr;
    LightRenderer* m_lightRenderer = nullptr;
    SpriteSheetRenderer* m_spriteSheetRenderer = nullptr;

    /// inclusive of m_mainEntities::Player as well.
    std::list<Entities::Player*> m_players;

    //HACK: remove when we get beyond just testing stupid shit
    Entity* m_uselessEntity = nullptr;
//    Sky *m_sky = nullptr;

    bool m_mouseLeftHeld = false;

    /**
     * In client window coordinates (relative)
     */
    glm::vec2 m_relativeVectorToAttack;

    /**
     * Null if in server mode. It refers to the client camera
     */
    Camera* m_camera = nullptr;

    b2World* m_box2DWorld = nullptr;
    b2Vec2 m_gravity = b2Vec2(0.0f, 9.8f);
//    b2Vec2 m_gravity = b2Vec2(0.0f, 0.0f);

    ContactListener* m_contactListener = nullptr;

    /**
     * Null if we are in server mode.
     * Else we're in client mode and this is OUR player, the one
     * the client's user is driving.
     */
    Entities::Player* m_mainPlayer = nullptr;

    Server* m_server = nullptr;
    Client* m_client = nullptr;

    int32_t m_blockToAttackX = -1;
    int32_t m_blockToAttackY = -1;

    const float m_zoomInFactor = 1.02;
    const float m_zoomOutFactor = 0.98;

    friend class TileRenderer;
};

#endif
