/*
    kpprogressbar.cpp

    Automatic solution finder for KhunPhan game
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

#include "misc1.h"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <math.h>
#ifdef HAVE_STDLIB_H
/* on some systems stdio.h is a prerequisite to stdlib.h */
#include <stdio.h>
#include <stdlib.h>
#endif
#include "kpprogressbar.h"

#define PROGRESS_MARGIN (0.1f)


KPprogressBar::KPprogressBar() : InAnimation(false),
    x(0),      old_x(0),      target_x(0),
    y(0),      old_y(0),      target_y(0),
    alpha(0),  old_Alpha(0),  target_Alpha(0),
    height(0), old_Height(0), target_Height(0),
    width(0),  old_Width(0),  target_Width(0),
    Time(0),   percent(0),    displayList(0)
{
    barColor[0] = 1.0;
    barColor[1] = 1.0;
    barColor[2] = 1.0;
}

KPprogressBar::~KPprogressBar()
{
    if (displayList)
    {
        glDeleteLists(displayList, 2);
    }
    displayList = 0;
}

void KPprogressBar::Initialize()
{
}

void KPprogressBar::Blended()
{
    target_Alpha = MOD_EINGEBLENDET;
    if (target_Alpha != alpha)
    {
        StartAnimation();
    }
}

void KPprogressBar::FullVisible()
{
    target_Alpha = MOD_VOLLSICHTBAR;
    if (target_Alpha != alpha)
    {
        StartAnimation();
    }
}

void KPprogressBar::StartAnimation()
{
    InAnimation = true;
    Time        = 0;
    old_x       = x;
    old_y       = y;
    old_Height  = height;
    old_Width   = width;
    old_Alpha   = alpha;
}

void KPprogressBar::SetPosition(float X, float Y, float Width, float Height,
                                tKPAlignment Alignment)
{
    switch (Alignment)
    {
        case A_LINKS:
        {
            target_x = X;
        }
        break;
        case A_MITTE:
        {
            target_x = X - Width * 0.5f;
        }
        break;
        case A_RECHTS:
        {
            target_x = X - Width;
        }
        break;
    }

    target_y     = Y;
    target_Height= Height;
    target_Width = Width;
    target_Alpha = MOD_EINGEBLENDET;

    if (alpha == MOD_AUSGEBLENDET)
    {
        x      = target_x      / 1.5f;
        y      = target_y      / 1.5f;
        height = target_Height / 1.5f;
        width  = target_Width  / 1.5f;
    }
    GenerateDisplayList();

    StartAnimation();
}

void KPprogressBar::Deactivate()
{
    target_Alpha = MOD_AUSGEBLENDET;

    target_x      = x      * 1.5f;
    target_y      = y      * 1.5f;
    target_Height = height * 1.5f;
    target_Width  = width  * 1.5f;

    if (target_Alpha != alpha)
    {
        StartAnimation();
    }
}

bool KPprogressBar::Animate(int factor)
{
    if (!InAnimation)
    {
        return false;
    }

    Time += factor;
    if (Time >= TOTAL_ANIMATIONTIME)
    {
        x = target_x;
        y = target_y;
        height = target_Height;
        width  = target_Width;
        alpha  = target_Alpha;
        InAnimation = false;
        return true;
    }
    else
    {
        GLfloat factor = 0.5f - 0.5f * cos(M_PIf * Time / TOTAL_ANIMATIONTIME);
        x = (target_x - old_x) * factor + old_x;
        y = (target_y - old_y) * factor + old_y;
        height = (target_Height - old_Height) * factor + old_Height;
        width  = (target_Width  - old_Width)  * factor + old_Width;
        alpha  = (target_Alpha  - old_Alpha)  * factor + old_Alpha;
        return false;
    }
}

void KPprogressBar::SetPercentage(int Percent)
{
    if (abs(Percent - percent) > 1)
    {
        percent = Percent;
        GenerateDisplayList();
    }
}

void KPprogressBar::GenerateDisplayList()
{
    GLfloat mw = height * PROGRESS_MARGIN;

    if (displayList == 0)
    {
        displayList = glGenLists(2);
    }

    glNewList(displayList, GL_COMPILE);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(0.0,        0.0,         0.0);
    glVertex3f(mw,         0.0,         0.0);
    glVertex3f(mw,         height,      0.0);
    glVertex3f(0.0,        height,      0.0);

    glVertex3f(width - mw, 0.0,         0.0);
    glVertex3f(width,      0.0,         0.0);
    glVertex3f(width,      height,      0.0);
    glVertex3f(width - mw, height,      0.0);

    glVertex3f(mw,         0.0,         0.0);
    glVertex3f(width - mw, 0.0,         0.0);
    glVertex3f(width - mw, mw,          0.0);
    glVertex3f(mw,         mw,          0.0);

    glVertex3f(mw,         height - mw, 0.0);
    glVertex3f(width - mw, height - mw, 0.0);
    glVertex3f(width - mw, height,      0.0);
    glVertex3f(mw,         height,      0.0);
    glEnd();
    glPopMatrix();
    glEndList();

    glNewList(displayList + 1, GL_COMPILE);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(mw,                                  mw,          0.0);
    glVertex3f(mw + (width - 2*mw) * percent / 100, mw,          0.0);
    glVertex3f(mw + (width - 2*mw) * percent / 100, height - mw, 0.0);
    glVertex3f(mw,                                  height - mw, 0.0);
    glEnd();
    glPopMatrix();
    glEndList();
}

void KPprogressBar::Draw()
{
    if (alpha > 0.0 && percent > 0.0)
    {
        glPushMatrix();
        glTranslatef(x, y, 0);
        glColor4f(1.0, 1.0, 1.0, alpha);
        glCallList(displayList);
        barColor[3] = alpha;
        glColor4fv(barColor);
        glCallList(displayList + 1);
        glPopMatrix();
    }
}

void KPprogressBar::SetBarColor(float R, float G, float B)
{
    barColor[0] = R;
    barColor[1] = G;
    barColor[2] = B;
}
