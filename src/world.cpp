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

#include "world.h"
#include "debug.h"

#include "globals.h"

#include "src/server/server.h"
#include "src/server/contactlistener.h"

#include "src/client/client.h"

#include "item.h"
#include "block.h"
#include "torch.h"
#include "game.h"
#include "camera.h"
#include "tilerenderer.h"
#include "lightrenderer.h"
#include "physicsdebugrenderer.h"

//HACK #include "sky.h"
#include "settings/settings.h"
#include "quickbarinventory.h"
#include "timer.h"

#include <Box2D/Box2D.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <random>
#include <assert.h>
#include <math.h>
#include <fstream>
#include <chrono>

World::World(Entities::Player* mainPlayer, Client* client, Server* server)
    : m_mainPlayer(mainPlayer),
      m_server(server),
      m_client(client)
{
    //FIXME:
//    m_player = new Entities::Player("someframe");
//    m_entities.insert(m_entities.end(), m_player);

    m_blocks.resize(WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    m_uselessEntity = new Entity("player1Standing1", SpriteSheetRenderer::SpriteSheetType::Character);
    m_uselessEntity->setPosition(2300 / PIXELS_PER_METER, 1400 / PIXELS_PER_METER);
    m_entities.insert(m_entities.end(), m_uselessEntity);

    if (!m_server) {

        m_camera = new Camera();
        m_spriteSheetRenderer = new SpriteSheetRenderer(m_camera);
//FIXME:        m_spriteSheetRenderer->registerSprite(m_uselessEntity);

        m_tileRenderer = new TileRenderer(this, m_camera, m_mainPlayer);

        Torch* torch = new Torch(glm::vec2(2400 / PIXELS_PER_METER, 1420 / PIXELS_PER_METER));
        m_torches.push_back(torch);
        m_spriteSheetRenderer->registerSprite(torch);

        m_blockPickingCrosshair = new Sprite("crosshairPickingActive", SpriteSheetRenderer::SpriteSheetType::Entity);
        m_spriteSheetRenderer->registerSprite(m_blockPickingCrosshair);

        m_lightRenderer = new LightRenderer(this, m_camera, m_mainPlayer);
        m_lightRenderer->setTileRendererTexture(m_tileRenderer->fboTexture());

        //FIXME: call each update, and make it only do visible ones
        m_lightRenderer->setTorches(&m_torches);
    }

    //client doesn't actually load/generate any world
    if (m_server) {
        m_box2DWorld = new b2World(m_gravity);
        m_box2DWorld->SetAllowSleeping(true);

        m_contactListener = new ContactListener();
        m_box2DWorld->SetContactListener(m_contactListener);

        /*
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(pixelsToMeters(0.0f), pixelsToMeters(2000.0f));//pixelsToMeters(1000));

        b2Body* groundBody = m_box2DWorld->CreateBody(&groundBodyDef);

        b2PolygonShape groundBox;
        const float groundHeight = 2200.0f;
        const float groundWidth = 2200.0f;
//        groundBox.SetAsBox(pixelsToMeters(groundWidth / 2.0f), pixelsToMeters(groundHeight / 2.0f));
        groundBox.SetAsBox(pixelsToMeters(2500), pixelsToMeters(50));

        groundBody->CreateFixture(&groundBox, 0.0f);
        */

        if (m_server->client()) {
            m_server->client()->setBox2DWorld(m_box2DWorld);
        }

        loadWorld();
        Debug::log(Debug::WorldLoaderArea) << "World is x: " << (WORLD_COLUMNCOUNT * Block::BLOCK_SIZE) << " y: " << (WORLD_ROWCOUNT * Block::BLOCK_SIZE) << " meters big";
    }

    //FIXME: saveMap();

    //FIXME: height
    //    m_sky = new Sky(m_window, m_view, 0.0f);
    //FIXME: HACK: rendering is borked because of this, server will shit bricks when it sees this, because it needs to run this code
}

World::~World()
{
    delete m_tileRenderer;
    delete m_spriteSheetRenderer;
    //    delete m_sky;

    delete m_box2DWorld;
}

void World::addPlayer(Entities::Player* player)
{

    m_players.push_back(player);

    if (m_server) {
        Debug::log(Debug::Area::ServerEntityCreationArea) << "Adding player to world. Position X :"  << player->position().x << " Y : " << player->position().y;

        const glm::vec2& playerPosition = player->position();

        // destroy an area around the player and where he spawns (blocks) so that he can collide properly with the tiles.

        //FIXME: HACK: this needs improvement. obviously..otherwise it could very easily destroy everything underneath wherever the player left off.
        //clear an area around the player's rect, of tiles, so he can spawn properly.
        const int startX = ((playerPosition.x) / Block::BLOCK_SIZE) - (10);
        const int endX = startX + (20);

        //columns are our X value, rows the Y
        const int startY = ((playerPosition.y) / Block::BLOCK_SIZE) - (10);
        const int endY = startY + (20);
        int index = 0;

        for (int row = startY; row < endY; ++row) {
            for (int column = startX; column < endX; ++column) {
                index = column * WORLD_ROWCOUNT + row;
                assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);
                Block& block = m_blocks[index];
                block.primitiveType = Block::BlockType::Null;
            }
        }

        createInitialTilePhysicsObjects(player);

        //NOTE: you might be asking, why don't we send a chunk? that's because this happens as soon as the client is validated and its
        // player is created. therefore the next calls will be sending player info, and then sending the initial world chunk at this player's position.

    } else if (!m_server) {
        m_spriteSheetRenderer->registerSprite(player);
    }
}

