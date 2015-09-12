/*
    btexture.h

    Basic class for using textures
    Copyright (C) 2001-2015  W. Schwotzer

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

#ifndef _BTEXTURE_H_
#define _BTEXTURE_H_

#include <stdio.h>


enum tTextureFormat
{
    TEX_WITH_ALPHA         = 0x01,

    TEX_MODIFY_ALPHA       = 0x0C,
    TEX_USE_RED_FOR_ALPHA  = 0x04,
    TEX_USE_GREEN_FOR_ALPHA= 0x08,
    TEX_USE_BLUE_FOR_ALPHA = 0x0C,

    TEX_COPY_COLOR         = 0x70,
    TEX_COPY_RED_TO_GREEN  = 0x10,
    TEX_COPY_RED_TO_BLUE   = 0x20,
    TEX_COPY_GREEN_TO_RED  = 0x30,
    TEX_COPY_GREEN_TO_BLUE = 0x40,
    TEX_COPY_BLUE_TO_RED   = 0x50,
    TEX_COPY_BLUE_TO_GREEN = 0x60,

    TEX_SET_COLORS         = 0x700,
    TEX_SET_COLOR_RED      = 0x100,
    TEX_SET_COLOR_GREEN    = 0x200,
    TEX_SET_COLOR_BLUE     = 0x400,

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
    const char *ReadTextureFromFile(const char *pFile, int flags = 0);
    const char *ReadTextureFromFile(FILE *fp, int flags = 0);
    bool        WriteTextureToFile (const char *pFile, int flags = 0);
    bool        WriteTextureToFile (FILE *fp, int flags = 0);
    bool        SetTexels (const char *texels, unsigned int width,
                           unsigned int height, unsigned int channels,
                           int format);
    const char *CopyAlphaChannel(int format);
    const char *SetAlphaChannel(int value);
    const char *CopyColor(int format);
    const char *SetColors(int format, int red, int green, int blue);
    bool        CreateSubImage(int x, int y, int width, int height,
                               char *texels);
    const char *Rescale(int exp, int format);
    void        fprintInfo(FILE *fp);

    inline unsigned int GetWidth()
    {
        return width;
    };
    inline unsigned int GetHeight()
    {
        return height;
    };
    inline unsigned int GetRowBytes()
    {
        return rowbytes;
    };
    inline unsigned int GetChannels()
    {
        return channels;
    };
    inline int          GetBitDepth()
    {
        return bit_depth;
    };

    static int   GetExpToBase2(unsigned int value);
    static bool  IsPowerOf2(unsigned int value);

    void SetChannels(unsigned int value)
    {
        channels = value;
        rowbytes = width * channels;
    };
    void SetWidth   (unsigned int value)
    {
        width    = value;
        rowbytes = width * channels;
    };
    void SetHeight  (unsigned int value)
    {
        height   = value;
    };

    static bool CheckFileFormat(const char
                                *pFile); // Check if file is available and
    // it's format is supported

protected:
    bool        WriteTextureToPngFile (FILE *fp, int flags = 0);
    const char *ReadTextureFromPngFile(FILE *fp, int flags);

    unsigned char *texels;
    unsigned long width;
    unsigned long height;
    unsigned int channels;
    unsigned int rowbytes;
    int bit_depth;
    int color_type;
    int interlace_type;
    int compression_type;
    int filter_type;
};

#endif
