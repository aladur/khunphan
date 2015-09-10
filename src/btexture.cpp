/*
    btexture.cpp

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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <png.h>
#include "misc1.h"
#include "btexture.h"


// BTexture has a public interface for reading
// image data into a general purpose RGBA buffer
//
// Currently only PNG format is supported.
// It could easily be extended for other format using
// the envelope/letter pattern
// (Each different file reader has a different "letter" class)

BTexture::BTexture() : texels(NULL), width(0), height(0), channels(0),
    rowbytes(0), bit_depth(0), color_type(0), interlace_type(0),
    compression_type(0), filter_type(0)
{
}

BTexture::~BTexture()
{
    delete [] texels;
}

int BTexture::GetExpToBase2(unsigned int value)
{
    int result = 0;
    while (!(value & (1 << result)) && result < 32)
    {
        result++;
    }
    if (result >= 32 || (value & ~(1 << result)) != 0)
    {
        return -1;
    }
    return result;
}

bool BTexture::IsPowerOf2(unsigned int value)
{
    int bitCount = 0;

    while (value != 0)
    {
        if ((value & 1) != 0)
        {
            bitCount++;
        }
        value >>= 1;
    }
    return bitCount == 1;
}

#define  HEADER_BYTES_READ     8

bool BTexture::CheckFileFormat(const char *pFile)
{
    png_byte header[HEADER_BYTES_READ];
    FILE *fp = fopen(pFile, "rb");
    bool is_png = false;

    if (fp)
    {
        fread(header, 1, HEADER_BYTES_READ, fp);
        is_png = !png_sig_cmp(header, 0, HEADER_BYTES_READ);
        fclose(fp);
    }
    return is_png;
}

const char *BTexture::ReadTextureFromFile(const char *pFile, int flags)
{
    FILE *fp = fopen(pFile, "rb");
    if (!fp)
    {
        return NULL;
    }

    DEBUGPRINT1("Reading '%s'\n", pFile);

    const char *texels = ReadTextureFromFile(fp, flags);
    fclose(fp);

    return texels;
}

const char *BTexture::ReadTextureFromFile(FILE *fp, int flags)
{
    char header[5];

    if (!fp)
    {
        return NULL;
    }

    fseek(fp, 0, SEEK_SET);

    // Do a simple check for PNG file format
    fread(header, 1, 4, fp);

    if (!strncmp(&header[1], "PNG", 3))
    {
        return ReadTextureFromPngFile(fp, flags);
    }

    return NULL;
}

const char *BTexture::ReadTextureFromPngFile(FILE *fp, int flags)
{
    png_byte header[HEADER_BYTES_READ];

    fseek(fp, 0, SEEK_SET);

    fread(header, 1, HEADER_BYTES_READ, fp);
    if (png_sig_cmp(header, 0, HEADER_BYTES_READ))
    {
        return NULL;
    }

    delete [] texels;
    texels = NULL;

    png_structp png_ptr = png_create_read_struct
                          (PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
                           //user_error_fn, user_warning_fn);
                           NULL, NULL);
    if (!png_ptr)
    {
        return NULL;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return NULL;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return NULL;
    }

    // Assign the already opened file pointer
    png_init_io(png_ptr, fp);

    // Already HEADER_BYTES_READ bytes are read from the header
    png_set_sig_bytes(png_ptr, HEADER_BYTES_READ);

    // read png file info
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr,
                 (png_uint_32 *)&width, (png_uint_32 *)&height,
                 &bit_depth, &color_type, &interlace_type,
                 &compression_type, &filter_type);

    channels = png_get_channels(png_ptr, info_ptr);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    //fprintInfo(stdout);

    // convert paletted image with bit_depth <= 8 to RGB
    if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8)
    {
        png_set_expand(png_ptr);
    }

    // expand gray scale images with < 8 Bit depth to 8 Bit
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    {
        png_set_expand(png_ptr);
    }

    // add alpha channel in rRNS chunk
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    {
        png_set_expand(png_ptr);
    }

    // strip 16 Bit color depth to 8 Bit
    if (bit_depth == 16)
    {
        png_set_strip_16(png_ptr);
    }

    // Strip alpha channel if requested format is RGB
    if (!(flags & TEX_WITH_ALPHA) && (color_type & PNG_COLOR_MASK_ALPHA))
    {
        png_set_strip_alpha(png_ptr);
    }

    // expand color depth of < 8 to 8 Bit
    if (bit_depth < 8)
    {
        png_set_packing(png_ptr);
    }

    // Fill Alpha channel with 255 if RGBA format is requested
    //if ((format & TEX_WITH_ALPHA) && (bit_depth == 8) &&
    //    (color_type == PNG_COLOR_TYPE_RGB))
    if ((flags & TEX_WITH_ALPHA))
    {
        png_set_filler(png_ptr, 255, PNG_FILLER_AFTER);
    }

    // convert gray scale to RGB
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(png_ptr);
    }

    double gamma;
    double screen_gamma = 2.0; // this is only a guess

    // set value for gamma correction

    if (png_get_gAMA(png_ptr, info_ptr, &gamma))
    {
        png_set_gamma(png_ptr, screen_gamma, gamma);
    }
    else
    {
        png_set_gamma(png_ptr, screen_gamma, 0.45455);
    }

    // support interlaced images
    /* int number_of_passes = */ png_set_interlace_handling(png_ptr);

    // update info structure
    png_read_update_info(png_ptr, info_ptr);

    channels = png_get_channels(png_ptr, info_ptr);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    //fprintInfo(stdout);

    png_bytepp row_pointers;

    texels =       (char *)    new char [height * rowbytes];
    row_pointers = (png_bytepp)new char [height * sizeof(char *)];
    for (unsigned int i = 0; i < height; i++)
    {
        row_pointers[height - i - 1] = (png_bytep)&texels[rowbytes * i];
    }

    // and finally read the image
    png_read_image(png_ptr, row_pointers);

    delete [] row_pointers;

    // cleanup
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    return texels;
}

