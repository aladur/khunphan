/*******************************************************************************
**
** plate.cpp   Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2015 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#endif
#include <GL/gl.h>
#include <math.h>
#include <limits.h>
#include "misc1.h"
#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>  // needed for access
#endif
#include <stdexcept>
#include "kpconfig.h"
#include "plate.h"
#include "btexture.h"
#include "kpuibase.h"


Plate::Plate(float R /*= 1.0*/,  float G /*= 1.0*/, float B /*= 1.0*/) :
    DisplayList(0), Type(0), AspectRatio(0.0),
    ax(0), ay(0), bx(0), by(0), Alpha(0),
    target_ax(0), target_ay(0), target_bx(0), target_by(0), target_Alpha(0),
    old_ax(0),  old_ay(0), old_bx(0),  old_by(0), old_Alpha(0),
    InAnimation(0), Signal(0), Time(0),
    r(R), g(G), b(B), Texture(0), TextureSize(0), Nearest(false),
    WithAlpha(false)
{
    Type = 3;

    DisplayList = glGenLists(1);
    if (DisplayList == 0)
    {
        // Could be caused if display list is totally full
        // or any other error.
        throw std::runtime_error("Error creating a display list");
    }

    ax = old_ax = target_ax = 8;
    ay = old_ay = target_ay = 6;
    bx = old_bx = target_bx = 8.1f;
    by = old_by = target_by = 6.1f;

    Alpha = old_Alpha = target_Alpha = MOD_FADEOUT;

    RecreateDisplayList();
}

Plate::Plate(const Plate &src) :
    DisplayList(0), Type(src.Type), AspectRatio(src.AspectRatio),
    ax(src.ax), ay(src.ay), bx(src.bx), by(src.by), Alpha(src.Alpha),
    target_ax(src.target_ax), target_ay(src.target_ay),
    target_bx(src.target_bx), target_by(src.target_by),
    target_Alpha(src.target_Alpha),
    old_ax(src.old_ax),  old_ay(src.old_ay),
    old_bx(src.old_bx),  old_by(src.old_by), old_Alpha(src.old_Alpha),
    InAnimation(src.InAnimation), Signal(src.Signal), Time(src.Time),
    r(src.r), g(src.g), b(src.b), TextureSource(src.TextureSource),
    Texture(0), TextureSize(src.TextureSize), Nearest(src.Nearest),
    WithAlpha(src.WithAlpha)
{
    BTexture texture;
    BTexture *pTexture = NULL;

    DisplayList = glGenLists(1);
    if (DisplayList == 0)
    {
        // Could be caused if display list is totally full
        // or any other error.
        throw std::runtime_error("Error creating a display list");
    }

    if (Type == 1)
    {
        const char *texels;

        if ((texels = texture.ReadTextureFromFile(TextureSource.c_str(),
            WithAlpha ? TEX_RGB_ALPHA : TEX_RGB)) == NULL)
        {
            message(mtError, "*** Error reading texture from file '%s'\n",
            TextureSource.c_str());
            exit(1);
        }

        pTexture = &texture;
    }

    RecreateDisplayList(pTexture);
}

Plate &Plate::operator=(const Plate &src)
{
    if (&src != this)
    {
        BTexture texture;
        BTexture *pTexture = NULL;

        Type = src.Type;
        AspectRatio = src.AspectRatio;
        ax = src.ax;
        ay = src.ay;
        bx = src.bx;
        by = src.by;
        Alpha = src.Alpha;
        target_ax = src.target_ax;
        target_ay = src.target_ay;
        target_bx = src.target_bx;
        target_by = src.target_by;
        target_Alpha = src.target_Alpha;
        old_ax = src.old_ax;
        old_ay = src.old_ay;
        old_bx = src.old_bx;
        old_by = src.old_by;
        old_Alpha = src.old_Alpha;
        InAnimation = src.InAnimation;
        Signal = src.Signal;
        Time = src.Time;
        r = src.r;
        g = src.g;
        b = src.b;
        TextureSource = src.TextureSource;
        TextureSize = src.TextureSize;
        Nearest = src.Nearest;
        WithAlpha = src.WithAlpha;

        if (Type == 1)
        {
            const char *texels;

            if ((texels = texture.ReadTextureFromFile(TextureSource.c_str(),
                WithAlpha ? TEX_RGB_ALPHA : TEX_RGB)) == NULL)
            {
                message(mtError, "*** Error reading texture from file '%s'\n",
                TextureSource.c_str());
                exit(1);
            }

            pTexture = &texture;
        }

        RecreateDisplayList(pTexture);
    }

    return *this;
}

