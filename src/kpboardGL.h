/*
    kpboardGL.h


    Copyright (C) 2002-2015  W. Schwotzer

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

#ifndef _KPBOARD_H__
#define _KPBOARD_H__

#include "kpnode.h"


#define   BOARD                 0
#define   WOOD_TEXTURE          1
#define   RED_MATERIAL          2
#define   BLINKING_RED_MATERIAL 3
#define   BRONZE_MATERIAL       4
#define   CUBOID_1X1_1          5
#define   CUBOID_1X1_2          6
#define   CUBOID_1X1_3          7
#define   CUBOID_1X1_4          8
#define   CUBOID_2X1            9
#define   CUBOID_1X2_1          10
#define   CUBOID_1X2_2          11
#define   CUBOID_1X2_3          12
#define   CUBOID_1X2_4          13
#define   CUBOID_2X2            14
#define   KBP_ENTRY_COUNT       15


class Kamera;
class Light;
class BString;

#define MAX_BOARD_TEXTURES  4


class KPboardView
{
public:
  KPboardView(void);
  virtual ~KPboardView();

  bool Initialize(const char *TextureName, unsigned int TextureSize = 1, bool Nearest = true);
  void InitializeTextures(const char *TextureName, unsigned int TextureSize = 1, bool Nearest =true, bool always = true);
  void Draw(bool render = true) const;
  void SetSolveTree(KPnode *n);
  void ResetBoard();
  QWord GetBoardId();
  void SetBoard(KPnode *n);
  void SetBoard(QWord id);
  short GetMovesToSolve() const;
  tKPTokenID Selection(const Kamera *pCamera, int x, int y) const;
  bool Move(tKPTokenID id, tKPDirection d);
  bool CanMove(tKPTokenID id, tKPDirection d);
  void Animate(int Factor);
  void EmphasizeToken(tKPTokenID id = TK_EMPTY) { emphasizedToken = id; };
  bool IsSolved();

private:
  void DrawToken(const tKPTokenID i) const;
  void DrawCuboid(float deltaAngle) const; // Only for test purposes
  void CreateCuboid(float dx, float dy, float dz, float x0, float y0, float z0, bool WithTexture = true) const;
  bool CreateTexture(unsigned int TextureSize, const char *pFile, bool Nearest, unsigned int *pId);
  void GetTokenCoordinates(tKPTokenID i, float *x, float *y) const;
  float getRnd(void) const;

  KPnode *pSolveTree; // Pointer to game model
  KPboard current;
  static const char *textureFile[MAX_BOARD_TEXTURES + 1];

// variables for token animation:
  float mat_value, xValue;
  tKPTokenID emphasizedToken;
  tKPTokenID animatedToken;
  float old_x, old_y, new_x, new_y, ax, ay, Time;
  unsigned int textureId[MAX_BOARD_TEXTURES];
  BString *textureSource;
  int callList;
};

#endif
