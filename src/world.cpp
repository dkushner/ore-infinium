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
#include "block.h"
#include "torch.h"
#include "game.h"
#include "camera.h"
#include "tilerenderer.h"
#include "lightrenderer.h"

//HACK #include "sky.h"
#include "settings/settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <random>
#include <assert.h>
#include <math.h>
#include <fstream>

World::World(Player* mainPlayer, Server* server)
    : m_mainPlayer(mainPlayer),
      m_server(server)
{

    //FIXME:
//    m_player = new Player("someframe");
//    m_entities.insert(m_entities.end(), m_player);

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

        m_lightingCamera = new Camera();
        m_lightRenderer = new LightRenderer(this, m_lightingCamera, m_mainPlayer);
        m_lightRenderer->setTileRendererTexture(m_tileRenderer->fboTexture());

        //FIXME: call each update, and make it only do visible ones
        m_lightRenderer->setTorches(m_torches);
    }

    loadMap();

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
    const float halfBlockSize = Block::blockSize * 0.5;

    const glm::ivec2 offset = tileOffset(player);

    // NOTE: (Settings::instance()->screenResolutionHeight % Block::blockSize) is what we add so that it is aligned properly to the tile grid, even though the screen is not evenly divisible by such.
    glm::vec2 crosshairPosition(mouse.x - mouse.x % Block::blockSize + (Settings::instance()->screenResolutionWidth % Block::blockSize) - offset.x + Block::blockSize,
                                mouse.y - mouse.y % Block::blockSize + (Settings::instance()->screenResolutionHeight % Block::blockSize) - offset.y + Block::blockSize);

    //    ALLEGRO_COLOR color = al_map_rgb(255, 0, 0);
    //   al_draw_rectangle(crosshairPosition.x(), crosshairPosition.y(), crosshairPosition.x() + radius, crosshairPosition.y() + radius, color, 1.0f);
    // ==================================================
    //    m_sky->render();
}

void World::handleEvent(const SDL_Event& event)
{
    //FIXME:!!!! unused, we just pass events to the player..among other children (currently just player though)
    //here, the inventory ui and other stuff may need to be factored in. who knows.
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN: {
        if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(MouseButton::Left)) {
            m_mouseLeftHeld = true;
        }
        break;
    }

    case SDL_MOUSEBUTTONUP: {
        if (!SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(MouseButton::Left)) {
            m_mouseLeftHeld = false;
        }
        break;
    }
    }

//    m_player->handleEvent(event);
}