void World::removePlayer(Entities::Player* player)
{
    m_players.remove(player);
}

void World::createInitialTilePhysicsObjects(Entities::Player* player)
{
    glm::vec2 position = player->position();

    float blockSize = Block::BLOCK_SIZE;
    int centerTileX = int((position.x / blockSize));
    int centerTileY = int((position.y / blockSize));

    //tile indexes
     //FIXME: HACK obviously
    int startRow = centerTileY - 20;
    int endRow = startRow + 60;
    int startColumn = centerTileX - 50;
    int endColumn = startColumn + 100;

    int count = 0;
    int index = 0;
    for (int currentRow = startRow; currentRow < endRow; ++currentRow) {
        for (int currentColumn = startColumn; currentColumn < endColumn; ++currentColumn) {
            count++;

            index = currentColumn * WORLD_ROWCOUNT + currentRow;
            assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);
            Block& block = m_blocks[index];

            if ( Block::blockTypeMap.at(block.primitiveType).collides == false) {
                //skip over tiles which are not marked as collideable types. obviously, no physics bodies need to be generated for such cases.
                continue;
            }

            b2Body* body = nullptr;

            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position.Set(Block::BLOCK_SIZE * float(currentColumn) + (Block::BLOCK_SIZE * 0.5f), Block::BLOCK_SIZE * float(currentRow) + (Block::BLOCK_SIZE * 0.5f));

            body = m_box2DWorld->CreateBody(&bodyDef);

            ContactListener::BodyUserData* userData = new ContactListener::BodyUserData();
            userData->type = ContactListener::BodyType::Block;
            //userData->data = m_blocks ...FIXME
            body->SetUserData(userData);
            b2PolygonShape box;
            box.SetAsBox(Block::BLOCK_SIZE * 0.5f , Block::BLOCK_SIZE * 0.5f);

            // create main body's fixture
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &box;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 1.0f;

            body->CreateFixture(&fixtureDef);
        }
    }

    Debug::log(Debug::ServerEntityCreationArea) << "Created initial tile physics objects for player, current world body count: " << m_box2DWorld->GetBodyCount() << " count: " << count;
}

void World::updateTilePhysicsObjects(Entities::Player* player)
{

}

Entities::Player* World::findPlayer(uint32_t playerID)
{
    for (auto * player : m_players) {
        if (player->playerID() == playerID) {
            return player;
        }
    }

    Debug::assertf(false, "World::findPlayer, player does not exist? that shit's whack");
    return nullptr;
}

