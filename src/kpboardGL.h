/*
    kpboardGL.h


    Copyright (C) 2002-2016  W. Schwotzer

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

#ifndef KPBOARDGL_H
#define KPBOARDGL_H

#include <string>
#include <array>
#include "kpboard.h"

enum tDisplayList
{
    BOARD = 0,
    WOOD_TEXTURE,
    RED_MATERIAL,
    BLINKING_RED_MATERIAL,
    BRONZE_MATERIAL,
    CUBOID_1X1_1,
    CUBOID_1X1_2,
    CUBOID_1X1_3,
    CUBOID_1X1_4,
    CUBOID_2X1,
    CUBOID_1X2_1,
    CUBOID_1X2_2,
    CUBOID_1X2_3,
    CUBOID_1X2_4,
    CUBOID_2X2,
    KBP_ENTRY_COUNT
};

class Camera;

#define MAX_BOARD_TEXTURES  4


class KPboardView
{
public:
    KPboardView(const KPboard &currentBoard,
                const std::string &TextureDirectory,
                const std::string &TextureName,
                unsigned int TextureSize = 1, bool Nearest = true);
    KPboardView() = delete;
    KPboardView(const KPboardView &) = delete;
    KPboardView & operator=(const KPboardView &) = delete;
    ~KPboardView();

    void InitializeTextures(const std::string &TextureDirectory,
                            const std::string &TextureName,
                            unsigned int TextureSize = 1,
                            bool Nearest = true, bool always = true);
    void Draw(bool render = true) const;
    QWord GetBoardId() const;
    void SetBoard(const KPboard &);
    tKPTokenID Selection(const Camera &camera, int x, int y) const;
    bool Move(tKPTokenID id, tKPDirection d);
    bool CanMove(tKPTokenID id, tKPDirection d);
    void Animate(unsigned int duration);
    void EmphasizeToken(tKPTokenID id = TK_EMPTY)
    {
        emphasizedToken = id;
    };
    bool IsSolved();

private:
    typedef std::array<std::string, MAX_BOARD_TEXTURES> tArrayOfString;

    void Initialize(const std::string &TextureDirectory,
                    const std::string &TextureName,
                    unsigned int TextureSize = 1,
                    bool Nearest = true);
    void DrawToken(const tKPTokenID i) const;
    void DrawCuboid(float deltaAngle) const; // Only for test purposes
    void CreateCuboid(float dx, float dy, float dz,
                      float x0, float y0, float z0,
                      bool WithTexture = true) const;
    bool CreateTexture(unsigned int TextureSize, const std::string &file,
                       bool Nearest, unsigned int *pId);
    void GetTokenCoordinates(tKPTokenID i, float *x, float *y) const;
    float getRnd(void) const;

    KPboard current;
    static const tArrayOfString textureFiles;

    // variables for token animation:
    float mat_value, xValue;
    tKPTokenID emphasizedToken;
    tKPTokenID animatedToken;
    float old_x, old_y, new_x, new_y, ax, ay, Time;
    // This has to be a c-style compatible array
    // because it is used in glGenTextures
    std::array<unsigned int, MAX_BOARD_TEXTURES> textureIds;
    tArrayOfString textureSources;
    int DisplayList;
};

#endif
