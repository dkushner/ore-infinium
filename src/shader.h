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

#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/gl.h>

class Shader
{
public:
    explicit Shader(const char* vertexShader, const char* fragmentShader);
    ~Shader();

    //FIXME: for now only. maybe later i'll turn it into a manager which allows binding and holds a list of shaders
    //to do so properly would require some thought though, specifically in cases where i'd want to bind a certain frag and certain vert shader.
    /**
     * Loads given vertex, fragment shaders, compiles, links and checks them, returns program handle.
     * FIXME: HACK: this actually leaks, because it doesn't return a handle to the frag/vert shaders
     * so you can never delete them. Not a biggy right now, it can only happen at destruction anyways..
     * and we only have 1 shader program at the moment. This will be remedied when we make this into an
     * actual manager of shader programs
     */
    GLuint loadShaders(const char* vertexShader, const char* fragmentShader);

private:
    void printShaderInfoLog(GLuint shader);
    bool checkShaderCompileStatus(GLuint obj);
    bool checkProgramLinkStatus(GLuint obj);
    char* loadFile(const char* fname, GLint* fSize);
};

#endif
