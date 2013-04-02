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

#include "shader.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>

#include "debug.h"
#include "game.h"
#include "settings/settings.h"

Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
    loadShaders(vertexShader, fragmentShader);

    if (Settings::instance()->debugOutput) {
        printShaderInfoLog(m_vertexShader);
        printShaderInfoLog(m_fragmentShader);
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderProgram);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
}

void Shader::bindProgram() const
{
    glUseProgram(m_shaderProgram);
}

void Shader::unbindProgram() const
{
    glUseProgram(0);
}

GLuint Shader::shaderProgram() const
{
    return m_shaderProgram;
}

GLuint Shader::vertexShader() const
{
    return m_vertexShader;
}

GLuint Shader::fragmentShader() const
{
    return m_fragmentShader;
}

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
char* Shader::loadFile(const char* fname, GLint* fSize)
{
    std::ifstream::pos_type size;
    char * memblock = 0;
    std::string text;

    // file read based on example in cplusplus.com tutorial
    std::ifstream file(fname, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        *fSize = (GLuint) size;

        memblock = new char [size];
        file.seekg(0, std::ios::beg);
        file.read(memblock, size);
        file.close();
        text.assign(memblock);

        Debug::log(Debug::Area::ShadersArea) << "shader : " << fname << " loaded successfully";
    } else {
        Debug::fatal(false,  Debug::Area::ShadersArea, "failed to load shader: " + std::string(fname));
    }
    return memblock;
}

void Shader::loadShaders(const char* vertexShader, const char* fragmentShader)
{
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

    GLint vertLength;
    GLint fragLength;

    char* vertSource;
    char* fragSource;

    vertSource = loadFile(vertexShader, &vertLength);
    fragSource = loadFile(fragmentShader, &fragLength);

    const char* vertSourceConst = vertSource;
    const char* fragSourceConst = fragSource;

    glShaderSource(m_vertexShader, 1, &vertSourceConst, &vertLength);
    glCompileShader(m_vertexShader);

    if (!checkShaderCompileStatus(m_vertexShader)) {
        assert(0);
        Debug::assertf(false, "vertex shader failed to compile properly");
    } else {
        Debug::log(Debug::Area::ShadersArea) << "vertex shader compiled!";
    }

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &fragSourceConst, &fragLength);
    glCompileShader(m_fragmentShader);

    if (!checkShaderCompileStatus(m_fragmentShader)) {
        Debug::assertf(false, "fragment shader failed to compile properly");
    } else {
        Debug::log(Debug::Area::ShadersArea) << "fragment shader compiled!";
    }

    m_shaderProgram = glCreateProgram();

    // attach shaders
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);

    glBindFragDataLocation(m_shaderProgram, 0, "fragColor");

    // link the program and check for errors
    glLinkProgram(m_shaderProgram);

    if (checkProgramLinkStatus(m_shaderProgram)) {
        Debug::log(Debug::Area::ShadersArea) << "shader program linked!";
    } else {
        Debug::fatal(false, Debug::Area::ShadersArea, "shader program link FAILURE");
    }

    delete [] vertSource;
    delete [] fragSource;
}

bool Shader::checkShaderCompileStatus(GLuint obj)
{
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length;

        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);

        Debug::log(Debug::Area::ShadersArea) << &log[0];
        return false;
    }
    return true;
}

bool Shader::checkProgramLinkStatus(GLuint obj)
{
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length;

        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> log(length);

        glGetProgramInfoLog(obj, length, &length, &log[0]);

        Debug::log(Debug::Area::Graphics) << &log[0];
        return false;
    }
    return true;
}

void Shader::printShaderInfoLog(GLuint shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);

        Debug::log(Debug::Area::ShadersArea) << "Shader info log: " << infoLog;

        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}
