/*
    btexture.cpp

    Basic class for using textures
    Copyright (C) 2001-2016  W. Schwotzer

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

#include "stdafx.h"
#include <ios>
#include <string>
#include "btexture.h"

// zlib best compression value
#ifndef Z_BEST_COMPRESSION
    #define Z_BEST_COMPRESSION 9
#endif

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

bool BTexture::CheckFileFormat(const std::string &file)
{
    png_byte header[HEADER_BYTES_READ];
    std::ifstream fs;
    bool is_png = false;

    fs.open(file.c_str(), std::ios::binary | std::ios::in);

    if (fs.is_open())
    {
        fs.read(reinterpret_cast<char *>(header), sizeof(header));
        is_png = (fs.gcount() == sizeof(header));
        is_png &= !png_sig_cmp(header, 0, HEADER_BYTES_READ);
        fs.close();
    }

    return is_png;
}

const char *BTexture::ReadTextureFromFile(
    const std::string &file, int flags/*= 0 */)
{
    std::ifstream fs;

    fs.open(file.c_str(), std::ios::binary | std::ios::in);

    if (!fs.is_open())
    {
        return NULL;
    }

    LOG3("Reading '", file.c_str(), "'");

    const char *returned_texels = ReadTextureFromFile(fs, flags);
    fs.close();

    return static_cast<const char *>(returned_texels);
}

const char *BTexture::ReadTextureFromFile(std::ifstream &fs, int flags/* = 0 */)
{
    png_byte header[HEADER_BYTES_READ];

    if (!fs.is_open())
    {
        return NULL;
    }

    fs.seekg(0);

    // Do a simple check for PNG file format
    fs.read(reinterpret_cast<char *>(header), sizeof(header));

    if (fs.gcount() == sizeof(header) &&
        !png_sig_cmp(header, 0, sizeof(header)))
    {
        return ReadTextureFromPngFile(fs, flags);
    }

    return NULL;
}

const char *BTexture::ReadTextureFromPngFile(std::ifstream &fs,
        int flags /* = 0 */)
{
    png_byte header[HEADER_BYTES_READ];
    png_uint_32 png_width;
    png_uint_32 png_height;

    fs.seekg(0);

    fs.read(reinterpret_cast<char *>(header), sizeof(header));

    if (fs.gcount() != sizeof(header) ||
        png_sig_cmp(header, 0, sizeof(header)))
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

    // Assign user defined read function to read from input file stream
    png_set_read_fn(png_ptr, reinterpret_cast<png_voidp>(&fs), read_data_fn);

    // Already HEADER_BYTES_READ bytes are read from the header
    png_set_sig_bytes(png_ptr, HEADER_BYTES_READ);

    // read png file info
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr,
                 &png_width, &png_height,
                 &bit_depth, &color_type, &interlace_type,
                 &compression_type, &filter_type);

    width = static_cast<unsigned int>(png_width),
    height = static_cast<unsigned int>(png_height),

    channels = png_get_channels(png_ptr, info_ptr);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    //printInfo(std::cout);

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

    //printInfo(std::cout);

    png_bytepp row_pointers;

    texels =       new unsigned char [height * rowbytes];
    row_pointers = new unsigned char *[height];

    for (unsigned int i = 0; i < height; i++)
    {
        row_pointers[height - i - 1] = &texels[rowbytes * i];
    }

    // and finally read the image
    png_read_image(png_ptr, row_pointers);

    delete [] row_pointers;

    // cleanup
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    return reinterpret_cast<const char *>(texels);
}

const char *BTexture::SetAlphaChannel(int value)
{
    if (texels != NULL)
    {
        size_t i;

        for (i = 0; i < rowbytes * height; i += channels)
        {
            texels[i + channels - 1] = value;
        }
    }

    return reinterpret_cast<const char *>(texels);
}

const char *BTexture::CopyAlphaChannel(int format)
{
    if (texels != NULL)
    {
        if (format & TEX_MODIFY_ALPHA)
        {
            size_t i;

            for (i = 0; i < rowbytes * height; i += channels)
            {
                if ((format & TEX_MODIFY_ALPHA) == TEX_USE_RED_FOR_ALPHA)
                {
                    texels[i + channels - 1] = texels[i];
                }

                if ((channels > 1) &&
                    ((format & TEX_MODIFY_ALPHA) == TEX_USE_GREEN_FOR_ALPHA))
                {
                    texels[i + channels - 1] = texels[i + 1];
                }

                if (channels > 2 &&
                    ((format & TEX_MODIFY_ALPHA) == TEX_USE_BLUE_FOR_ALPHA))
                {
                    texels[i + channels - 1] = texels[i + 2];
                }
            }
        }
    }

    return reinterpret_cast<const char *>(texels);
}

