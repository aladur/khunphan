/*******************************************************************************
**
** camera.cpp  Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2016 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#include "stdafx.h"
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include "camera.h"


// define constant for degree per radian */
const float Camera::degprad = 180.0f / M_PIf;

Camera::Camera() :   AspectRatio(0.0),
    Pos_x(0.0),      Pos_y(0.0),       Pos_z(0.0),
    Alpha(0.0),      Beta(0.0),        FOV(0.0),
    target_Pos_x(0.0), target_Pos_y(0.0),  target_Pos_z(0.0),
    target_Alpha(0.0), target_Beta(0.0),   target_FOV(0.0),
    d_Pos_x(0.0),    d_Pos_y(0.0),     d_Pos_z(0.0),
    d_Alpha(0.0),    d_Beta(0.0),      d_FOV(0.0),
    Near(0.0),        Far(0.0),
    IsRoundtrip(0),   MoveFactor(0.3f), RotateFactor(0.3f)
{
    // Initialize default camera positions with the parameter list:
    //   Pos_x, Pos_y, Pos_z, Alpha, Beta, FOV

    // Position with index 0
    Positions.push_back(SPosition(-100.0f, -50.0f, 50.0f, 60.0f, 60.0f, 38.6f));

    // Position with index 1
    Positions.push_back(SPosition(0.0f, 160.0f, 80.0f, 60.0f, 180.0f, 38.6f));

    // Position with index 2
    Positions.push_back(SPosition(0.0f, 140.0f, 120.0f, 45.0f, 180.0f, 38.6f));

    // Position with index 3
    Positions.push_back(SPosition(0.0f, 115.0f, 180.0f, 30.0f, 180.0f, 38.6f));

    // Position with index 4
    Positions.push_back(SPosition(0.0f, 0.0f, 220.0f, 0.0f, 180.0f, 38.6f));

    // Position with index 5
    Positions.push_back(SPosition(0.0f, 0.0f, 300.0f, 0.0f, 180.0f, 38.6f));

    // Position with index 6
    Positions.push_back(SPosition(-60.f, 160.0f, 140.0f, 45.0f, 155.0f, 38.6f));

    // Position with index 7
    Positions.push_back(SPosition(60.0f, 160.0f, 140.0f, 45.0f, 205.0f, 38.6f));

    target_Pos_x = -60;
    target_Pos_y = -30;

    Roundtrip(0);

    Alpha = target_Alpha;
    Beta  = target_Beta;
    Pos_x = target_Pos_x;
    Pos_y = target_Pos_y;
    Pos_z = target_Pos_z;

    AspectRatio = 4.0f / 3.0f;
    IsRoundtrip = false;
}

void Camera::SetAspectRatio(float aspectRatio)
{
    AspectRatio = aspectRatio;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, AspectRatio, Near, Far);
    glMatrixMode(GL_MODELVIEW);
}

