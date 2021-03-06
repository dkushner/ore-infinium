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

#include "image.h"

#include "debug.h"

#include <FreeImage.h>
#include <sys/stat.h>

Image::Image(const std::string& fileName)
{
    loadImage(fileName);
}

Image::~Image()
{
    FreeImage_Unload(m_bitmap);
}

uint32_t Image::width() const
{
    return m_width;
}

uint32_t Image::height() const
{
    return m_height;
}

void Image::loadImage(const std::string& filename)
{
    FREE_IMAGE_FORMAT imageFormat = FIF_UNKNOWN;

    Debug::log(Debug::Area::ImageLoaderArea) << "Loading image: " << filename << " ... ";

    struct stat fileAttribute;
    bool fileExists = stat(filename.c_str(), &fileAttribute) == 0;

    Debug::fatal(fileExists, Debug::Area::ImageLoaderArea, "image file failed to load, file does not exist. Filename: " + filename);

    imageFormat = FreeImage_GetFileType(filename.c_str());


    //if still unknown, try to guess the file format from the file extension
    if (imageFormat == FIF_UNKNOWN) {
        imageFormat = FreeImage_GetFIFFromFilename(filename.c_str());
    }

    //still unknown, we're boned.
    if (imageFormat == FIF_UNKNOWN) {
        Debug::fatal(false, Debug::Area::ImageLoaderArea, "failure to load image, type unknown");
    }

    //check that the plugin has reading capabilities for this file and load the file
    if (FreeImage_FIFSupportsReading(imageFormat)) {
        m_bitmap = FreeImage_Load(imageFormat, filename.c_str());
    }

    Debug::fatal(m_bitmap, Debug::Area::ImageLoaderArea, "failure to load image, bitmap pointer invalid");

    m_width = FreeImage_GetWidth(m_bitmap);
    m_height = FreeImage_GetHeight(m_bitmap);

    if (m_width == 0 || m_height == 0) {
        Debug::fatal(false, Debug::Area::ImageLoaderArea, "failure to load image, bitmap sizes invalid or bits invalid");
    }
}

void Image::flipVertically()
{
    FreeImage_FlipVertical(m_bitmap);
}

BYTE* Image::bytes()
{
    Debug::fatal(m_bitmap, Debug::Area::ImageLoaderArea, "bitmap invalid!");
    return FreeImage_GetBits(m_bitmap);
}