const char *BTexture::SetAlphaChannel(int value)
{
    if (texels == NULL)
    {
        return NULL;
    }

    unsigned int i;

    for (i = 0; i < rowbytes * height; i += channels)
    {
        texels[i+channels-1] = value;
    }

    return texels;
}

const char *BTexture::CopyAlphaChannel(int format)
{
    if (texels == NULL)
    {
        return NULL;
    }


    if (format & TEX_MODIFY_ALPHA)
    {
        unsigned int i;

        for (i = 0; i < rowbytes * height; i += channels)
        {
            if ((format & TEX_MODIFY_ALPHA) == TEX_USE_RED_FOR_ALPHA)
            {
                texels[i+channels-1] = texels[i];
            }
            if ((channels > 1) &&
                ((format & TEX_MODIFY_ALPHA) == TEX_USE_GREEN_FOR_ALPHA))
            {
                texels[i+channels-1] = texels[i+1];
            }
            if (channels > 2 &&
                ((format & TEX_MODIFY_ALPHA) == TEX_USE_BLUE_FOR_ALPHA))
            {
                texels[i+channels-1] = texels[i+2];
            }
        }
    }
    return texels;
}

const char *BTexture::CopyColor(int format)
{
    if (texels == NULL)
    {
        return NULL;
    }

    if (format & TEX_COPY_COLOR)
    {
        unsigned int i;

        for (i = 0; i < rowbytes * height; i += channels)
        {
            if (channels > 1 &&
                ((format & TEX_COPY_COLOR) == TEX_COPY_RED_TO_GREEN))
            {
                texels[i+1] = texels[i];
            }
            if (channels > 2 &&
                ((format & TEX_COPY_COLOR) == TEX_COPY_RED_TO_BLUE))
            {
                texels[i+2] = texels[i];
            }
            if (channels > 1 &&
                ((format & TEX_COPY_COLOR) == TEX_COPY_GREEN_TO_RED))
            {
                texels[i]   = texels[i+1];
            }
            if (channels > 2 &&
                ((format & TEX_COPY_COLOR) == TEX_COPY_GREEN_TO_BLUE))
            {
                texels[i+2] = texels[i+1];
            }
            if (channels > 2 &&
                ((format & TEX_COPY_COLOR) == TEX_COPY_BLUE_TO_RED))
            {
                texels[i]   = texels[i+2];
            }
            if (channels > 2 &&
                ((format & TEX_COPY_COLOR) == TEX_COPY_BLUE_TO_GREEN))
            {
                texels[i+1] = texels[i+2];
            }
        }
    }
    return texels;
}

const char *BTexture::SetColors(int format, int red, int green, int blue)
{
    if (texels == NULL)
    {
        return NULL;
    }


    if (format & TEX_SET_COLORS)
    {
        unsigned int i;

        for (i = 0; i < rowbytes * height; i += channels)
        {
            if ((format & TEX_SET_COLOR_RED) == TEX_SET_COLOR_RED)
            {
                texels[i] = red;
            }
            if (channels > 1 &&
                ((format & TEX_SET_COLOR_GREEN) == TEX_SET_COLOR_GREEN))
            {
                texels[i+1] = green;
            }
            if (channels > 2 &&
                ((format & TEX_SET_COLOR_BLUE) == TEX_SET_COLOR_BLUE))
            {
                texels[i+2] = blue;
            }
        }
    }
    return texels;
}

