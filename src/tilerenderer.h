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

class Image;
namespace Entities
{
class Player;
}

class Camera;
class Image;
class World;
class Shader;
class Torch;

class TileRenderer
{
public:
    explicit TileRenderer(World* world, Camera* camera, Entities::Player* mainPlayer);
    ~TileRenderer();

    void render();
    GLuint fboTexture();

    void setCamera(Camera* camera);

    void setRenderingEnabled(bool enabled);
    bool renderingEnabled() {
        return m_renderingEnabled;
    }

private:
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

    void initGL();

    bool m_renderingEnabled = true;

    /// the 3D tilemap texture, whose z texcoord indicates which tile type it is.
    int m_tileSheetCount = 0;
    GLuint m_tileMapTexture;
    GLint m_texture_location;

    GLuint m_spriteSheetTexture = 0;

    GLuint m_fbo = 0;
    GLuint m_fboTexture = 0;
    GLuint m_rb = 0; //render buffer object

    GLuint m_vao; // vertex array object
    GLuint m_vbo; // vertex buffer object
    GLuint m_ebo; // element buffer object

    //FIXME: count max tiles to render
    //HACK: GOD AWFUL HACK, count this dynamically...based on how many fit inside our resolution
    int m_maxTileCount = 8800;

    Camera* m_camera = nullptr;
    Entities::Player* m_mainPlayer = nullptr;

    World* m_world = nullptr;
    Shader* m_shader = nullptr;
};

#endif