void World::update(double elapsedTime)
{
    //FIXME:
//    if (m_mouseLeftHeld) {
//        performBlockAttack();
//    }

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

glm::ivec2 World::mousePosition() const
{
    int x; int y;
    SDL_GetMouseState(&x, &y);
    return glm::ivec2(x, y);
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

unsigned char World::calculateTileMeshingType(int tileX, int tileY) const
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
    const unsigned char result = (1 * topRight) + (2 * bottomRight) + (4 * bottomLeft) + (8 * topLeft) + (16 * top) + (32 * right) + (64 * bottom) + (128 * left);
    return result;
}

bool World::isBlockSolid(const glm::vec2& vecDest) const
{
    const int column = int(std::ceil(vecDest.x) / Block::blockSize);
    const int row = int(std::ceil(vecDest.y) / Block::blockSize);

    int index = column * WORLD_ROWCOUNT + row;
    assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    const unsigned char blockType = World::m_blocks[index].primitiveType;

    //FIXME: do water, lava, doors..what else?
    return  blockType != 0;
}

unsigned char World::getBlockType(const glm::vec2& vecPoint) const
{
    const int column = int(std::ceil(vecPoint.x) / Block::blockSize);
    const int row = int(std::ceil(vecPoint.y) / Block::blockSize);

    int index = column * WORLD_ROWCOUNT + row;
    assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    const unsigned char blockType = World::m_blocks[index].primitiveType;

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

//FIXME: this function needs a lot of help.
//so make it so it doesn't iterate over the whole visible screen but just the blockPickingRadius size.
void World::performBlockAttack(Player* player)
{
    /*
     *   const glm::vec2 viewCenter = m_view->getCenter();
     *
     *   glm::vec2 viewPosition;
     *   //    std::cout << "viewportcenter" << " viewportcenter y: " << viewportCenter().y << " view->getcenter() y: " << viewCenter.y << "\n";
     *   viewPosition.x = viewCenter.x - viewportCenter().x;
     *   viewPosition.y = viewCenter.y - viewportCenter().y;
     *   const int column = int((m_relativeVectorToAttack.x + viewPosition.x) / Block::blockSize);
     *   const int row = int((m_relativeVectorToAttack.y + viewPosition.y) / Block::blockSize);
     *   //    std::cout << "relativevector y: " << m_relativeVectorToAttack.y << " view position y: " << viewPosition.y << "\n";
     *
     *   const int startRow = (m_player->position().y / Block::blockSize) - radius;
     *   const int startColumn = (m_player->position().x / Block::blockSize) - radius;
     *   const int endRow = (m_player->position().y / Block::blockSize) + radius;
     *   const int endColumn = (m_player->position().x / Block::blockSize) + radius;
     */

    glm::ivec2 mouse = mousePosition();

    //FIXME: eventually will need to make this go to the players center
    // can we divide player pos by half of screen h/w ?
    glm::vec2 center(Settings::instance()->screenResolutionWidth * 0.5, Settings::instance()->screenResolutionHeight * 0.5);

    // if the attempted block pick location is out of range, do nothing.
    if (mouse.x < center.x - Player::blockPickingRadius ||
            mouse.x > center.x + Player::blockPickingRadius ||
            mouse.y < center.y - Player::blockPickingRadius ||
            mouse.y > center.y + Player::blockPickingRadius) {
        return;
    }

    mouse.x /= int(Block::blockSize);
    mouse.y /= int(Block::blockSize);

    const int radius = Player::blockPickingRadius / Block::blockSize;

    int attackX = 0 ; //HACK= mouse.x() + (m_view->getCenter().x() - Settings::instance()->screenResolutionWidth * 0.5) / Block::blockSize;
    int attackY = 0; //HACK= mouse.y() + (m_view->getCenter().y() - Settings::instance()->screenResolutionHeight * 0.5) / Block::blockSize;

    const glm::vec2 playerPosition = player->position();

    //consider block map as starting at player pos == 0,0 and going down and to the right-ward
    //tilesBefore{X,Y} is only at the center of the view though..find the whole screen real estate
    // which is why startRow etc add and subtract half the screen
    //column
    int tilesBeforeX = playerPosition.x / Block::blockSize;
    //row
    int tilesBeforeY = playerPosition.y / Block::blockSize;

    const int startRow = tilesBeforeY - ((Settings::instance()->screenResolutionHeight * 0.5) / Block::blockSize);
    const int endRow = tilesBeforeY + ((Settings::instance()->screenResolutionHeight * 0.5) / Block::blockSize);

    //columns are our X value, rows the Y
    const int startColumn = tilesBeforeX - ((Settings::instance()->screenResolutionWidth * 0.5) / Block::blockSize);
    const int endColumn = tilesBeforeX + ((Settings::instance()->screenResolutionWidth * 0.5) / Block::blockSize);

    int index = 0;

    for (int row = startRow; row < endRow; ++row) {
        for (int column = startColumn; column < endColumn; ++column) {
            if (row == attackY && column == attackX) {
                index = column * WORLD_ROWCOUNT + row;
                assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);
                World::m_blocks[index].primitiveType = 0;
                return;
            }
        }
    }

    std::cout << "ERROR: " << " no block found to attack?" << "\n";
}
/*
 * FIXME: USELESS, UNNEEDED, GET RID OF THAT SHIT
 v *oid World::generatePixelTileMap()
 {
     const glm::vec2 playerPosition = m_player->position();
     //consider block map as starting at player pos == 0,0 and going down and to the right-ward
     //tilesBefore{X,Y} is only at the center of the view though..find the whole screen real estate
     //column
     int tilesBeforeX = playerPosition.x() / Block::blockSize;
     //row
     int tilesBeforeY = playerPosition.y() / Block::blockSize;

     //FIXME: only calculate this crap when we move/change tiles
     // -1 so that we render an additional row and column..to smoothly scroll
     const int startRow = tilesBeforeY - ((Settings::instance()->screenResolutionHeight * 0.5) / Block::blockSize) - 1;
     const int endRow = tilesBeforeY + ((Settings::instance()->screenResolutionHeight * 0.5) / Block::blockSize);

     //columns are our X value, rows the Y
     const int startColumn = tilesBeforeX - ((Settings::instance()->screenResolutionWidth * 0.5) / Block::blockSize) - 1;
     const int endColumn = tilesBeforeX + ((Settings::instance()->screenResolutionWidth * 0.5) / Block::blockSize);

     if (std::abs(startColumn) != startColumn) {
         std::cout << "FIXME, WENT INTO NEGATIVE COLUMN!!";
         assert(0);
     } else if (std::abs(startRow) != startRow) {
         std::cout << "FIXME, WENT INTO NEGATIVE ROW!!";
         assert(0);
     }

     // only make it as big as we need it, remember this is a pixel representation of the visible
     // tile map, with the red channel identifying what type of tile it is
     // x is columns..since they move from left to right, rows start at top and move to bottom
     // (and yes..i confused this fact before, leaving a headache here ;)
     m_tileMapPixelsTexture = al_create_bitmap(endColumn - startColumn, endRow - startRow);

     al_lock_bitmap(m_tileMapPixelsTexture, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);

     al_set_target_bitmap(m_tileMapPixelsTexture);

     int x = 0;
     int y = 0;

     // [y*rowlength + x]
     for (int currentRow = startRow; currentRow < endRow; ++currentRow) {
         for (int currentColumn = startColumn; currentColumn < endColumn; ++currentColumn) {

             const int index = currentColumn * WORLD_ROWCOUNT + currentRow;
             assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

             ALLEGRO_COLOR color = al_map_rgb(m_blocks[index].primitiveType, 0, 0);

             //FIXME: possibly use al_lock_bitmap and use ALLEGRO_LOCKED_REGION
             al_put_pixel(x, y, color);
             ++x;
         }
         ++y;
         x = 0;
     }

     al_unlock_bitmap(m_tileMapPixelsTexture);

     al_save_bitmap("pixelmap.png", m_tileMapPixelsTexture);

     al_set_target_backbuffer(m_display);

     //FIXME: hugely fucking expensive..fix the above loops so we *generate* it upside down
     // or...change the shader to calculate it properly
     //HACK:  m_tileMapPixelsImage.flipVertically();

     //TODO: al_get_shader_log here?
     //    Debug::fatal(al_set_shader_sampler(m_shader, "tilemap_pixels", m_tileMapPixelsTexture, 0), Debug::Area::Graphics, "shader tilemap_pixels set failure");

     // to get per-pixel smooth scrolling, we get the remainders and pass it as an offset to the shader
     float floatArray[2] = { tileOffset().x(), tileOffset().y() };
     //    Debug::fatal(al_set_shader_float_vector(m_shader, "offset", 2, floatArray, 2), Debug::Area::Graphics, "shader offset set failure");
     }
     */

glm::ivec2 World::tileOffset(Player* player) const
{
    const glm::vec2 playerPosition = player->position();
    // to get per-pixel smooth scrolling, we get the remainders and pass it as an offset to things that need to know the tile positions
    const glm::ivec2 ret = glm::ivec2(int(playerPosition.x) & Block::blockSize - 1, int(playerPosition.y) & Block::blockSize - 1);
    return ret;
}

void World::loadMap()
{
    std::cout << "loading map!" << std::endl;
    std::cout << "SIZEOF m_blocks: " << sizeof(m_blocks) / 1e6 << " MiB" << std::endl;
    generateMap();

    //FIXME:
//    m_player->setPosition(100, 200);
}

void World::generateMap()
{
    //FIXME: use std::chrono
//    const double startTime = SDL_GetTicks();

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

//    const double elapsedTime = SDL_GetTicks() - startTime;
//   Debug::log(Debug::Area::General) << "Time taken for map generation: " << elapsedTime << " Milliseconds";
}

void World::saveMap()
{
    //FIXME: use binary methods only, no more pixel saving/png saving/loading
    /*
     *   std::cout << "saving map!" << std::endl;
     *   sf::Image image;
     *   image.create(WORLD_ROWCOUNT, WORLD_COLUMNCOUNT, sf::Color::White);
     *
     *   sf::Color color(0, 0, 0, 255);
     *   sf::Clock clock;
     *
     *   for (int row = 0; row < WORLD_ROWCOUNT; ++row) {
     *       for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
     *           color.r = m_blocks[column * WORLD_ROWCOUNT + row].primitiveType;
     *           image.setPixel(row, column, color);
     }
     }

     bool saved = image.saveToFile("levelsave.png");
     assert(saved);

     const int elapsedTime = clock.getElapsedTime().asMilliseconds();
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

void World::toggleLightRenderingEnabled()
{
    m_lightRenderer->setRenderingEnabled(!m_lightRenderer->lightRenderingEnabled());
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