bool BTexture::CreateSubImage(int x, int y, int w, int h, char *subTexels)
{
    if (subTexels == NULL)
    {
        return false;
    }

    int i = 0;
    unsigned int ay;

    for ( ay = height - y - h; ay < height - y; ay++ )
    {
        memcpy(&subTexels[i], &texels[ay * rowbytes + (x * channels)],
               w * channels);
        i += w * channels;
    }
    return true;
}

void BTexture::fprintInfo(FILE *fp)
{
    char *pColorType;
    char *pInterlaceType;
    char *pCompressionType;

    switch (color_type)
    {
        case PNG_COLOR_TYPE_GRAY:
            pColorType = (char *)"PNG_COLOR_TYPE_GRAY";
            break;
        case PNG_COLOR_TYPE_PALETTE:
            pColorType = (char *)"PNG_COLOR_TYPE_PALETTE";
            break;
        case PNG_COLOR_TYPE_RGB:
            pColorType = (char *)"PNG_COLOR_TYPE_RGB";
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            pColorType = (char *)"PNG_COLOR_TYPE_RGB_ALPHA";
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            pColorType = (char *)"PNG_COLOR_TYPE_GRAY_ALPHA";
            break;
        default:
            pColorType = (char *)"???";
    }
    switch (interlace_type)
    {
        case PNG_INTERLACE_NONE:
            pInterlaceType = (char *)"PNG_INTERLACE_NONE";
            break;
        case PNG_INTERLACE_ADAM7:
            pInterlaceType = (char *)"PNG_INTERLACE_ADAM7";
            break;
        case PNG_INTERLACE_LAST:
            pInterlaceType = (char *)"PNG_INTERLACE_LAST";
            break;
        default:
            pInterlaceType = (char *)"???";
    }
    switch (compression_type)
    {
        case PNG_COMPRESSION_TYPE_BASE:
            pCompressionType = (char *)"PNG_COMPRESSION_TYPE_BASE";
            break;
        default:
            pCompressionType = (char *)"???";
    }

    fprintf(fp, "Info about png-file:\n");
    fprintf(fp, "   Width:            %u\n", width);
    fprintf(fp, "   Height:           %u\n", height);
    fprintf(fp, "   bit_depth:        %d\n", bit_depth);
    fprintf(fp, "   color_type:       %s\n", pColorType);
    fprintf(fp, "   interlace_type:   %s\n", pInterlaceType);
    fprintf(fp, "   compression_type: %s\n", pCompressionType);
    fprintf(fp, "   filter_type:      %d\n", filter_type);
    fprintf(fp, "   channels:         %u\n", channels);
    fprintf(fp, "   rowbytes:         %u\n", rowbytes);
    fprintf(fp, "\n");
}

bool BTexture::WriteTextureToFile(const char *pFile, int flags)
{
    FILE *fp = fopen(pFile, "wb");
    if (!fp)
    {
        return false;
    }

    bool success = WriteTextureToFile(fp, flags);
    fclose(fp);

    return success;
}

bool BTexture::WriteTextureToFile(FILE *fp, int flags)
{
    if (!fp)
    {
        return false;
    }

    return WriteTextureToPngFile(fp, flags);
}

bool BTexture::SetTexels(const char *pTexels, unsigned int aWidth,
                         unsigned int aHeight, unsigned int aChannels,
                         int format)
{
    delete [] texels;
    texels = NULL;

    channels = aChannels;
    width    = aWidth;
    height   = aHeight;
    rowbytes = width * channels;
    bit_depth= 8;

    texels = (char *)new char [rowbytes * height];
    memcpy(texels, pTexels, rowbytes * height);

    switch (format)
    {
        case  TEX_ILLUMINANCE:
            color_type = PNG_COLOR_TYPE_GRAY;
            break;
        case  TEX_ILLUMINANCE_ALPHA:
            color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
            break;
        case  TEX_RGB:
            color_type = PNG_COLOR_TYPE_RGB;
            break;
        case  TEX_RGB_ALPHA:
            color_type = PNG_COLOR_TYPE_RGB_ALPHA;
            break;
        default:
            color_type = PNG_COLOR_TYPE_RGB_ALPHA;
            break;
    }

    return true;
}

