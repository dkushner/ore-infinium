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

#ifndef TILERENDERER_H
#define TILERENDERER_H

#include "debug.h"
#include "block.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>
#include <GL/glew.h>

#include <map>
#include <string>
#include <vector>

class Player;
class Camera;
class Image;
class World;
class Shader;

class TileRenderer
{
public:
    explicit TileRenderer(World* world, Camera* camera, Player* mainPlayer);
    ~TileRenderer();

    void render();

    void setCamera(Camera* camera);

private:
    typedef uint32_t u32;
    typedef float f32;

    /* Each vertex is:
     * two floats for the 2d coordinate
     * four u8s for the color
     * two f32s for the texcoords
     * the vbo contains data of the aforementioned elements interleaved.
     * Each sprite has four vertices.
     * */
    struct Vertex {
        float x, y;
        unsigned int color; // packed with 4 u8s (unsigned chars) for color
        float u, v, w;
    };

    //FIXME: don't use NPOT
    const GLsizei TILESHEET_WIDTH = 170;
    const GLsizei TILESHEET_HEIGHT = 253;

    void loadTileSheets();
    void loadTileSheet(const std::string& fileName, Block::BlockType type);

    void initGL();

//    std::map<SpriteSheetType, SpriteSheet> m_spriteSheetTextures;

    /**
     * Map containing all the sprite frame names and their properties for this
     * particular spritesheet. e.g. x, y, width, height.
     */
//    std::map<std::string, SpriteFrameIdentifier> m_spriteSheetCharactersDescription;
    std::map<Block::BlockType, Image*> m_tileSheets;

    /// the 3D tilemap texture, whose z texcoord indicates which tile type it is.
    int m_tileSheetCount = 0;
    GLuint m_tileMapTexture;
    GLint m_texture_location;

    GLuint m_vao; // vertex array object
    GLuint m_vbo; // vertex buffer object
    GLuint m_ebo; // element buffer object

    glm::mat4 m_modelMatrix;
    glm::mat4 m_projectionMatrix;

    int m_tileCount = 0;
    int m_maxSpriteCount = 20200;

    Camera* m_camera = nullptr;
    Player* m_mainPlayer = nullptr;

    World* m_world = nullptr;
    Shader* m_shader = nullptr;
};

#endif