void World::render()
{
    assert(m_mainPlayer && !m_server);

    m_lightRenderer->setRenderingEnabled(Settings::instance()->debugRendererFlags & Debug::RenderingDebug::LightRenderingPassDebug);
    m_tileRenderer->setRenderingEnabled(Settings::instance()->debugRendererFlags & Debug::RenderingDebug::TileRenderingPassDebug);

    //Sky at bottom layer

    //TODO render tilemap..

    //set our view so that the player will stay relative to the view, in the center.
    //HACK    m_window->setView(*m_view);
   m_lightRenderer->renderToFBO();

    //    m_sky->render();
    m_tileRenderer->render();

    //FIXME: incorporate entities into the pre-lit gamescene FBO, then render lighting as last pass
    m_lightRenderer->renderToBackbuffer();

    //HACK    m_window->setView(m_window->getDefaultView());
    m_spriteSheetRenderer->renderEntities();
    m_spriteSheetRenderer->renderCharacters();

    renderCrosshair();
}

void World::renderCrosshair()
{
    glm::vec2 mouse = m_mainPlayer->mousePositionWorldCoords();

    glm::vec2 crosshairPosition = glm::vec2(Block::BLOCK_SIZE * floor(mouse.x / Block::BLOCK_SIZE), Block::BLOCK_SIZE * floor(mouse.y / Block::BLOCK_SIZE));
    glm::vec2 crosshairOriginOffset = glm::vec2(m_blockPickingCrosshair->sizeMeters().x * 0.5f, m_blockPickingCrosshair->sizeMeters().y * 0.5f);
    glm::vec2 crosshairFinalPosition = glm::vec2(crosshairPosition.x + crosshairOriginOffset.x, crosshairPosition.y + crosshairOriginOffset.y);

    m_blockPickingCrosshair->setPosition(crosshairFinalPosition);
}

float World::metersToPixels(float meters)
{
    return meters * PIXELS_PER_METER;
}

float World::pixelsToMeters(float pixels)
{
    return pixels / PIXELS_PER_METER;
}

