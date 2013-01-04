/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *   Based on original code from Ben English <benjamin.english@oit.edu>       *
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

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <FreeImage.h>

#include <GL/gl.h>

#include <map>
#include <string>

class TextureManager
{
public:
    static TextureManager* instance();
    virtual ~TextureManager();

    /**
     * Load an image as an OpenGL texture and make it the current texture
     * If @p texID is already in use, it will be unloaded and replaced with this texture
     *
     * @p filename path relative or absolute including extension to the image to load
     * @p texID the GLint handle to the resulting texture
     * @p image_format which format the image on disk is in. Default is GL_BGRA
     * @p internal_format the format to tell OpenGL how it should store it. Default GL_RGBA
     * @p level what mipmapping level to utilize. Default 0.
     * @p border border size. Default 0
     */
    void loadTexture(std::string filename, GLuint texID, GLenum image_format = GL_BGRA, GLint internal_format = GL_RGBA, GLint level = 0, GLint border = 0);

    /**
     * free the memory for a texture
     */
    bool unloadTexture(GLuint texID);

    /**
     * free all texture memory
     */
    void unloadAllTextures();
    //set the current texture
    bool bindTexture(GLuint texID);

private:
    TextureManager();
    TextureManager(const TextureManager& tm);
    TextureManager& operator=(const TextureManager& tm);

    static TextureManager* s_instance;
    std::map<unsigned int, GLuint> m_texID;
};

#endif
