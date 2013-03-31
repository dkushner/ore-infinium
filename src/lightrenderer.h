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

#ifndef LIGHTRENDERER_H
#define LIGHTRENDERER_H

#include "debug.h"
#include "block.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>
#include <GL/glew.h>

#include <map>
#include <string>
#include <vector>

namespace Entities
{
class Player;
}

class Camera;
class Image;
class World;
class Shader;
class Torch;

class LightRenderer
{
public:
    explicit LightRenderer(World* world, Camera* camera, Entities::Player* mainPlayer);
    ~LightRenderer();

    void renderToFBO();
    void renderToBackbuffer();

    void setCamera(Camera* camera);

    void setTorches(std::vector<Torch*>* torches);
    void setTileRendererTexture(GLuint texture);

    /**
     * Enables or disables lighting rendering. When disabled, it just
     * goes straight to the passthrough shader and combines a blank FBO (white)
     * with the scene, so technically also decreases rendering complexity.
     */
    void setRenderingEnabled(bool enabled) {
        m_renderingEnabled = enabled;
    }
    bool lightRenderingEnabled() {
        return m_renderingEnabled;
    }

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

    void initGL();
    void initBackbufferGL();

    std::vector<Torch*> *m_torches;

    // GLuint m_tileMapTexture;

    // framebuffer, which we first render our lightmaps to, then we render it to the backbuffer
    GLuint m_fbo = 0;
    GLuint m_fboTexture = 0;
    GLuint m_torchLightTexture = 0;

    GLuint m_tileRendererTexture = 0;

    GLuint m_vaoBackbuffer; // vertex array object
    GLuint m_vboBackbuffer; // vertex buffer object
    GLuint m_eboBackbuffer; // element buffer object

    GLuint m_rb = 0; //render buffer object

    GLuint m_vao; // vertex array object
    GLuint m_vbo; // vertex buffer object
    GLuint m_ebo; // element buffer object

    glm::mat4 m_modelMatrix;
    glm::mat4 m_projectionMatrix;

    //FIXME: count max tiles to render
    int m_maxTileCount = 8200;

    bool m_renderingEnabled = true;

    Camera* m_camera = nullptr;
    Entities::Player* m_mainPlayer = nullptr;

    World* m_world = nullptr;
    Shader* m_shader = nullptr;
    Shader* m_shaderPassthrough = nullptr;
};

#endif