void World::update(double elapsedTime)
{
    //FIXME: MAKE IT CENTER ON THE CENTER OF THE PLAYER SPRITE
    //only occurs on client side, obviously the server doesn't need to do this stuff

    if (m_server) {
        for (auto * player : m_players) {
            if (player->mouseLeftButtonHeld()) {

                handlePlayerLeftMouse(player);
            }
        }
    }

    if (m_server) {
        m_box2DWorld->Step(FIXED_TIMESTEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    }

    //    m_sky->update(elapsedTime);

    //NOTE: players are not exactly considered entities. they are, but they aren't
    for (Entity * currentEntity : m_entities) {
        currentEntity->update(elapsedTime, this);
        if (m_server) {
            if (currentEntity->dirtyFlags() & Entity::DirtyFlags::PositionDirty) {
//                m_server->sendPlayerMove(player);
                currentEntity->clearDirtyFlag(Entity::DirtyFlags::PositionDirty);
            }
        }
    }

    for (Entities::Player * player : m_players) {
        player->update(elapsedTime, this);

        if (m_server) {
            if (player->dirtyFlags() & Entity::DirtyFlags::PositionDirty) {
                m_server->sendPlayerMove(player);
                player->clearDirtyFlag(Entity::DirtyFlags::PositionDirty);
                Debug::log(Debug::ServerEntityLogicArea) << "Player position, x: " << player->position().x << " y: " << player->position().y;
            }
        }
    }

    if (m_mainPlayer) {
        m_camera->centerOn(m_mainPlayer->position());
    }

    //calculateAttackPosition();
}

void World::generateTileMeshes()
{
    /*
    for (int row = 0; row < WORLD_ROWCOUNT; ++row) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
            if (column - 4 >= 0 && row - 4 >= 0) {
                if (column + 4 <= WORLD_COLUMNCOUNT && row + 4 <= WORLD_ROWCOUNT) {
                    m_blocks[column * WORLD_ROWCOUNT + row].meshType = Block::tileMeshingTable.at(calculateTileMeshingType(column, row));
                }
            }
        }
    }
    */
}

uint8_t World::calculateTileMeshingType(int tileX, int tileY) const
{
    const bool left = tileBlendTypeMatch(tileX, tileY, tileX - 1, tileY);
    const bool right = tileBlendTypeMatch(tileX, tileY, tileX + 1, tileY);

    const bool top = tileBlendTypeMatch(tileX, tileY, tileX, tileY - 1);
    const bool bottom = tileBlendTypeMatch(tileX, tileY, tileX, tileY + 1);

    const bool topLeft = tileBlendTypeMatch(tileX, tileY, tileX - 1, tileY - 1);
    const bool bottomLeft = tileBlendTypeMatch(tileX, tileY, tileX - 1, tileY + 1);

    const bool topRight = tileBlendTypeMatch(tileX, tileY, tileX + 1, tileY - 1);
    const bool bottomRight = tileBlendTypeMatch(tileX, tileY, tileX + 1, tileY + 1);

    // behold the motherfucking magic
    const uint8_t result = (1 * topRight) + (2 * bottomRight) + (4 * bottomLeft) + (8 * topLeft) + (16 * top) + (32 * right) + (64 * bottom) + (128 * left);
    return result;
}

bool World::isBlockSolid(const glm::vec2& vecDest) const
{
    const int column = int(std::ceil(vecDest.x) / Block::BLOCK_SIZE);
    const int row = int(std::ceil(vecDest.y) / Block::BLOCK_SIZE);

    int index = column * WORLD_ROWCOUNT + row;
    assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    const uint8_t blockType = World::m_blocks[index].primitiveType;

    //FIXME: do water, lava, doors..what else?
    return  blockType != 0;
}

uint8_t World::blockType(const glm::vec2& vecPoint) const
{
    const int column = int(std::ceil(vecPoint.x) / Block::BLOCK_SIZE);
    const int row = int(std::ceil(vecPoint.y) / Block::BLOCK_SIZE);

    int index = column * WORLD_ROWCOUNT + row;
    assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    const uint8_t blockType = World::m_blocks[index].primitiveType;

    return  blockType;
}

bool World::tileBlendTypeMatch(int sourceTileX, int sourceTileY, int nearbyTileX, int nearbyTileY) const
{
    bool isMatched = false;
    const int srcIndex = sourceTileX * WORLD_ROWCOUNT + sourceTileY;
    const int nearbyIndex = nearbyTileX * WORLD_ROWCOUNT + nearbyTileY;

    if (m_blocks[srcIndex].primitiveType == m_blocks[nearbyIndex].primitiveType) {
        isMatched = true;
    }

    //TODO future, use blending types..not just tile type is same

    return isMatched;
}

//FIXME: unused..will be used for shooting and such. not for block breaking.
void World::calculateAttackPosition()
{
    /*    const glm::vec2 _viewportCenter = viewportCenter();
     *
     *       const sf::Vector2i mousePos = sf::Mouse::position(*m_window);
     *
     *       glm::vec2 diffVect;
     *       diffVect.x = mousePos.x - _viewportCenter.x;
     *       diffVect.y = mousePos.y - _viewportCenter.y;
     *
     *       const double angle = atan2(diffVect.y, diffVect.x);
     *       const float newX = _viewportCenter.x + cos(angle) * Entities::Player::blockPickingRadius;
     *       const float newY= _viewportCenter.y  + sin(angle) * Entities::Player::blockPickingRadius;
     *       m_relativeVectorToAttack = glm::vec2(newX, newY);
     */
}

//FIXME: this function needs a lot of help.
//so make it so it doesn't iterate over the whole visible screen but just the blockPickingRadius size.
void World::performBlockAttack(Entities::Player* player)
{
    glm::vec2 mouse = player->mousePositionWorldCoords();

    glm::vec2 center(Settings::instance()->screenResolutionWidth * 0.5, Settings::instance()->screenResolutionHeight * 0.5);

    // if the attempted block pick location is out of range, do nothing.
    if (mouse.x < center.x - Entities::Player::blockPickingRadius ||
            mouse.x > center.x + Entities::Player::blockPickingRadius ||
            mouse.y < center.y - Entities::Player::blockPickingRadius ||
            mouse.y > center.y + Entities::Player::blockPickingRadius) {
        return;
    }

    glm::ivec2 transformedMouse = glm::ivec2(floor((mouse.x / 2 + player->position().x) / Block::BLOCK_SIZE), floor((mouse.y / 2 + player->position().y) / Block::BLOCK_SIZE));

    const int radius = Entities::Player::blockPickingRadius / Block::BLOCK_SIZE;

    int attackX = transformedMouse.x;
    int attackY = transformedMouse.y;

    const glm::vec2 playerPosition = player->position();

    //consider block map as starting at player pos == 0,0 and going down and to the right-ward
    //tilesBefore{X,Y} is only at the center of the view though..find the whole screen real estate
    // which is why startRow etc add and subtract half the screen
    //column
    int tilesBeforeX = playerPosition.x / Block::BLOCK_SIZE;
    //row
    int tilesBeforeY = playerPosition.y / Block::BLOCK_SIZE;
    Debug::log(Debug::Area::ServerEntityLogicArea) << "performBlockAttack, tilesbeforeX: " << tilesBeforeX << " tilesbeforey: " << tilesBeforeY;

    //FIXME:
    const int startX = ((playerPosition.x - (Settings::instance()->screenResolutionWidth * 0.5) + player->mousePositionWorldCoords().x) / Block::BLOCK_SIZE) - 1; // -1 for alignment with crosshair
    const int endX = startX + 1;
//
    //columns are our X value, rows the Y
    const int startY = ((playerPosition.y - (Settings::instance()->screenResolutionHeight * 0.5) + player->mousePositionWorldCoords().y) / Block::BLOCK_SIZE) - 2; //HACK: -2 for alignment..fuck if i know why it's needed
    const int endY = startY + 1;
    int index = 0;

    bool blocksModified = false;
    for (int row = startY; row < endY; ++row) {
        for (int column = startX; column < endX; ++column) {
//            if (row == attackY && column == attackX) {
            index = column * WORLD_ROWCOUNT + row;
            assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);
            Block& block = m_blocks[index];
            if (block.primitiveType != 0) {
                //FIXME: decrement health..
                block.primitiveType = 0; //FIXME:
                blocksModified = true;
            }
        }
    }

    Chunk chunk(startX, startY, endX, endY, &m_blocks);

    if (blocksModified) {
        m_server->sendWorldChunk(&chunk);
    }
}