const char *BTexture::CopyColor(int format)
{
    if (texels != NULL)
    {
        if (format & TEX_COPY_COLOR)
        {
            size_t i;

            for (i = 0; i < rowbytes * height; i += channels)
            {
                if (channels > 1 &&
                    ((format & TEX_COPY_COLOR) == TEX_COPY_RED_TO_GREEN))
                {
                    texels[i + 1] = texels[i];
                }

                if (channels > 2 &&
                    ((format & TEX_COPY_COLOR) == TEX_COPY_RED_TO_BLUE))
                {
                    texels[i + 2] = texels[i];
                }

                if (channels > 1 &&
                    ((format & TEX_COPY_COLOR) == TEX_COPY_GREEN_TO_RED))
                {
                    texels[i]   = texels[i + 1];
                }

                if (channels > 2 &&
                    ((format & TEX_COPY_COLOR) == TEX_COPY_GREEN_TO_BLUE))
                {
                    texels[i + 2] = texels[i + 1];
                }

                if (channels > 2 &&
                    ((format & TEX_COPY_COLOR) == TEX_COPY_BLUE_TO_RED))
                {
                    texels[i]   = texels[i + 2];
                }

                if (channels > 2 &&
                    ((format & TEX_COPY_COLOR) == TEX_COPY_BLUE_TO_GREEN))
                {
                    texels[i + 1] = texels[i + 2];
                }
            }
        }
    }

    return reinterpret_cast<const char *>(texels);
}

const char *BTexture::SetColors(int format, int red, int green, int blue)
{
    if (texels != NULL)
    {
        if (format & TEX_SET_COLORS)
        {
            size_t i;

            for (i = 0; i < rowbytes * height; i += channels)
            {
                if ((format & TEX_SET_COLOR_RED) == TEX_SET_COLOR_RED)
                {
                    texels[i] = red;
                }

                if (channels > 1 &&
                    ((format & TEX_SET_COLOR_GREEN) == TEX_SET_COLOR_GREEN))
                {
                    texels[i + 1] = green;
                }

                if (channels > 2 &&
                    ((format & TEX_SET_COLOR_BLUE) == TEX_SET_COLOR_BLUE))
                {
                    texels[i + 2] = blue;
                }
            }
        }
    }

    return reinterpret_cast<const char *>(texels);
}

bool BTexture::CreateSubImage(int x, int y, int w, int h, char *subTexels) const
{
    if (subTexels == NULL)
    {
        return false;
    }

    size_t i = 0;
    unsigned int ay;

    for (ay = height - y - h; ay < height - y; ay++)
    {
        memcpy(&subTexels[i], &texels[ay * rowbytes + (x * channels)],
               w * channels);
        i += w * channels;
    }

    return true;
}

void BTexture::printInfo(std::ostream &os) const
{
    const char *pColorType;
    const char *pInterlaceType;
    const char *pCompressionType;

    switch (color_type)
    {
        case PNG_COLOR_TYPE_GRAY:
            pColorType = static_cast<const char *>("PNG_COLOR_TYPE_GRAY");
            break;

        case PNG_COLOR_TYPE_PALETTE:
            pColorType = static_cast<const char *>("PNG_COLOR_TYPE_PALETTE");
            break;

        case PNG_COLOR_TYPE_RGB:
            pColorType = static_cast<const char *>("PNG_COLOR_TYPE_RGB");
            break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            pColorType = static_cast<const char *>("PNG_COLOR_TYPE_RGB_ALPHA");
            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            pColorType = static_cast<const char *>("PNG_COLOR_TYPE_GRAY_ALPHA");
            break;

        default:
            pColorType = static_cast<const char *>("???");
    }

    switch (interlace_type)
    {
        case PNG_INTERLACE_NONE:
            pInterlaceType = static_cast<const char *>("PNG_INTERLACE_NONE");
            break;

        case PNG_INTERLACE_ADAM7:
            pInterlaceType = static_cast<const char *>("PNG_INTERLACE_ADAM7");
            break;

        case PNG_INTERLACE_LAST:
            pInterlaceType = static_cast<const char *>("PNG_INTERLACE_LAST");
            break;

        default:
            pInterlaceType = static_cast<const char *>("???");
    }

    switch (compression_type)
    {
        case PNG_COMPRESSION_TYPE_BASE:
            pCompressionType =
                static_cast<const char *>("PNG_COMPRESSION_TYPE_BASE");
            break;

        default:
            pCompressionType = static_cast<const char *>("???");
    }

    os << "Info about png-file:" << std::endl
       << "   Width:            " << width << std::endl
       << "   Height:           " << height << std::endl
       << "   bit_depth:        " << bit_depth << std::endl
       << "   color_type:       " << pColorType << std::endl
       << "   interlace_type:   " << pInterlaceType << std::endl
       << "   compression_type: " << pCompressionType << std::endl
       << "   filter_type:      " << filter_type << std::endl
       << "   channels:         " << channels << std::endl
       << "   rowbytes:         " << rowbytes << std::endl
       << std::endl;
}

