/*
    kpboardGL.cpp


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


#include "stdafx.h"
#include <GL/gl.h>
#include <math.h>
#include <limits.h>
#include <stdexcept>
#include "kpconfig.h"
#include "kpboardGL.h"
#include "camera.h"
#include "light.h"
#include "btexture.h"


#define   DY    25.0f
#define   DX    25.0f
#define   DZ    12.0f
#define   DT    (4.0f * 25.0f)
#define   DGAP  1.0f
#define   DBOXH (DZ + 3.0f)
#define   DBOXW 5.0f

//#define   DRAW_TEST   1   // Uncomment this line only for test purposes

const char *KPboardView::textureFile[MAX_BOARD_TEXTURES] =
{
    "texture1.png",
    "texture2.png",
    "texture3.png",
    "board.png"
};

KPboardView::KPboardView(const KPboard &currentBoard,
                         const std::string &TextureDirectory,
                         const std::string &TextureName,
                         unsigned int TextureSize /*=1*/,
                         bool Nearest /*=true*/) :
    current(currentBoard),
    mat_value(0.0), xValue(0.0),
    emphasizedToken(TK_EMPTY), animatedToken(TK_EMPTY),
    old_x(0.0), old_y(0.0), new_x(0.0), new_y(0.0),
    ax(0.0), ay(0.0), Time(0.0), textureSource(NULL),
    DisplayList(0)
{
    srand(327);
    DisplayList = glGenLists(KBP_ENTRY_COUNT);

    if (DisplayList == 0)
    {
        // Could be caused if display list has no space left
        // for contiguous elements or any other error.
        throw std::runtime_error("Error creating a display list");
    }

    for (auto i = 0; i < MAX_BOARD_TEXTURES; i++)
    {
        textureId[i] = 0;
    }

    Initialize(TextureDirectory, TextureName, TextureSize, Nearest);
}

KPboardView::~KPboardView()
{
    glDeleteLists(DisplayList, KBP_ENTRY_COUNT);
    glDeleteTextures(MAX_BOARD_TEXTURES, textureId);
    delete [] textureSource;
}

QWord KPboardView::GetBoardId() const
{
    return current.GetID();
}

void KPboardView::SetBoard(const KPboard &board)
{
    current = board;
}

bool KPboardView::CreateTexture(unsigned int TextureSize,
                                const std::string &file,
                                bool Nearest, unsigned int *pId)
{
    // define a Display List WOOD_TEXTURE containing light and texture stuff
    BTexture *pTexture = new BTexture;

    auto *texels = pTexture->ReadTextureFromFile(file, 0);

    if (texels == NULL)
    {
        delete pTexture;
        return false;
    }

    texels = pTexture->Rescale(BTexture::GetExpToBase2(TextureSize),
                               TEX_SMALLER | TEX_RESCALE_AVG);

    auto width  = pTexture->GetWidth();
    auto height = pTexture->GetHeight();

    if (!BTexture::IsPowerOf2(width) || !BTexture::IsPowerOf2(height))
    {
        message(mtWarning, "Width or Height of '%s' is not a power of 2\n",
                file.c_str());
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glBindTexture(GL_TEXTURE_2D, *pId);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    (Nearest ? GL_NEAREST : GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    (Nearest ? GL_NEAREST : GL_LINEAR));
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY,   1.0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, texels);
    // Avoid to bind the texture by another glTexImage2D:
    glBindTexture(GL_TEXTURE_2D, 0);
    delete pTexture;
    return true;
}

void KPboardView::InitializeTextures(const std::string &TextureDirectory,
                                     const std::string &TextureName,
                                     unsigned int TextureSize /*=1*/,
                                     bool         Nearest /*=true*/,
                                     bool         always /*=true*/)
{
    // If Textures are enabled read them from PNG files
    auto i = 0;

    if (textureId[0] == 0)
    {
        glGenTextures(MAX_BOARD_TEXTURES, &textureId[0]);
    }

    if (textureSource == NULL)
    {
        textureSource = new std::string[MAX_BOARD_TEXTURES];
    }

    while (TextureSize && (i < MAX_BOARD_TEXTURES))
    {
        auto file = TextureDirectory + TextureName +
                    PATHSEPARATORSTRING + textureFile[i];

        if (!always && file == textureSource[i])
        {
            continue;
        }

        if (!CreateTexture(TextureSize, file, Nearest, &textureId[i]))
        {
            file = TextureDirectory + textureFile[i];

            if (!always && file == textureSource[i])
            {
                continue;
            }

            if (!CreateTexture(TextureSize, file, Nearest,
                               &textureId[i]))
            {
                std::stringstream message;

                message << "*** Error creating texture from image file '"
                        << file << "'";
                throw std::runtime_error(message.str());
            }
        }

        textureSource[i] = file;
        i++;
    }
}