// Move the camera towards the view direction.
// May change x-, y- or z-position.
void Camera::MoveIn(float factor)
{
    target_Pos_x += 2 * MoveFactor * factor * sin(target_Alpha / degprad) *
                    sin(target_Beta / degprad);
    target_Pos_y += 2 * MoveFactor * factor * sin(target_Alpha / degprad) *
                    cos(target_Beta / degprad);
    target_Pos_z -= 2 * MoveFactor * factor * cos(target_Alpha / degprad);

    //  if (Pos_z > 400.0f) {Pos_z = 400.0f;}
    if (target_Pos_z < 2.8f)
    {
        target_Pos_z = 2.8f;
    }

    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera away from the view direction.
// May change x-, y- or z-position.
void Camera::MoveOut(float factor)
{
    target_Pos_x -= 2 * MoveFactor * factor * sin(target_Alpha / degprad) *
                    sin(target_Beta / degprad);
    target_Pos_y -= 2 * MoveFactor * factor * sin(target_Alpha / degprad) *
                    cos(target_Beta / degprad);
    target_Pos_z += 2 * MoveFactor * factor * cos(target_Alpha / degprad);

    //  if (Pos_z > 400.0f) {Pos_z = 400.0f;}
    if (target_Pos_z < 2.8f)
    {
        target_Pos_z = 2.8f;
    }

    RecalculateViewFrustum();
    IsRoundtrip = false;
}

void Camera::SetTargetPosition(SPosition &target_Pos)
{
    target_Pos_x = target_Pos.Pos_x;
    target_Pos_y = target_Pos.Pos_y;
    target_Pos_z = target_Pos.Pos_z;
    target_Alpha = target_Pos.Alpha;
    target_Beta  = target_Pos.Beta;
    target_FOV   = target_Pos.FOV;

    Beta = fmod(Beta, 360.0f);
    target_Beta = fmod(target_Beta, 360.0f);

    if (target_Beta > Beta + 180)
    {
        target_Beta -= 360;
    }

    if (target_Beta < Beta - 180)
    {
        target_Beta += 360;
    }

    RecalculateViewFrustum();
    IsRoundtrip = false;
}

void Camera::SetTargetPosition(float tgtPosx, float tgtPosy, float tgtPosz,
                               float tgtAlpha, float tgtBeta, float tgtFOV)
{
    target_Pos_x = tgtPosx;
    target_Pos_y = tgtPosy;
    target_Pos_z = tgtPosz;
    target_Alpha = tgtAlpha;
    target_Beta  = tgtBeta;
    target_FOV   = tgtFOV;

    Beta = fmod(Beta, 360.0f);
    target_Beta = fmod(target_Beta, 360.0f);

    if (target_Beta > Beta + 180)
    {
        target_Beta -= 360;
    }

    if (target_Beta < Beta - 180)
    {
        target_Beta += 360;
    }

    IsRoundtrip = false;
}

// duration is the time since the last call to Animate() in ms
void Camera::Animate(unsigned int duration)
{
    for (decltype(duration) i = 0; i < duration / 10; i++)
    {

        if (target_Beta - Beta < -180)
        {
            Beta -= 360;
        }

        if (target_Beta - Beta > 180)
        {
            Beta += 360;
        }

        d_Pos_x = d_Pos_x * 0.9f + 0.003f * (target_Pos_x - Pos_x);
        d_Pos_y = d_Pos_y * 0.9f + 0.003f * (target_Pos_y - Pos_y);
        d_Pos_z = d_Pos_z * 0.9f + 0.003f * (target_Pos_z - Pos_z);
        d_Alpha = d_Alpha * 0.9f + 0.003f * (target_Alpha - Alpha);
        d_Beta  = d_Beta  * 0.9f + 0.003f * (target_Beta - Beta);
        d_FOV   = d_FOV   * 0.9f + 0.003f * (target_FOV - FOV);

        Pos_x += d_Pos_x;
        Pos_y += d_Pos_y;
        Pos_z += d_Pos_z;
        Alpha += d_Alpha;
        Beta  += d_Beta;
        FOV   += d_FOV;

        RecalculateViewFrustum();
    }

    if (IsRoundtrip)
    {
        Roundtrip(duration);
    }
}

// This method can be used for both display update and Selection
// Parameters:
//   For Display update: not necessary
//   For Selection:      the X and Y Screen coordinates to select

void Camera::Draw(int x /* = -1 */, int y /* = -1 */) const
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (x >= 0 && y >= 0)        // only necessary in selection mode:
    {
        GLint viewport[4];

        glGetIntegerv(GL_VIEWPORT, viewport);
        gluPickMatrix(x, (viewport[3] - y), 1.0f, 1.0f, viewport);
    }

    gluPerspective(FOV, AspectRatio, Near, Far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            // Set camera to the origin

    glRotatef(Alpha, -1, 0, 0);  // Rotate up by Alpha
    glRotatef(Beta, 0, 0, 1);    // Rotate right by Beta
    glTranslatef(-Pos_x, -Pos_y, -Pos_z); // Move to the target position
}

// Switches to one of the pre-initialized camera positions.
void Camera::SetPosition(int index)
{
    SetTargetPosition(Positions.at(index));
    IsRoundtrip = false;
}

void Camera::RecalculateViewFrustum()
{
    auto ax = fabs(Pos_x);
    auto ay = fabs(Pos_y);
    auto az = Pos_z;

    if (ax < 150)
    {
        if (ay < 80)
        {
            Near = az - 5;
        }
        else
        {
            Near = sqrt((ay - 80) * (ay - 80) + (az - 5) * (az - 5));
        }
    }
    else
    {
        if (ay < 80)
        {
            Near = sqrt((ax - 150) * (ax - 150) + (az - 5) * (az - 5));
        }
        else
        {
            Near = sqrt((ax - 150) * (ax - 150) + (ay - 80) * (ay - 80) +
                        (az - 5) * (az - 5));
        }
    }

    Near *= 0.8f;

    if (Near < 1)
    {
        Near = 1;
    }

    Far = sqrt((ax + 150) * (ax + 150) + (ay + 80) * (ay + 80) + az * az);
}


void Camera::Roundtrip(int duration)
{
    if (target_Pos_y == 0)
    {
        target_Pos_y = 0.00001f;
    }

    //target_Beta = duration * 0.01f +
    //                  atan(target_Pos_x / target_Pos_y) * degprad;
    //if (target_Pos_y > 0) {target_Beta -= 180;}
    target_Beta += 0.01f * duration;
    target_Pos_x = (-30.0f * sin(target_Beta * M_PIf / 180.0f) - 280.0f) *
                   sin(target_Beta * M_PIf / 180.0f);
    target_Pos_y = (-30.0f * sin(target_Beta * M_PIf / 180.0f) - 280.0f) *
                   cos(target_Beta * M_PIf / 180.0f);
    // Sw: unfinished
    //target_Pos_z=100-50*sin(target_Beta*M_PI/180);
    target_Pos_z = 150.0f - 25.0f * sin(target_Beta * M_PIf / 180.0f);
    target_FOV = 36.8f;
    target_Alpha = degprad * atan(sqrt(target_Pos_x * target_Pos_x +
                                       target_Pos_y * target_Pos_y) /
                                  target_Pos_z);
}