void World::loadWorld()
{
    Debug::log(Debug::Area::WorldLoaderArea) << "Loading world!";
    Debug::log(Debug::Area::WorldLoaderArea) << "SIZEOF Block class: " << sizeof(Block);
    Debug::log(Debug::Area::WorldLoaderArea) << "SIZEOF m_blocks: " << sizeof(m_blocks) / 1e6 << " MiB";
    generateWorld();
}

void World::generateWorld()
{
    Debug::log(Debug::Area::WorldGeneratorArea) << "Generating a new world.";

    Timer timer;

    std::random_device device;
    std::mt19937 rand(device());
    //FIXME: convert to 1, n
    std::uniform_int_distribution<> distribution(0, 3);

    int lastRow = 0;

    // 200 rows of "sky"
    for (; lastRow < 15; ++lastRow) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
            m_blocks[column * WORLD_ROWCOUNT + lastRow].primitiveType = 0;
        }
    }

    for (; lastRow < WORLD_ROWCOUNT; ++lastRow) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
            m_blocks[column * WORLD_ROWCOUNT + lastRow].primitiveType = distribution(rand);
        }
    }

    generateTileMeshes();

    Debug::log(Debug::Area::WorldGeneratorArea) << "Time taken for world generation: " << timer.milliseconds() << " milliseconds";
}

void World::saveWorld()
{
    /*

     std::cout << "Time taken for map saving: " << elapsedTime << " Milliseconds" << std::endl;
     */
    /*
    std::ofstream file("TESTWORLDDATA");

    int index = 0;
    for (int row = 0; row < WORLD_ROWCOUNT; ++row) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
            index = column * WORLD_ROWCOUNT + row;
            Block* block = &m_blocks[index];
            file.write((char*)(block), sizeof(Block));
        }
    }
    file.close();
    */
}

void World::loadChunk(Chunk* chunk)
{
    int sourceIndex = 0;

    Debug::log(Debug::NetworkClientContinuousArea) << "Loading Chunk START Y: " << chunk->startY() << " ENDY: " << chunk->endY() << " STARTX: " <<  chunk->startX() << " ENDX: " << chunk->endX();

    for (int row = chunk->startY(); row < chunk->endY(); ++row) {
        for (int column = chunk->startX(); column < chunk->endX(); ++column) {

            uint32_t index = column * WORLD_ROWCOUNT + row;
            m_blocks.at(index) = chunk->blocks()->at(sourceIndex);
            ++sourceIndex;
        }
    }
}