void KPboardView::Initialize(const std::string &TextureDirectory,
                             const std::string &TextureName,
                             unsigned int TextureSize /*=1*/,
                             bool Nearest /*=true*/)
{
    InitializeTextures(TextureDirectory, TextureName, TextureSize, Nearest);

    glNewList(DisplayList + BRONZE_MATERIAL, GL_COMPILE);
    {
        GLfloat mat_ambient[]  = {0.50f, 0.50f, 0.50f, 1.0f};
        GLfloat mat_diffuse[]  = {0.60f, 0.60f, 0.60f, 1.0f};
        GLfloat mat_specular[] = {0.15f, 0.15f, 0.15f, 1.0f};
        auto mat_shininess  = 20.0f;

        glDisable(GL_TEXTURE_2D);

        glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

        glEnable(GL_TEXTURE_2D);
    }
    glEndList();

    // define a Display List BOARD containing the Khun Phan board
    glNewList(DisplayList + BOARD, GL_COMPILE);

    // Front end (with gap for Khun Phan to escape :-)
    CreateCuboid(
        DBOXW + DX + DGAP, DBOXW, DBOXH,
        -((HORIZONTAL_MAX * (DX + DGAP) - DGAP) / 2) - DGAP - DBOXW,
        ((VERTICAL_MAX   * (DY + DGAP) - DGAP) / 2) + DGAP, 0);
    CreateCuboid(
        2 * (DX + DGAP) + DGAP, DBOXW, DBOXH - (DZ * 2 / 3) - DGAP,
        -((2 * (DX + DGAP) - DGAP) / 2) - DGAP,
        ((VERTICAL_MAX   * (DY + DGAP) - DGAP) / 2) + DGAP,
        (DZ * 2 / 3) + DGAP);
    CreateCuboid(
        DBOXW + DX + DGAP, DBOXW, DBOXH,
        ((2 * (DX + DGAP) - DGAP) / 2) + DGAP,
        ((VERTICAL_MAX   * (DY + DGAP) - DGAP) / 2) + DGAP, 0);
    // Back end
    CreateCuboid(
        2 * DBOXW + DGAP + (HORIZONTAL_MAX * (DX + DGAP)), DBOXW, DBOXH,
        -((HORIZONTAL_MAX * (DX + DGAP) - DGAP) / 2) - DGAP - DBOXW,
        -((VERTICAL_MAX   * (DY + DGAP) - DGAP) / 2) - DGAP - DBOXW, 0);
    // Left end
    CreateCuboid(
        DBOXW, VERTICAL_MAX * (DY + DGAP) + DGAP, DBOXH,
        -((HORIZONTAL_MAX * (DX + DGAP) - DGAP) / 2) - DGAP - DBOXW,
        -((VERTICAL_MAX   * (DY + DGAP) + DGAP) / 2), 0);
    // Right end
    CreateCuboid(
        DBOXW, VERTICAL_MAX * (DY + DGAP) + DGAP, DBOXH,
        ((HORIZONTAL_MAX  * (DX + DGAP) - DGAP) / 2) + DGAP,
        -((VERTICAL_MAX    * (DY + DGAP) - DGAP) / 2) - DGAP, 0);
    // Bottom end
    CreateCuboid(
        2 * DBOXW + DGAP + (HORIZONTAL_MAX * (DX + DGAP)),
        2 * DBOXW + DGAP + VERTICAL_MAX * (DY + DGAP), DBOXW,
        -(HORIZONTAL_MAX * (DX + DGAP) / 2) - DGAP - DBOXW,
        -(VERTICAL_MAX * (DY + DGAP) / 2) - DGAP - DBOXW, -DBOXW);

    glEndList();

    // define a Display List CUBOID_1X1 containing a cuboid with size 1 x 1

    glNewList(DisplayList + CUBOID_1X1_1, GL_COMPILE);
    CreateCuboid(DX, DY, DZ, 0, 0, 0, true);
    glEndList();
    glNewList(DisplayList + CUBOID_1X1_2, GL_COMPILE);
    CreateCuboid(DX, DY, DZ, 0, 0, 0, true);
    glEndList();
    glNewList(DisplayList + CUBOID_1X1_3, GL_COMPILE);
    CreateCuboid(DX, DY, DZ, 0, 0, 0, true);
    glEndList();
    glNewList(DisplayList + CUBOID_1X1_4, GL_COMPILE);
    CreateCuboid(DX, DY, DZ, 0, 0, 0, true);
    glEndList();

    // define a Display List CUBOID_1X2 containing a cuboid with size 1 x 2

    glNewList(DisplayList + CUBOID_1X2_1, GL_COMPILE);
    CreateCuboid(DX, 2 * DY + DGAP, DZ, 0, 0, 0, true);
    glEndList();
    glNewList(DisplayList + CUBOID_1X2_2, GL_COMPILE);
    CreateCuboid(DX, 2 * DY + DGAP, DZ, 0, 0, 0, true);
    glEndList();
    glNewList(DisplayList + CUBOID_1X2_3, GL_COMPILE);
    CreateCuboid(DX, 2 * DY + DGAP, DZ, 0, 0, 0, true);
    glEndList();
    glNewList(DisplayList + CUBOID_1X2_4, GL_COMPILE);
    CreateCuboid(DX, 2 * DY + DGAP, DZ, 0, 0, 0, true);
    glEndList();

    // define a Display List CUBOID_2X1 containing a cuboid with size 2 x 1

    glNewList(DisplayList + CUBOID_2X1, GL_COMPILE);
    CreateCuboid(2 * DX + DGAP, DY, DZ, 0, 0, 0, true);
    glEndList();

    // define a Display List CUBOID_2X2 containing a cuboid with size 2 x 2

    glNewList(DisplayList + CUBOID_2X2, GL_COMPILE);
    CreateCuboid(2 * DX + DGAP, 2 * DY + DGAP, DZ * 2 / 3, 0, 0, 0, true);
    glEndList();
}