bool BTexture::WriteTextureToPngFile (FILE *fp, int)
{
    if (fp == NULL)
    {
        return false;
    }

    png_structp png_ptr = png_create_write_struct
                          (PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    //user_error_fn, user_warning_fn);
    if (!png_ptr)
    {
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return false;
    }

    if (setjmp(png_ptr->jmpbuf))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    png_init_io(png_ptr, fp);

    png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);
    //png_set_filter(png_ptr, 0, PNG_FILTER_NONE);

    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    png_set_packing(png_ptr);

    png_bytepp row_pointers;

    row_pointers = (png_bytepp)new char [height * sizeof(char *)];
    for (unsigned int i = 0; i < height; i++)
    {
        row_pointers[height - i - 1] = (png_bytep)&texels[rowbytes * i];
    }

    // and finally write the image
    png_write_image(png_ptr, row_pointers);

    delete [] row_pointers;

    png_write_end(png_ptr, info_ptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    return true;
}

// reduce or increase image by 2 ^ exp
const char *BTexture::Rescale(int exp, int format)
{
    if (exp == 0)
    {
        return texels;
    }

    unsigned int factor = 1 << exp;

    if ((format & TEX_LARGER) != 0)
    {
        // unfinished
        return NULL;
    }
    if ((format & TEX_SMALLER) != 0)
    {

        // Width and Height must be multiples of factor
        if (GetWidth() % factor != 0 || (GetHeight() % factor != 0))
        {
            return NULL;
        }

        char *oldTexels = texels;

        SetWidth (GetWidth()  >> exp);
        SetHeight(GetHeight() >> exp);
        texels = (char *)new char[GetHeight() * GetRowBytes()];

        unsigned int   x, y;

        if ((format & TEX_RESCALE_AVG) != 0)
        {
            // resulting color and alpha channel
            // is the average of all source texels
            unsigned short ox, oy;
            unsigned short c;
            const char *pOldTexel  = NULL;
            const char *pOldTexel0 = NULL; // address of upper left texel in
                                           // old texture
            unsigned int  *pBuffer = new unsigned
            int[GetChannels()];            // buffer for one texel
            int            shift   = 2*exp;
            unsigned int   fChannels = factor * GetChannels();

            for (y = 0; y < GetHeight(); y++)
            {
                char *pRow = &texels[ y * GetRowBytes()];
                pOldTexel0 = &oldTexels[(y * GetRowBytes()) << shift];
                for(x = 0; x < GetWidth(); x++)
                {
                    memset(pBuffer, 0, GetChannels() * sizeof(unsigned int));
                    for (oy = 0; oy < factor; oy++)
                    {
                        c = 0;
                        pOldTexel = pOldTexel0 + (oy * (GetRowBytes() << exp));
                        for (ox = 0; ox < fChannels; ox++)
                        {
                            pBuffer[c] += (unsigned char)*(pOldTexel++);
                            if (++c == GetChannels())
                            {
                                c = 0;
                            }
                        }
                    }
                    switch (GetChannels())
                    {
                        case 1:
                            *(pRow++) = pBuffer[0] >> shift;
                            break;
                        case 2:
                            *(pRow++) = pBuffer[0] >> shift;
                            *(pRow++) = pBuffer[1] >> shift;
                            break;
                        case 3:
                            *(pRow++) = pBuffer[0] >> shift;
                            *(pRow++) = pBuffer[1] >> shift;
                            *(pRow++) = pBuffer[2] >> shift;
                            break;
                        case 4:
                            *(pRow++) = pBuffer[0] >> shift;
                            *(pRow++) = pBuffer[1] >> shift;
                            *(pRow++) = pBuffer[2] >> shift;
                            *(pRow++) = pBuffer[3] >> shift;
                            break;
                        default:
                            for (c = 0; c < GetChannels(); c++)
                            {
                                *(pRow++) = pBuffer[c] >> shift;
                            }
                    }
                    pOldTexel0 += GetChannels() << exp;
                }
            }
            delete [] pBuffer;
        }
        else
        {
            // resulting color and alpha channel is the
            // color and alpha value of the upper left texel (is faster)
            for (y = 0; y < GetHeight(); y++)
            {
                char       *pRow    = &texels   [ y * GetRowBytes()];
                const char *pOldRow = &oldTexels[(y * GetRowBytes()) <<(2*exp)];

                for(x = 0; x < GetWidth(); x++)
                {
                    memcpy(&pRow[x * GetChannels()],
                           &pOldRow[(x * GetChannels()) << exp],
                           GetChannels());
                }
            }
        }
        delete [] oldTexels;

        return texels;
    }
    return NULL;
}