bool BTexture::WriteTextureToFile(
    const std::string &file, int flags /* = 0 */) const
{
    std::ofstream fs;
    bool success = false;

    fs.open(file.c_str(), std::ios::binary | std::ios::out);

    if (fs.is_open())
    {
        success = WriteTextureToFile(fs, flags);
        fs.close();
    }

    return success;
}

bool BTexture::WriteTextureToFile(std::ofstream &fs, int flags /* = 0 */) const
{
    if (!fs.is_open())
    {
        return false;
    }

    return WriteTextureToPngFile(fs, flags);
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
    bit_depth = 8;

    texels = new unsigned char [rowbytes * height];
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

bool BTexture::WriteTextureToPngFile(std::ofstream &fs, int) const
{
    if (!fs.is_open())
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

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    // Assign user defined write and flush function
    // to write to output file stream
    png_set_write_fn(png_ptr, reinterpret_cast<png_voidp>(&fs),
                     write_data_fn,
                     flush_fn);

    png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);
    //png_set_filter(png_ptr, 0, PNG_FILTER_NONE);

    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    png_set_packing(png_ptr);

    png_bytepp row_pointers;

    row_pointers = static_cast<png_bytepp>(new unsigned char *[height]);

    for (unsigned int i = 0; i < height; i++)
    {
        row_pointers[height - i - 1] =
            reinterpret_cast<png_bytep>(&texels[rowbytes * i]);
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
        return reinterpret_cast<const char *>(texels);
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

        unsigned char *oldTexels = texels;

        SetWidth(GetWidth()  >> exp);
        SetHeight(GetHeight() >> exp);
        texels = new unsigned char[GetHeight() * GetRowBytes()];

        unsigned int   x, y;

        if ((format & TEX_RESCALE_AVG) != 0)
        {
            // resulting color and alpha channel
            // is the average of all source texels
            unsigned short ox, oy, i;
            unsigned short c;
            const unsigned char *pOldTexel  = NULL;
            const unsigned char *pOldTexel0 = NULL; // address of upper left
            // texel in old texture
            unsigned int  *pBuffer = new unsigned int[GetChannels()];
            // buffer for one texel
            int            shift   = 2 * exp;
            size_t         fChannels = factor * GetChannels();

            for (y = 0; y < GetHeight(); y++)
            {
                unsigned char *pRow = &texels[ y * GetRowBytes()];
                pOldTexel0 = &oldTexels[(y * GetRowBytes()) << shift];

                for (x = 0; x < GetWidth(); x++)
                {
                    for (i = 0; i < GetChannels(); ++i)
                    {
                        pBuffer[i] = 0;
                    }

                    for (oy = 0; oy < factor; oy++)
                    {
                        c = 0;
                        pOldTexel = pOldTexel0 + (oy * (GetRowBytes() << exp));

                        for (ox = 0; ox < fChannels; ox++)
                        {
                            pBuffer[c] += *(pOldTexel++);

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
                unsigned char       *pRow    = &texels   [ y * GetRowBytes()];
                const unsigned char *pOldRow = &oldTexels[(y * GetRowBytes())
                                               << (2 * exp)];

                for (x = 0; x < GetWidth(); x++)
                {
                    memcpy(&pRow[x * GetChannels()],
                           &pOldRow[(x * GetChannels()) << exp],
                           GetChannels());
                }
            }
        }

        delete [] oldTexels;

        return reinterpret_cast<const char *>(texels);
    }

    return NULL;
}

void BTexture::read_data_fn(png_structp png_ptr,
                            png_bytep data,
                            png_size_t length)
{
    png_voidp io_ptr = png_get_io_ptr(png_ptr);

    if (io_ptr != NULL)
    {
        std::ifstream &fs = *(std::ifstream *)io_ptr;

        fs.read(reinterpret_cast<char *>(data), length);

        if (static_cast<png_size_t>(fs.gcount()) != length)
        {
            png_error(png_ptr, "Read less bytes than requested.");
        }
    }
}

void BTexture::write_data_fn(png_structp png_ptr,
                             png_bytep data,
                             png_size_t length)
{
    png_voidp io_ptr = png_get_io_ptr(png_ptr);

    if (io_ptr != NULL)
    {
        std::ofstream &fs = *(std::ofstream *)io_ptr;

        fs.write(reinterpret_cast<char *>(data), length);

        if (fs.bad())
        {
            png_error(png_ptr, "Non-recoverable write error occured.");
        }
    }
}

void BTexture::flush_fn(png_structp png_ptr)
{
    png_voidp io_ptr = png_get_io_ptr(png_ptr);

    if (io_ptr != NULL)
    {
        std::ofstream &fs = *(std::ofstream *)io_ptr;

        fs.flush();
    }
}