Plate::~Plate()
{
    if (DisplayList != 0)
    {
        glDeleteLists(DisplayList, 1);
        DisplayList = 0;
    }
    if (Texture != 0)
    {
        glDeleteTextures(1, &Texture);
        Texture = 0;
    }
}

bool Plate::Update(std::string    &TextureName,
                   unsigned int    textureSize,
                   bool            nearest,
                   bool            withAlpha,
                   const char     *Name,
                   const KPConfig &config)
{
    std::string file1, file2;
    const char *texels;
    BTexture texture;

    // The texture size must be a power of 2 (1, 2, 4, 8, ...)
    if (BTexture::GetExpToBase2(textureSize) == -1)
    {
        return false;
    }

    Type = 1;

    Nearest     = nearest;
    WithAlpha   = withAlpha;
    TextureSize = textureSize;

    file1 = config.GetDirectory(KP_TEXTURE_DIR) + TextureName +
            PATHSEPARATORSTRING + Name + ".png";
    file2 = config.GetDirectory(KP_TEXTURE_DIR) + Name + ".png";

    if ((texels = texture.ReadTextureFromFile(file1.c_str(),
                  withAlpha ? TEX_RGB_ALPHA : TEX_RGB)) == NULL)
    {
        if ((texels = texture.ReadTextureFromFile(file2.c_str(),
                      withAlpha ? TEX_RGB_ALPHA : TEX_RGB)) == NULL)
        {
            message(mtError, "*** Error reading texture from file '%s'\n",
                    file2.c_str());
            exit(1);
        }
        else
        {
            TextureSource = file2;
        }
    }
    else
    {
        TextureSource = file1;
    }

    RecreateDisplayList(&texture);

    return true;
}

void Plate::RecreateDisplayList(BTexture *pTexture /* = NULL */)
{
    const char *texels;
    unsigned int width;
    unsigned int height;

    switch (Type)
    {
        case 1:
	    texels = pTexture->Rescale(BTexture::GetExpToBase2(TextureSize),
				       TEX_SMALLER | TEX_RESCALE_AVG);

	    width  = pTexture->GetWidth();
	    height = pTexture->GetHeight();

	    if (!BTexture::IsPowerOf2(width) || !BTexture::IsPowerOf2(height))
	    {
                message(mtWarning,
                "*** Warning: width or height of '%s' is not a power of 2\n",
                TextureSource.c_str());
	    }

	    if (Texture == 0)
	    {
		glGenTextures(1, &Texture);
	    }

	    glBindTexture(GL_TEXTURE_2D, Texture);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY,   1.0);

	    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			    (Nearest ? GL_NEAREST : GL_LINEAR));
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			    (Nearest ? GL_NEAREST : GL_LINEAR));

	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
			 WithAlpha ? GL_RGBA : GL_RGB,
                         GL_UNSIGNED_BYTE, texels);

	    glNewList(DisplayList, GL_COMPILE);
	    glBindTexture(GL_TEXTURE_2D, Texture);
	    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	    glBegin(GL_QUADS);
	    glTexCoord2f(0.0,0.0);
	    glVertex2f(0.0,0.0);
	    glTexCoord2f(1.0,0.0);
	    glVertex2f(1.0,0.0);
	    glTexCoord2f(1.0,1.0);
	    glVertex2f(1.0,1.0);
	    glTexCoord2f(0.0,1.0);
	    glVertex2f(0.0,1.0);
	    glEnd();
	    glEndList();

	    AspectRatio = (width + 1.0f) / (height + 1.0f);
            break;

        case 3:
	    glNewList(DisplayList, GL_COMPILE);
	    glDisable(GL_TEXTURE_2D);
	    glBegin(GL_QUADS);
	    glVertex2f(0.0,0.0);
	    glVertex2f(1.0,0.0);
	    glVertex2f(1.0,1.0);
	    glVertex2f(0.0,1.0);
	    glEnd();
	    glEndList();

            break;

        default:
            message(mtError, "*** Plate has invalid type '%d'\n", Type);
            exit(1);
    }
}

void Plate::Draw()
{
    if (Alpha)
    {
        GLfloat color[] = {r, g, b, Alpha};
        glPushMatrix();
        glTranslatef(ax,ay,0);
        glScalef(bx-ax,by-ay,1);
        if (Texture != 0)
        {
            glEnable(GL_TEXTURE_2D);
            glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,color);
        }
        glColor4fv(color);
        glCallList(DisplayList);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
}