Chunk World::createChunk(uint32_t startX, uint32_t startY, uint32_t endX, uint32_t endY)
{
    Chunk chunk(startX, startY, endX, endY, &m_blocks);
    return chunk;
}

void World::zoomIn()
{
    m_camera->zoom(m_zoomInFactor);
}

void World::zoomOut()
{
    m_camera->zoom(m_zoomOutFactor);
}

void World::itemQuickBarInventoryDropped(Entities::Player* player, Item* item, uint32_t amount)
{
    Item* droppedItem = item->duplicate();
    assert(droppedItem);

    droppedItem->setStackSize(item->dropStack(amount));

    m_entities.insert(m_entities.end(), droppedItem);
}

void World::itemPrimaryActivated(Entities::Player* player, Item* item)
{
    if (item->placeable()) {
        // place the item in the world (append to entity list)
        //TODO: if it's dynamic_cast a Torch, then put it in torch list

    }

    item->activatePrimary();
}

void World::itemSecondaryActivated(Entities::Player* player, Item* item)
{
    item->activateSecondary();
}

void World::handlePlayerLeftMouse(Entities::Player* player)
{
    //TODO: HANDLE INVENTORY AND TAKE THAT INTO ACCOUNT
    // performBlockAttack(player);

    // FIXME: HACK: perform the action based on what type of thing is equipped.
    // if it's a sword we attack shit, if it's a pickaxe we attack blocks. for now, lets
    // just try to place the equipped object in the world
    QuickBarInventory* inventory = player->quickBarInventory();
    Item* item = inventory->item(inventory->equippedIndex());

    //these items are placeable
    if (item != nullptr && item->placeable()) {
        attemptItemPlacement(player);
    }
}

void World::attemptItemPlacement(Entities::Player* player)
{
    QuickBarInventory* inventory = player->quickBarInventory();
    Item* item = inventory->item(inventory->equippedIndex());

    if (item == nullptr) {
        return;
    }

    if (item->stackSize() == 0) {
        Debug::log(Debug::ServerEntityLogicArea) << "server: well that's odd, was told that we should place an item, but the item is valid/hanging around, but has no stack size..so it's a count of 0...shouldn't happen.";
        return;
    }

    if (player->canPlaceItem() == false) {
        // FIXME: has an arbitrary delay between item placement timings, i'm not sure if this is even needed..maybe.
        // but as of right now it's definitely needed since there's no placement rules, so it will blow through a whole stack in a fraction fo a second.,
        // all placed at the same position.
        return;
    }

    //use player's placement timing and such.
    player->placeItem();

    item->setState(Item::ItemState::Placed);

    glm::vec2 position = glm::vec2(player->mousePositionWorldCoords().x, player->mousePositionWorldCoords().y);
    item->setPosition(position);

    switch (item->type()) {
    case Item::ItemType::Torch: {
        Torch* torch = dynamic_cast<Torch*>(item);

        torch->dropStack(1);

        Torch* newTorch = dynamic_cast<Torch*>(torch->duplicate());
        newTorch->setStackSize(1);
        m_torches.push_back(newTorch);

        //send the new inventory item count to this player's client.
        m_server->sendQuickBarInventoryItemCountChanged(player, inventory->equippedIndex(), torch->stackSize());
        m_server->sendItemSpawned(newTorch);

        if (torch->stackSize() == 0) {
            //remove it from *our* inventory. the client has already done so.
            player->quickBarInventory()->deleteItem(inventory->equippedIndex());
        }
        break;
    }
    }
}

void World::attemptItemPrimaryAttack(Entities::Player* player)
{

}

void World::spawnItem(Item* item)
{
    switch (item->type()) {
    case Item::ItemType::Torch: {
        Torch* torch = dynamic_cast<Torch*>(item);

        //FIXME: HACK;
        m_torches.push_back(torch);
        break;
    }

    case Item::ItemType::Block: {

        break;
    }
    }

    m_spriteSheetRenderer->registerSprite(item);
}