float KPboardView::getRnd(void) const
{
    return static_cast<float>(rand()) / RAND_MAX;
}

void KPboardView::CreateCuboid(float dx, float dy, float dz, float x0, float y0,
                               float z0, bool WithTexture /* = true */) const
{
    GLfloat nv[] = {0, 0, 0};

    glBegin(GL_QUADS);

    // Top Face
    auto tx0 = getRnd();
    auto ty0 = getRnd();
    auto tdx = tx0 + dx / DT;
    auto tdy = ty0 + dy / DT;
    nv[0] = 0;
    nv[1] = 0;
    nv[2] = 1;
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, ty0);
    }

    glVertex3f(x0, y0, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, ty0);
    }

    glVertex3f(x0 + dx, y0, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, tdy);
    }

    glVertex3f(x0 + dx, y0 + dy, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, tdy);
    }

    glVertex3f(x0, y0 + dy, z0 + dz);

    // Back Face
    tx0 = getRnd();
    ty0 = getRnd();
    tdx = tx0 + dx / DT;
    tdy = ty0 + dz / DT;
    nv[0] = 0;
    nv[1] = -1;
    nv[2] = 0;
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, ty0);
    }

    glVertex3f(x0, y0, z0);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, ty0);
    }

    glVertex3f(x0 + dx, y0, z0);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, tdy);
    }

    glVertex3f(x0 + dx, y0, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, tdy);
    }

    glVertex3f(x0, y0, z0 + dz);

    // Front Face
    tx0 = getRnd();
    ty0 = getRnd();
    tdx = tx0 + dz / DT;
    tdy = ty0 + dx / DT;
    nv[0] = 0;
    nv[1] = 1;
    nv[2] = 0;
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, ty0);
    }

    glVertex3f(x0, y0 + dy, z0);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, ty0);
    }

    glVertex3f(x0, y0 + dy, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, tdy);
    }

    glVertex3f(x0 + dx, y0 + dy, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, tdy);
    }

    glVertex3f(x0 + dx, y0 + dy, z0);

    // Left Face
    tx0 = getRnd();
    ty0 = getRnd();
    tdx = tx0 + dz / DT;
    tdy = ty0 + dy / DT;
    nv[0] = -1;
    nv[1] = 0;
    nv[2] = 0;
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, ty0);
    }

    glVertex3f(x0, y0, z0);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, ty0);
    }

    glVertex3f(x0, y0, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, tdy);
    }

    glVertex3f(x0, y0 + dy, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, tdy);
    }

    glVertex3f(x0, y0 + dy, z0);

    // Right Face
    tx0 = getRnd();
    ty0 = getRnd();
    tdx = tx0 + dy / DT;
    tdy = ty0 + dz / DT;
    nv[0] = 1;
    nv[1] = 0;
    nv[2] = 0;
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, ty0);
    }

    glVertex3f(x0 + dx, y0, z0);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, ty0);
    }

    glVertex3f(x0 + dx, y0 + dy, z0);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tdx, tdy);
    }

    glVertex3f(x0 + dx, y0 + dy, z0 + dz);
    glNormal3fv(nv);

    if (WithTexture)
    {
        glTexCoord2f(tx0, tdy);
    }

    glVertex3f(x0 + dx, y0, z0 + dz);

    /*
    // The bottom face is never displayed. It increases performance not to draw
    // it.
    // Bottom Face
    tx0 = getRnd();
    ty0 = getRnd();
    tdx = tx0 + dy / DT;
    tdy = ty0 + dx / DT;
    nv[0] = 0;
    nv[1] = 0;
    nv[2] = -1;
    glNormal3fv(nv);
    if (WithTexture)
    {
        glTexCoord2f(tx0, ty0);
    }
    glVertex3f(x0, y0, z0);
    glNormal3fv(nv);
    if (WithTexture)
    {
        glTexCoord2f(tdx, ty0);
    }
    glVertex3f(x0, y0 + dy, z0);
    glNormal3fv(nv);
    if (WithTexture)
    {
       glTexCoord2f(tdx, tdy);
    }
    glVertex3f(x0 + dx, y0 + dy, z0);
    glNormal3fv(nv);
    if (WithTexture)
    {
        glTexCoord2f(tx0, tdy);
    }
    glVertex3f(x0 + dx, y0, z0);
    */
    glEnd();
}