void Plate::SetPosition(float ax_, float ay_, float bx_, float by_)
{
    // Correction for AspectRatio needed here
    if (AspectRatio != 0.0 && AspectRatio != (bx_ - ax_) / (by_ - ay_))
    {
        GLfloat cx = (ax_ + bx_) * 0.5f;
        ax_=cx-AspectRatio * (by_ - ay_) * 0.5f;
        bx_=cx+AspectRatio * (by_ - ay_) * 0.5f;
    }

    target_ax = ax_;
    target_ay = ay_;
    target_bx = bx_;
    target_by = by_;

    if (Type == 3)
    {
        target_Alpha = MOD_TRANSPARENCY;
    }
    else
    {
        target_Alpha = MOD_FADEIN;
    }

    if (Alpha == MOD_FADEOUT)
    {

        ax = ((target_ax - 8) / 1.5f) + 8;
        ay = ((target_ay - 6) / 1.5f) + 6;
        bx = ((target_bx - 8) / 1.5f) + 8;
        by = ((target_by - 6) / 1.5f) + 6;

        /*
        ax = ((24.0 * rand()) / RAND_MAX) - 4;
        ay = ((14.0 * rand()) / RAND_MAX) - 1;
        bx = ax + 2 * (bx_ - ax_);
        by = ay + 2 * (by_ - ay_);
        */
    }

    Signal = 0;

    StartAnimation();
}

void Plate::SetFadeOut()
{
    target_Alpha = MOD_FADEOUT;

    target_ax = ((ax - 8) * 1.5f) + 8;
    target_ay = ((ay - 6) * 1.5f) + 6;
    target_bx = ((bx - 8) * 1.5f) + 8;
    target_by = ((by - 6) * 1.5f) + 6;

    /*
    target_ax = ((24.0 * rand()) / RAND_MAX) - 4;
    target_ay = ((14.0 * rand()) / RAND_MAX) - 1;
    target_bx = target_ax + 0.5 * (bx - ax);
    target_by = target_ay + 0.5 * (by - ay);
    */

    if (target_Alpha != Alpha)
    {
        StartAnimation();
    }
}

void Plate::SetSelected()
{
    Alpha = MOD_SELECTED;
    target_Alpha = MOD_FADEIN;
    if (target_Alpha != Alpha)
    {
        StartAnimation();
    }
}

void Plate::SetFadeIn()
{
    target_Alpha = MOD_FADEIN;
    if (target_Alpha != Alpha)
    {
        StartAnimation();
    }
}

void Plate::SetFullyVisible()
{
    target_Alpha = MOD_FULLYVISIBLE;
    if (target_Alpha != Alpha)
    {
        StartAnimation();
    }
}

void Plate::StartAnimation()
{
    InAnimation = 1;
    Time = 0;
    old_ax = ax;
    old_ay = ay;
    old_bx = bx;
    old_by = by;
    old_Alpha = Alpha;
}

int Plate::Animate(int factor)
{
    if (!InAnimation)
    {
        return 0;
    }
    Time += factor;
    if (Time >= TOTAL_ANIMATIONTIME)
    {
        ax = target_ax;
        ay = target_ay;
        bx = target_bx;
        by = target_by;
        Alpha = target_Alpha;
        InAnimation = 0;
        return 1;
    }
    else
    {
        GLfloat localFactor;

        localFactor = 0.5f - 0.5f * cos(M_PIf * Time / TOTAL_ANIMATIONTIME);
        ax   = (target_ax - old_ax) * localFactor + old_ax;
        ay   = (target_ay - old_ay) * localFactor + old_ay;
        bx   = (target_bx - old_bx) * localFactor + old_bx;
        by   = (target_by - old_by) * localFactor + old_by;
        Alpha= (target_Alpha - old_Alpha) * localFactor + old_Alpha;
        return 0;
    }
}

int Plate::MouseEvent(tMouseButton button, tMouseEvent event,
                      int x, int y, KPUIBase &ui)
{
    GLfloat xf = 16.0f * x / ui.GetValue(KP_WINDOW_WIDTH);
    GLfloat yf = 12.0f - 12.0f * y / ui.GetValue(KP_WINDOW_HEIGHT);

    if (target_Alpha > 0.0 &&
        Signal !=0 &&
        ax <= xf && xf <= bx && ay <= yf && yf <= by)
    {
        if (button == KP_LEFT_MB)
        {
            if (event == KP_BUTTON_PRESS)
            {
                SetSelected();
                return -1;
            }
            else
            {
                return Signal;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return 0;
    }
}

void Plate::SetSignal(int NewSignal)
{
    Signal = NewSignal;
}
