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

#include "src/server/server.h"
#include "src/client/client.h"
#include "item.h"
#include "block.h"
#include "torch.h"
#include "game.h"
#include "camera.h"
#include "tilerenderer.h"
#include "lightrenderer.h"

//HACK #include "sky.h"
#include "settings/settings.h"
#include "quickbarinventory.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <random>
#include <assert.h>
#include <math.h>
#include <fstream>
#include <chrono>

World::World(Player* mainPlayer, Client* client, Server* server)
    : m_mainPlayer(mainPlayer),
      m_client(client),
      m_server(server)
{

    //FIXME:
//    m_player = new Player("someframe");
//    m_entities.insert(m_entities.end(), m_player);

    m_blocks.resize(WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    m_uselessEntity = new Entity("player1Standing1", SpriteSheetRenderer::SpriteSheetType::Character);
    m_uselessEntity->setPosition(2300, 1400);
    m_entities.insert(m_entities.end(), m_uselessEntity);

    if (!m_server) {
        m_tileMapCamera = new Camera();
        m_tileRenderer = new TileRenderer(this, m_tileMapCamera, m_mainPlayer);
        m_tileMapCamera->setPosition(glm::vec2(0.0, 0.0));

        m_camera = new Camera();
        m_spriteSheetRenderer = new SpriteSheetRenderer(m_camera);
        m_spriteSheetRenderer->registerSprite(m_uselessEntity);

        Torch* torch = new Torch(glm::vec2(2400, 1420));
        m_torches.push_back(torch);
        m_spriteSheetRenderer->registerSprite(torch);

        m_blockPickingCrosshair = new Sprite("crosshairPickingActive", SpriteSheetRenderer::SpriteSheetType::Entity);
        m_spriteSheetRenderer->registerSprite(m_blockPickingCrosshair);

        m_lightingCamera = new Camera();
        m_lightRenderer = new LightRenderer(this, m_lightingCamera, m_mainPlayer);
        m_lightRenderer->setTileRendererTexture(m_tileRenderer->fboTexture());

        //FIXME: call each update, and make it only do visible ones
        m_lightRenderer->setTorches(m_torches);

    }

    //client doesn't actually load/generate any world
    if (m_server) {
        loadMap();
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
}

void World::addPlayer(Player* player)
{
    m_players.push_back(player);

    if (!m_server) {
        m_spriteSheetRenderer->registerSprite(player);
    }
}

void World::removePlayer(Player* player)
{
    m_players.remove(player);
}

Player* World::findPlayer(uint32_t playerID)
{
for (auto * player : m_players) {
        if (player->playerID() == playerID) {
            return player;
        }
    }

    Debug::assertf(false, "World::findPlayer, player does not exist? that shit's whack");
}

void World::render(Player* player)
{
    assert(m_mainPlayer && !m_server);

    //Sky at bottom layer

    //TODO render tilemap..

    //set our view so that the player will stay relative to the view, in the center.
    //HACK    m_window->setView(*m_view);
    m_lightRenderer->renderToFBO();

    m_tileRenderer->render();

    //FIXME: incorporate entities into the pre-lit gamescene FBO, then render lighting as last pass
    m_lightRenderer->renderToBackbuffer();

    //HACK    m_window->setView(m_window->getDefaultView());
    m_spriteSheetRenderer->renderEntities();
    m_spriteSheetRenderer->renderCharacters();

    // ==================================================
    glm::ivec2 mouse = mousePosition();

    const float radius = 16.0f;
    const float halfRadius = radius * 0.5;
    const float halfBlockSize = Block::BLOCK_SIZE * 0.5;

    glm::vec2 topLeftLocalCoordinates = glm::vec2(m_mainPlayer->position().x - 1600 / 2, m_mainPlayer->position().y - 900 / 2);
    glm::vec2 transformedMouse = glm::vec2(topLeftLocalCoordinates.x + mouse.x, topLeftLocalCoordinates.y + mouse.y);
    //snap crosshair to the tile grid..the offset is already calculated for us (apparently), so even with per-pixel tilemap scrolling it snaps fine.
    glm::vec2 crosshairPosition(floor(transformedMouse.x / Block::BLOCK_SIZE) * Block::BLOCK_SIZE, floor(transformedMouse.y / Block::BLOCK_SIZE) * Block::BLOCK_SIZE);

    m_blockPickingCrosshair->setPosition(crosshairPosition);
    // ==================================================

    //    ALLEGRO_COLOR color = al_map_rgb(255, 0, 0);
    //   al_draw_rectangle(crosshairPosition.x(), crosshairPosition.y(), crosshairPosition.x() + radius, crosshairPosition.y() + radius, color, 1.0f);

    //    m_sky->render();
}

void World::update(double elapsedTime)
{
    if (m_server) {
    for (auto * player : m_players) {
            if (player->mouseLeftButtonHeld()) {

                handlePlayerLeftMouse(player);
            }
        }
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

for (Player * player : m_players) {
        player->update(elapsedTime, this);

        if (m_server) {
            if (player->dirtyFlags() & Entity::DirtyFlags::PositionDirty) {
                m_server->sendPlayerMove(player);
                player->clearDirtyFlag(Entity::DirtyFlags::PositionDirty);
            }
        }
    }

    if (m_server) {
        float x = m_uselessEntity->position().x;
        float y = m_uselessEntity->position().y;

        if (x > 3200) {
            x = 2200;
        } else {
            x += 1.0f;
        }

        m_uselessEntity->setPosition(x, y);
    }

    //FIXME: MAKE IT CENTER ON THE CENTER OF THE PLAYER SPRITE
    //only occurs on client side, obviously the server doesn't need to do this stuff
    if (m_mainPlayer) {
        m_camera->centerOn(m_mainPlayer->position());
        m_lightingCamera->centerOn(m_mainPlayer->position());
    }

    //calculateAttackPosition();
}

void World::generateTileMeshes()
{
    for (int row; row < WORLD_ROWCOUNT; ++row) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
            if (column - 4 >= 0 && row - 4 >= 0) {
                if (column + 4 <= WORLD_COLUMNCOUNT && row + 4 <= WORLD_ROWCOUNT) {
                    m_blocks[column * WORLD_ROWCOUNT + row].meshType = Block::tileMeshingTable.at(calculateTileMeshingType(column, row));
                }
            }
        }
    }
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

uint8_t World::getBlockType(const glm::vec2& vecPoint) const
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

glm::vec2 World::viewportCenter() const
{
    return glm::vec2(Settings::instance()->screenResolutionWidth * 0.5, Settings::instance()->screenResolutionHeight * 0.5);
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
     *       const float newX = _viewportCenter.x + cos(angle) * Player::blockPickingRadius;
     *       const float newY= _viewportCenter.y  + sin(angle) * Player::blockPickingRadius;
     *       m_relativeVectorToAttack = glm::vec2(newX, newY);
     */
}

glm::ivec2 World::mousePosition() const
{
    int x; int y;
    SDL_GetMouseState(&x, &y);
    return glm::ivec2(x, y);
}

//FIXME: this function needs a lot of help.
//so make it so it doesn't iterate over the whole visible screen but just the blockPickingRadius size.
void World::performBlockAttack(Player* player)
{
    glm::ivec2 mouse = player->mousePosition();

    glm::vec2 center(Settings::instance()->screenResolutionWidth * 0.5, Settings::instance()->screenResolutionHeight * 0.5);

    // if the attempted block pick location is out of range, do nothing.
    if (mouse.x < center.x - Player::blockPickingRadius ||
            mouse.x > center.x + Player::blockPickingRadius ||
            mouse.y < center.y - Player::blockPickingRadius ||
            mouse.y > center.y + Player::blockPickingRadius) {
        return;
    }

    glm::ivec2 transformedMouse = glm::ivec2(floor((mouse.x / 2 + player->position().x) / Block::BLOCK_SIZE), floor((mouse.y / 2 + player->position().y) / Block::BLOCK_SIZE));

    const int radius = Player::blockPickingRadius / Block::BLOCK_SIZE;

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
    Debug::log() << "tilesbeforeX: " << tilesBeforeX << " tilesbeforey: " << tilesBeforeY;

    //FIXME:
    const int startX = ((playerPosition.x - (Settings::instance()->screenResolutionWidth * 0.5) + player->mousePosition().x) / Block::BLOCK_SIZE) - 1; // -1 for alignment with crosshair
    const int endX = startX + 1;
//
    //columns are our X value, rows the Y
    const int startY = ((playerPosition.y - (Settings::instance()->screenResolutionHeight * 0.5) + player->mousePosition().y) / Block::BLOCK_SIZE) - 2; //HACK: -2 for alignment..fuck if i know why it's needed
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

glm::ivec2 World::tileOffset(Player* player) const
{
    const glm::vec2 playerPosition = player->position();
    // to get per-pixel smooth scrolling, we get the remainders and pass it as an offset to things that need to know the tile positions
    const glm::ivec2 ret = glm::ivec2(int(playerPosition.x) & Block::BLOCK_SIZE - 1, int(playerPosition.y) & Block::BLOCK_SIZE - 1);
    return ret;
}

void World::loadMap()
{
    Debug::log(Debug::Area::General) << "loading map!";
    Debug::log(Debug::Area::General) << "SIZEOF Block class: " << sizeof(Block);
    Debug::log(Debug::Area::General) << "SIZEOF m_blocks: " << sizeof(m_blocks) / 1e6 << " MiB";
    generateMap();
}

void World::generateMap()
{
    auto timerStart = std::chrono::high_resolution_clock::now();

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

    auto timerEnd = std::chrono::high_resolution_clock::now();

    auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd - timerStart).count();
    Debug::log(Debug::Area::General) << "Time taken for map generation: " << timeTaken << " Milliseconds";
}

void World::saveMap()
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
    for (int row = chunk->startY(); row < chunk->endY(); ++row) {
        for (int column = chunk->startX(); column < chunk->endX(); ++column) {

            uint32_t index = column * WORLD_ROWCOUNT + row;
            m_blocks.at(index) = chunk->blocks()->at(sourceIndex);
            ++sourceIndex;
        }
    }
}

void World::toggleLightRenderingEnabled()
{
    m_lightRenderer->setRenderingEnabled(!m_lightRenderer->lightRenderingEnabled());
}

Chunk World::createChunk(uint32_t startX, uint32_t startY, uint32_t endX, uint32_t endY)
{
    Chunk chunk(startX, startY, endX, endY, &m_blocks);
    return chunk;
}

void World::zoomIn()
{
    m_camera->zoom(m_zoomInFactor);
    m_tileMapCamera->zoom(m_zoomInFactor);
}

void World::zoomOut()
{
    m_camera->zoom(m_zoomOutFactor);
    m_tileMapCamera->zoom(m_zoomOutFactor);
}

void World::itemQuickBarInventoryDropped(Player* player, Item* item, uint32_t amount)
{
    Item* droppedItem = item->duplicate();
    assert(droppedItem);

    droppedItem->setStackSize(item->dropStack(amount));

    m_entities.insert(m_entities.end(), droppedItem);
}

void World::itemPrimaryActivated(Player* player, Item* item)
{
    if (item->placeable()) {
        // place the item in the world (append to entity list)
        //TODO: if it's dynamic_cast a Torch, then put it in torch list

    }

    item->activatePrimary();
}

void World::itemSecondaryActivated(Player* player, Item* item)
{
    item->activateSecondary();
}

void World::handlePlayerLeftMouse(Player* player)
{
    //TODO: HANDLE INVENTORY AND TAKE THAT INTO ACCOUNT
    // performBlockAttack(player);

    // FIXME: HACK: perform the action based on what type of thing is equipped.
    // if it's a sword we attack shit, if it's a pickaxe we attack blocks. for now, lets
    // just try to place the equipped object in the world
    QuickBarInventory* inventory = player->quickBarInventory();
    Item* item = inventory->item(inventory->equippedIndex());

    //these items are placeable
    if (item->placeable()) {
        attemptItemPlacement(player);
    }
}

void World::attemptItemPlacement(Player* player)
{
    QuickBarInventory* inventory = player->quickBarInventory();
    Item* item = inventory->item(inventory->equippedIndex());

    switch (item->type()) {
        case Item::ItemType::Torch:
            Torch* torch = dynamic_cast<Torch*>(item);
            Debug::log(Debug::Area::NetworkServer) << "TORCH RADIUS: " << torch->radius();
            break;
    }
}

void World::attemptItemPrimaryAttack(Player* player)
{

}