void KPboardView::GetTokenCoordinates(tKPTokenID i, float *x, float *y) const
{
    if (i == animatedToken)
    {
        if (x != NULL)
        {
            *x = ax;
        }

        if (y != NULL)
        {
            *y = ay;
        }
    }
    else
    {
        if (x != NULL)
        {
            *x = ((DX + DGAP) * static_cast<float>(current.GetX(i)) -
                  ((HORIZONTAL_MAX * (DX + DGAP) - DGAP) / 2));
        }

        if (y != NULL)
        {
            *y = ((DY + DGAP) * static_cast<float>(current.GetY(i)) -
                  ((VERTICAL_MAX   * (DY + DGAP) - DGAP) / 2));
        }
    }
}

void KPboardView::Draw(bool render /* = true */) const
{
#ifdef DRAW_TEST
    DrawCuboid(-0.5);
    return;
#else

    GLfloat x0, y0;

    // set material
    glCallList(DisplayList + BRONZE_MATERIAL);

    ///////////////////////////////////////////////////////////////////////
    // Draw all Tokens
    //
    // Loop for drawing all tokens according to their position on the board
    // One token may be in animation mode or not
    ///////////////////////////////////////////////////////////////////////
    auto i = TK_GREEN1;

    do
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glEnable(GL_TEXTURE_2D);

        switch (i)
        {
            case TK_GREEN1:
            case TK_GREEN2:
            case TK_GREEN3:
            case TK_GREEN4:
                glBindTexture(GL_TEXTURE_2D, textureId[0]);
                break;

            case TK_WHITE1:
            case TK_WHITE2:
            case TK_WHITE3:
            case TK_WHITE4:
            case TK_WHITE5:
                glBindTexture(GL_TEXTURE_2D, textureId[1]);
                break;

            case TK_RED1:
                glBindTexture(GL_TEXTURE_2D, textureId[2]);
                break;

            default:
                break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        GetTokenCoordinates(i, &x0, &y0);

        if (emphasizedToken == i)
        {
            continue;
        }

        glTranslatef(x0, y0, 0.0);
        glLoadName(i); // needed for Token Selection
        DrawToken(i);
        glTranslatef(-x0, -y0, 0.0);
    }
    while (++i != TK_GREEN1);

    //////////////////////////////////////
    // Draw the board (after the tokens!)
    //////////////////////////////////////
    glBindTexture(GL_TEXTURE_2D, textureId[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    if (render)
    {
        glCallList(DisplayList + BOARD);
    }

    ////////////////////////////////////////////////////////////
    // Draw emphasized token separatey with depth buffer set to
    // readonly and blending over texture and emission color
    ////////////////////////////////////////////////////////////
    i = emphasizedToken;

    if (i != TK_EMPTY)
    {
        GetTokenCoordinates(i, &x0, &y0);
        glTranslatef(x0, y0, 0.0);
        glLoadName(i); // needed for Token Selection

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        GLfloat color[] =
        {
            0.6f + 0.4f * mat_value, 0.0f, 0.0f, 1.0f
        };
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color);
        DrawToken(i);

        glTranslatef(-x0, -y0, 0.0);
    }

    // Avoid to bind the texture by another glTexImage2D:
    glBindTexture(GL_TEXTURE_2D, 0);
#endif
    glDisable(GL_TEXTURE_2D);
}

void KPboardView::DrawToken(const tKPTokenID i) const
{
    switch (i)
    {
        case TK_GREEN1:
            glCallList(DisplayList + CUBOID_1X1_1);
            break;

        case TK_GREEN2:
            glCallList(DisplayList + CUBOID_1X1_2);
            break;

        case TK_GREEN3:
            glCallList(DisplayList + CUBOID_1X1_3);
            break;

        case TK_GREEN4:
            glCallList(DisplayList + CUBOID_1X1_4);
            break;

        case TK_WHITE1:
            glCallList(DisplayList + CUBOID_1X2_1);
            break;

        case TK_WHITE2:
            glCallList(DisplayList + CUBOID_1X2_2);
            break;

        case TK_WHITE3:
            glCallList(DisplayList + CUBOID_1X2_3);
            break;

        case TK_WHITE4:
            glCallList(DisplayList + CUBOID_1X2_4);
            break;

        case TK_WHITE5:
            glCallList(DisplayList + CUBOID_2X1);
            break;

        case TK_RED1:
            glCallList(DisplayList + CUBOID_2X2);
            break;

        default:
            break;
    }
}

// duration is the time since the last call to Animate() in ms
void KPboardView::Animate(unsigned int duration)
{
    xValue +=  duration * 2 * M_PIf / TOTAL_ANIMATIONTIME;

    if (xValue >= 2 * M_PI)
    {
        xValue -= 2 * M_PIf;
    }

    mat_value = 0.5f * sin(xValue) + 0.5f;

    if (animatedToken == TK_EMPTY)
    {
        return;
    }

    Time += 2 * duration;

    if (Time >= TOTAL_ANIMATIONTIME)
    {
        animatedToken = TK_EMPTY;
    }
    else
    {
        auto localFactor = 0.5f - 0.5f *
                           cos(M_PIf * Time / TOTAL_ANIMATIONTIME);
        ax = (new_x - old_x) * localFactor + old_x;
        ay = (new_y - old_y) * localFactor + old_y;
    }
}

bool KPboardView::CanMove(tKPTokenID id, tKPDirection d)
{
    return current.CanMove(id, d);
}

bool KPboardView::Move(tKPTokenID id, tKPDirection d)
{
    if (animatedToken != TK_EMPTY)
    {
        return false;    // Ignore if Animation in progress
    }

    auto possibleMoves = 0;
    auto direction = MOVE_NO;

    // count the possibilities token id can move to in i
    if (current.CanMove(id, MOVE_UP))
    {
        possibleMoves++;
        direction = MOVE_UP;
    }

    if (current.CanMove(id, MOVE_DOWN))
    {
        possibleMoves++;
        direction = MOVE_DOWN;
    }

    if (current.CanMove(id, MOVE_LEFT))
    {
        possibleMoves++;
        direction = MOVE_LEFT;
    }

    if (current.CanMove(id, MOVE_RIGHT))
    {
        possibleMoves++;
        direction = MOVE_RIGHT;
    }

    if (possibleMoves == 0)
    {
        return false;    // token is not movable at all
    }

    if (possibleMoves >= 2)
    {
        // more than one possibility: use mouse motion to get
        // direction
        direction = d;
    }

    if (direction == MOVE_NO)
    {
        return false;    // no direction given. No move possible
    }

    if (!current.CanMove(id, direction))
    {
        return false;    // Ignore if token id can not be moved in direction d
    }

    animatedToken = id;
    Time = 0;
    ax = old_x = ((DX + DGAP) * static_cast<float>(current.GetX(id)) -
                  ((HORIZONTAL_MAX * (DX + DGAP) - DGAP) / 2));
    ay = old_y = ((DY + DGAP) * static_cast<float>(current.GetY(id)) -
                  ((VERTICAL_MAX   * (DY + DGAP) - DGAP) / 2));

    current.Move(id, direction); // do the move

    new_x = ((DX + DGAP) * static_cast<float>(current.GetX(id)) -
             ((HORIZONTAL_MAX * (DX + DGAP) - DGAP) / 2));
    new_y = ((DY + DGAP) * static_cast<float>(current.GetY(id)) -
             ((VERTICAL_MAX   * (DY + DGAP) - DGAP) / 2));

    return true;
}

bool KPboardView::IsSolved()
{
    return current.IsSolved();
}

#ifdef DRAW_TEST
void KPboardView::DrawCuboid(float deltaAngle) const// Only for test purposes
{
    static auto rquad = 0.0f;
    auto without_camera = 0;  // switch to 1 to test without any camera
    // properties

    glPushMatrix();
    //glDisable(GL_LIGHTING);
    //glMatrixMode(GL_PROJECTION); // Kameraparameter!
    //glLoadIdentity();            // zuruecksetzen
    //gluPerspective(38.6f,800.0f/600.0f,1.0f, 500.0f);

    //glMatrixMode(GL_MODELVIEW);
    if (without_camera)
    {
        glLoadIdentity();
        glTranslatef(1.5f, 0.0f, -100.0f);     // Move Right And Into The Screen
        glRotatef(rquad, 1.0f, 1.0f, 1.0f);    // Rotate The Cube On X, Y & Z
    }

    glCallList(DisplayList + CUBOID_1X1_1);

    if (without_camera)
    {
        rquad -= deltaAngle;                   // Decrease The Rotation Variable
        // For The Quad
    }

    glPopMatrix();
}
#else
void KPboardView::DrawCuboid(float /* deltaAngle */)
const // Only for test purposes
{
}
#endif

#define BUF_SIZE 256

// Check if a token has been selected
// If no token was selected return TK_EMPTY

tKPTokenID KPboardView::Selection(const Camera *pCamera, int x, int y) const
{
    GLuint  buffer[BUF_SIZE]; // Set Up A Selection Buffer
    auto choose = TK_EMPTY;

    glSelectBuffer(BUF_SIZE, buffer);
    glRenderMode(GL_SELECT); // Change to Selection mode
    glInitNames();
    glPushName(0); // Push 0 (At Least One Entry) Onto The Stack

    pCamera->Draw(x, y);
    Draw(true); // Draw the tokens (not the board)

    auto hits = glRenderMode(GL_RENDER);  // Change back to render mode

    if (hits > 0)
    {
        choose = static_cast<tKPTokenID>(buffer[3]);
        auto depth = buffer[1];

        for (auto loop = 1; loop < hits; loop++)
        {
            if (buffer[loop * 4 + 1] < GLuint(depth))
            {
                // store the closest object
                choose = static_cast<tKPTokenID>(buffer[loop * 4 + 3]);
                depth  = buffer[loop * 4 + 1];
            }
        }
    }

    return choose;
}

