/*
    btexture.h

    Basic class for using textures
    Copyright (C) 2001-2024  W. Schwotzer

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef BTEXTURE_H
#define BTEXTURE_H

#include "stdafx.h"
#include <fstream>
#include <string>
#include <png.h>


enum tTextureFormat
{
    TEX_WITH_ALPHA         = 0x01,

    TEX_MODIFY_ALPHA       = 0x0C,
    TEX_USE_RED_FOR_ALPHA  = 0x04,
    TEX_USE_GREEN_FOR_ALPHA = 0x08,
    TEX_USE_BLUE_FOR_ALPHA = 0x0C,

    TEX_COPY_COLOR         = 0x70,
    TEX_COPY_RED_TO_GREEN  = 0x10,
    TEX_COPY_RED_TO_BLUE   = 0x20,
    TEX_COPY_GREEN_TO_RED  = 0x30,
    TEX_COPY_GREEN_TO_BLUE = 0x40,
    TEX_COPY_BLUE_TO_RED   = 0x50,
    TEX_COPY_BLUE_TO_GREEN = 0x60,

    TEX_SET_COLOR_RED      = 0x100,
    TEX_SET_COLOR_GREEN    = 0x200,
    TEX_SET_COLOR_BLUE     = 0x400,
    TEX_SET_COLORS         = (TEX_SET_COLOR_RED | TEX_SET_COLOR_GREEN |
                              TEX_SET_COLOR_BLUE),

    TEX_ILLUMINANCE        = 0x800,
    TEX_RGB                = 0x1000,
    TEX_ILLUMINANCE_ALPHA  = (TEX_ILLUMINANCE | TEX_WITH_ALPHA),
    TEX_RGB_ALPHA          = (TEX_RGB         | TEX_WITH_ALPHA),

    TEX_LARGER             = 0x4000,
    TEX_SMALLER            = 0x8000,
    TEX_RESCALE_AVG        = 0x10000
};

class BTexture
{
public:
    BTexture();
    virtual ~BTexture();
    const char *ReadTextureFromFile(const std::string &file, int flags = 0);
    const char *ReadTextureFromFile(std::ifstream &fs, int flags = 0);
    bool        WriteTextureToFile(const std::string &file,
                                   int flags = 0) const;
    bool        WriteTextureToFile(std::ofstream &fs, int flags = 0) const;
    bool        SetTexels(const char *texels, unsigned int width,
                          unsigned int height, unsigned int channels,
                          int format);
    const char *CopyAlphaChannel(int format);
    const char *SetAlphaChannel(int value);
    const char *CopyColor(int format);
    const char *SetColors(int format, int red, int green, int blue);
    bool        CreateSubImage(int x, int y, int width, int height,
                               char *texels) const;
    const char *Rescale(int exp, int format);
    void        printInfo(std::ostream &fs) const;

    inline unsigned int GetWidth() const
    {
        return width;
    };
    inline unsigned int GetHeight() const
    {
        return height;
    };
    inline size_t GetRowBytes() const
    {
        return rowbytes;
    };
    inline size_t GetChannels() const
    {
        return channels;
    };
    inline int GetBitDepth() const
    {
        return bit_depth;
    };

    static int GetExpToBase2(unsigned int value);
    static bool IsPowerOf2(unsigned int value);

    void SetChannels(unsigned int value)
    {
        channels = value;
        rowbytes = width * channels;
    };
    void SetWidth(unsigned int value)
    {
        width    = value;
        rowbytes = width * channels;
    };
    void SetHeight(unsigned int value)
    {
        height   = value;
    };

    // Check if file is available and it's format is supported
    static bool CheckFileFormat(const std::string &file);

protected:
    static void read_data_fn(png_structp png_ptr,
                             png_bytep data,
                             png_size_t length);
    static void write_data_fn(png_structp png_ptr,
                              png_bytep data,
                              png_size_t length);
    static void flush_fn(png_structp png_ptr);
    bool        WriteTextureToPngFile(std::ofstream &fs, int flags = 0) const;
    const char *ReadTextureFromPngFile(std::ifstream &fs, int flags = 0);
    unsigned char *texels;
    unsigned int width;
    unsigned int height;
    size_t channels;
    size_t rowbytes;
    int bit_depth;
    int color_type;
    int interlace_type;
    int compression_type;
    int filter_type;
};

#endif
