/*******************************************************************************
**
** plate.cpp   Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2024 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#include "stdafx.h"
#include <GL/gl.h>
#include <math.h>
#include <limits.h>
#include <stdexcept>
#include "kpconfig.h"
#include "plate.h"
#include "btexture.h"
#include "kpuibase.h"
#include "kpsignals.h"


Plate::Plate(float R /*= 1.0*/,  float G /*= 1.0*/, float B /*= 1.0*/) :
    DisplayList(0), Type(3), AspectRatio(0.0),
    ax(8), ay(6), bx(8.1f), by(6.1f), Alpha(MOD_FADEOUT),
    target_ax(8), target_ay(6), target_bx(8.1f), target_by(6.1f),
    target_Alpha(MOD_FADEOUT),
    old_ax(8), old_ay(6), old_bx(8.1f), old_by(6.1f), old_Alpha(MOD_FADEOUT),
    signal(Signal::Void),
    r(R), g(G), b(B), Texture(0), TextureSize(0), Nearest(false),
    WithAlpha(false), animationTimer(TOTAL_ANIMATIONTIME, false)
{
    DisplayList = glGenLists(1);

    if (DisplayList == 0)
    {
        // Could be caused if display list is totally full
        // or any other error.
        throw std::runtime_error("Error creating a display list");
    }

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
    signal(src.signal),
    r(src.r), g(src.g), b(src.b), TextureSource(src.TextureSource),
    Texture(0), TextureSize(src.TextureSize), Nearest(src.Nearest),
    WithAlpha(src.WithAlpha), animationTimer(src.animationTimer)
{
    BTexture *pTexture = nullptr;

    DisplayList = glGenLists(1);

    if (DisplayList == 0)
    {
        // Could be caused if display list is totally full
        // or any other error.
        throw std::runtime_error("Error creating a display list");
    }

    if (Type == 1)
    {
        BTexture texture;
        auto flags = WithAlpha ? TEX_RGB_ALPHA : TEX_RGB;
        auto *texels = texture.ReadTextureFromFile(TextureSource, flags);

        if (texels == nullptr)
        {
            std::stringstream message;

            message << "Error reading texture from file '"
                    << TextureSource << "'";
            throw std::runtime_error(message.str());
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
        BTexture *pTexture = nullptr;

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
        signal = src.signal;
        r = src.r;
        g = src.g;
        b = src.b;
        TextureSource = src.TextureSource;
        TextureSize = src.TextureSize;
        Nearest = src.Nearest;
        WithAlpha = src.WithAlpha;
        animationTimer = src.animationTimer;

        if (Type == 1)
        {
            auto flags = WithAlpha ? TEX_RGB_ALPHA : TEX_RGB;
            auto *texels = texture.ReadTextureFromFile(TextureSource, flags);

            if (texels == nullptr)
            {
                std::stringstream message;

                message << "Error reading texture from file '"
                        << TextureSource << "'";
                throw std::runtime_error(message.str());
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

    auto file1 = config.Get(KPDirectory::Texture) + TextureName +
                 PATHSEPARATORSTRING + Name + ".png";

    auto flags = WithAlpha ? TEX_RGB_ALPHA : TEX_RGB;
    auto *texels = texture.ReadTextureFromFile(file1, flags);

    if (texels == nullptr)
    {
        auto file2 = config.Get(KPDirectory::Texture) + Name + ".png";

        texels = texture.ReadTextureFromFile(file2, flags);

        if (texels == nullptr)
        {
            std::stringstream message;

            message << "Error reading texture from file '"
                    << file2 << "'";
            throw std::runtime_error(message.str());
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

void Plate::RecreateDisplayList(BTexture *pTexture /* = nullptr */)
{
    switch (Type)
    {
        case 1:
        {
            if (pTexture == nullptr)
            {
                std::stringstream message;

                message << "Plate has type " << Type
                        << " but pTexture is a nullptr.";
                throw std::runtime_error(message.str());
            }

            auto exp = BTexture::GetExpToBase2(TextureSize);
            auto texels = pTexture->Rescale(exp, TEX_SMALLER | TEX_RESCALE_AVG);
            auto width  = pTexture->GetWidth();
            auto height = pTexture->GetHeight();

            if (!BTexture::IsPowerOf2(width) || !BTexture::IsPowerOf2(height))
            {
                message(MsgType::Warning, "*** Warning: width or height of '",
                        TextureSource, "' is not a power of 2\n");
            }

            if (Texture == 0)
            {
                glGenTextures(1, &Texture);
            }

            glBindTexture(GL_TEXTURE_2D, Texture);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY,   1.0);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0);
            glVertex2f(0.0, 0.0);
            glTexCoord2f(1.0, 0.0);
            glVertex2f(1.0, 0.0);
            glTexCoord2f(1.0, 1.0);
            glVertex2f(1.0, 1.0);
            glTexCoord2f(0.0, 1.0);
            glVertex2f(0.0, 1.0);
            glEnd();
            glEndList();

            AspectRatio = (width + 1.0f) / (height + 1.0f);
            break;
        }

        case 3:
            glNewList(DisplayList, GL_COMPILE);
            glDisable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
            glVertex2f(0.0, 0.0);
            glVertex2f(1.0, 0.0);
            glVertex2f(1.0, 1.0);
            glVertex2f(0.0, 1.0);
            glEnd();
            glEndList();

            break;

        default:
            std::stringstream message;

            message << "Plate has invalid type " << Type;
            throw std::runtime_error(message.str());
    }
}

void Plate::Draw() const
{
    if (Alpha != 0.0f)
    {
        GLfloat color[] = {r, g, b, Alpha};
        glPushMatrix();
        glTranslatef(ax, ay, 0);
        glScalef(bx - ax, by - ay, 1);

        if (Texture != 0)
        {
            glEnable(GL_TEXTURE_2D);
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color);
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
        auto cx = (ax_ + bx_) * 0.5f;
        ax_ = cx - AspectRatio * (by_ - ay_) * 0.5f;
        bx_ = cx + AspectRatio * (by_ - ay_) * 0.5f;
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

    signal = Signal::Void;

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
    animationTimer.Restart();
    old_ax = ax;
    old_ay = ay;
    old_bx = bx;
    old_by = by;
    old_Alpha = Alpha;
}

// duration is the time since the last call to Animate() in ms
void Plate::Animate(unsigned int duration)
{
    if (animationTimer.IsStarted())
    {
        if (animationTimer.Add(duration))
        {
            ax = target_ax;
            ay = target_ay;
            bx = target_bx;
            by = target_by;
            Alpha = target_Alpha;
        }
        else
        {
            auto relativeTime = animationTimer.GetRelativeTime();

            auto localFactor = 0.5f - 0.5f *
                               cos(M_PIf * static_cast<GLfloat>(relativeTime));
            ax   = (target_ax - old_ax) * localFactor + old_ax;
            ay   = (target_ay - old_ay) * localFactor + old_ay;
            bx   = (target_bx - old_bx) * localFactor + old_bx;
            by   = (target_by - old_by) * localFactor + old_by;
            Alpha = (target_Alpha - old_Alpha) * localFactor + old_Alpha;
        }
    }
}

Signal Plate::MouseEvent(MouseButton button, MouseButtonEvent event,
                         int x, int y, KPUIBase &ui)
{
    auto xf = 16.0f * x / ui.Get(WindowProperty::Width);
    auto yf = 12.0f - 12.0f * y / ui.Get(WindowProperty::Height);

    if (target_Alpha > 0.0 &&
        signal != Signal::Void &&
        ax <= xf && xf <= bx && ay <= yf && yf <= by)
    {
        if (button == MouseButton::Left)
        {
            if (event == MouseButtonEvent::Press)
            {
                SetSelected();
            }
            else
            {
                return signal;
            }
        }
    }

    return Signal::Void;
}

void Plate::SetSignal(Signal NewSignal)
{
    signal = NewSignal;
}
