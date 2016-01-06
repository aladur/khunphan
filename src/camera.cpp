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
    IsRoundtrip(0),   MoveFactor(0.3f),RotateFactor(0.3f)
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
    Positions.push_back(SPosition(-60.0f, 160.0f, 140.0f, 45.0f, 155.0f, 38.6f));

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

#ifdef CAMERA_EXTENDED
// Move the camera towards the view direction. Does not change the height.
void Camera::MoveForward(float factor)
{
    target_Pos_x += 2 * MoveFactor * factor * sin(target_Beta / degprad);
    target_Pos_y += 2 * MoveFactor * factor * cos(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera away from the view direction. Does not change the height.
void Camera::MoveBackward(float factor)
{
    target_Pos_x -= 2 * MoveFactor * factor * sin(target_Beta / degprad);
    target_Pos_y -= 2 * MoveFactor * factor * cos(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera to the right. Does not change the view direction.
void Camera::MoveRight(float factor)
{
    target_Pos_x += MoveFactor * factor * cos(target_Beta / degprad);
    target_Pos_y -= MoveFactor * factor * sin(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera to the left. Does not change the view direction.
void Camera::MoveLeft(float factor)
{
    target_Pos_x -= MoveFactor * factor * cos(target_Beta / degprad);
    target_Pos_y += MoveFactor * factor * sin(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera to the up. Does not change the view direction.
void Camera::MoveUp(float factor)
{
    target_Pos_z += MoveFactor * factor;
    //  if (Pos_z>400) {Pos_z=400;}
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera to the down. Does not change the view direction.
void Camera::MoveDown(float factor)
{
    target_Pos_z -= MoveFactor * factor;
    if (target_Pos_z < 2.8f)
    {
        target_Pos_z = 2.8f;
    }
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Decreases the field of view angle, increases the zoom factor.
void Camera::ZoomIn(float factor)
{
    target_FOV -= MoveFactor * factor;
    if (target_FOV < 1)
    {
        target_FOV = 1;
    }
    IsRoundtrip = false;
}

// Increases the field of view angle, decreases the zoom factor.
void Camera::ZoomOut(float factor)
{
    target_FOV += MoveFactor * factor;
    IsRoundtrip = false;
}

// Increases the vertigo
void Camera::VertigoIn(float factor)
{
    target_Pos_x -= 2 * MoveFactor * factor * sin(target_Alpha / degprad) *
                    sin(target_Beta / degprad);
    target_Pos_y -= 2 * MoveFactor * factor * sin(target_Alpha / degprad) *
                    cos(target_Beta / degprad);
    target_Pos_z += 2 * MoveFactor * factor * cos(target_Alpha / degprad);
 
    target_FOV = 2077 * cos(target_Alpha / degprad) / target_Pos_z;
    if (target_FOV < 1)
    {
        target_FOV = 1;
    }
    IsRoundtrip = false;
}

// Decreases the vertigo
void Camera::VertigoOut(float factor)
{
    target_Pos_x += 2 * MoveFactor * factor * sin(target_Alpha / degprad) *
                    sin(target_Beta / degprad);
    target_Pos_y += 2 * MoveFactor * factor * sin(target_Alpha / degprad) *
                    cos(target_Beta / degprad);
    target_Pos_z -= 2 * MoveFactor * factor * cos(target_Alpha / degprad);
    target_FOV = 2077 * cos(target_Alpha / degprad) / target_Pos_z;
    IsRoundtrip = false;
}

// Rotates the cameras view direction to the right. Keeps the camera position.
void Camera::RotateRight(float factor)
{
    target_Beta += factor * RotateFactor;
    IsRoundtrip = false;
}

// Rotates the cameras view direction to the left. Keeps the camera position.
void Camera::RotateLeft(float factor)
{
    target_Beta -= factor * RotateFactor;
    IsRoundtrip = false;
}

// Rotates the cameras view direction to the up. Keeps the camera position.
void Camera::RotateUp(float factor)
{
    target_Alpha += factor * RotateFactor;
    if (target_Alpha > 90)
    {
        target_Alpha = 90;
    }
    if (target_Alpha < 0)
    {
        target_Alpha = 0;
    }
    IsRoundtrip = false;
}

// Rotates the cameras view direction to the down. Keeps the camera position.
void Camera::RotateDown(float factor)
{
    target_Alpha -= factor * RotateFactor;
    if (target_Alpha > 90)
    {
        target_Alpha = 90;
    }
    if (target_Alpha < 0)
    {
        target_Alpha = 0;
    }
    IsRoundtrip = false;
}

void Camera::PanLeft(float factor, float center_x, float center_y)
{
    GLfloat distance;

    distance = sqrt((center_x - target_Pos_x) * (center_x - target_Pos_x) +
                    (center_y - target_Pos_y) * (center_y - target_Pos_y));

    if (distance < 5)
    {
        distance = 5;
    }

    target_Beta += factor * RotateFactor * 7.338f / sqrt(distance);
    target_Pos_x = center_x - distance * sin(target_Beta / degprad);
    target_Pos_y = center_y - distance * cos(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

void Camera::PanRight(float factor, float center_x, float center_y)
{
    GLfloat distance;

    distance = sqrt((center_x - target_Pos_x) * (center_x - target_Pos_x) +
                    (center_y - target_Pos_y) * (center_y - target_Pos_y));

    if (distance < 5)
    {
        distance = 5;
    }

    target_Beta -= factor * RotateFactor * 7.338f / sqrt(distance);
    target_Pos_x = center_x - distance * sin(target_Beta / degprad);
    target_Pos_y = center_y - distance * cos(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

void Camera::PanUp(float factor, float center_x, float center_y)
{
    GLfloat distance;

    distance = sqrt((center_x - target_Pos_x) * (center_x - target_Pos_x) +
                    (center_y - target_Pos_y) * (center_y - target_Pos_y) +
                    (target_Pos_z - 2.8f) * (target_Pos_z - 2.8f));

    if (distance == 0)
    {
        target_Alpha = 0;
    }
    else
    {
        target_Alpha += factor * RotateFactor * 7.338f / sqrt(distance);

        if (target_Alpha < 0)
        {
            target_Alpha = 0;
        }
        if (target_Alpha > 90)
        {
            target_Alpha = 90;
        }
    }

    target_Pos_x = center_x - distance * sin(target_Beta / degprad) *
                                         sin(target_Alpha / degprad);
    target_Pos_y = center_y - distance * cos(target_Beta / degprad) *
                                         sin(target_Alpha / degprad);
    target_Pos_z = 2.8f + distance * cos(target_Alpha / degprad);
    //  if (Pos_z > 400.0f) {Pos_z = 400.0f;}
    if (target_Pos_z < 2.8f)
    {
        target_Pos_z = 2.8f;
    }
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

void Camera::PanDown(float factor, float center_x, float center_y)
{
    GLfloat distance;

    distance = sqrt((center_x - target_Pos_x) * (center_x - target_Pos_x) +
                    (center_y - target_Pos_y) * (center_y - target_Pos_y) +
                    (target_Pos_z - 2.8f) * (target_Pos_z - 2.8f));

    if (distance == 0)
    {
        target_Alpha = 0;
    }
    else
    {
        target_Alpha -= factor * RotateFactor * 7.338f / sqrt(distance);

        if (target_Alpha < 0)
        {
            target_Alpha = 0;
        }
        if (target_Alpha > 90)
        {
            target_Alpha = 90;
        }
    }

    target_Pos_x = center_x - distance * sin(target_Beta / degprad) *
                                         sin(target_Alpha / degprad);
    target_Pos_y = center_y - distance * cos(target_Beta / degprad) *
                                         sin(target_Alpha / degprad);

    target_Pos_z = 2.8f + distance * cos(target_Alpha / degprad);
    //  if (Pos_z > 400.0f) {Pos_z = 400.0f;}
    if (target_Pos_z < 2.8f)
    {
        target_Pos_z = 2.8f;
    }
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

void Camera::FocusUp(float focus_x, float focus_y)
{
    GLfloat tgtPosx = focus_x + 50 * (Pos_x - focus_x) /
                      sqrt((focus_x - Pos_x) * (focus_x - Pos_x) +
                           (focus_y - Pos_y) * (focus_y - Pos_y));
    GLfloat tgtPosy = focus_y + 50 * (Pos_y - focus_y) /
                      sqrt((focus_x - Pos_x) * (focus_x - Pos_x) +
                           (focus_y - Pos_y) * (focus_y - Pos_y));
    GLfloat tgtPosz = 20;
    GLfloat tgtAlpha = 71.0167f;
    GLfloat tgtBeta = degprad * atan((focus_x - tgtPosx) / (focus_y - tgtPosy));
    if (tgtPosy > focus_y)
    {
        tgtBeta -= 180.0f;
    }
    GLfloat tgtFOV = 38.6f;

    SetTargetPosition(tgtPosx, tgtPosy, tgtPosz, tgtAlpha, tgtBeta, tgtFOV);
    IsRoundtrip = false;
}

void Camera::FocusUp2(float focus_x, float focus_y)
{
    GLfloat tgtPosx = focus_x + 80 * (Pos_x - focus_x) /
                      sqrt((focus_x - Pos_x) * (focus_x - Pos_x) +
                           (focus_y - Pos_y) * (focus_y - Pos_y));
    GLfloat tgtPosy = focus_y + 80 * (Pos_y - focus_y) /
                      sqrt((focus_x - Pos_x) * (focus_x - Pos_x) +
                           (focus_y - Pos_y) * (focus_y - Pos_y));
    GLfloat tgtPosz = 50;
    GLfloat tgtAlpha = 72;
    GLfloat tgtBeta = degprad * atan((focus_x - tgtPosx) / (focus_y - tgtPosy));
    if (tgtPosy > focus_y)
    {
        tgtBeta -= 180.0f;
    }
    GLfloat tgtFOV = 38.6f;

    SetTargetPosition(tgtPosx, tgtPosy, tgtPosz, tgtAlpha, tgtBeta, tgtFOV);
    IsRoundtrip = false;
}

void Camera::FocusUp3(float focus_x, float focus_y)
{
    GLfloat tgtPosx = focus_x + 80 * (Pos_x - focus_x) /
                      sqrt((focus_x - Pos_x) * (focus_x - Pos_x) +
                           (focus_y - Pos_y) * (focus_y - Pos_y));
    GLfloat tgtPosy = focus_y + 80 * (Pos_y - focus_y) /
                      sqrt((focus_x - Pos_x) * (focus_x - Pos_x) +
                           (focus_y - Pos_y) * (focus_y - Pos_y));
    GLfloat tgtPosz = 50;
    GLfloat tgtAlpha = 58;
    GLfloat tgtBeta = degprad * atan((focus_x - tgtPosx) / (focus_y - tgtPosy));
    if (tgtPosy>focus_y)
    {
        tgtBeta -= 180.0f;
    }
    GLfloat tgtFOV = 38.6f;

    SetTargetPosition(tgtPosx, tgtPosy, tgtPosz, tgtAlpha, tgtBeta, tgtFOV);
    IsRoundtrip = false;
}
#endif // #ifdef CAMERA_EXTENDED

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

void Camera::SetTargetPosition(float tgtPosx, float tgtPosy,float tgtPosz,
                               float tgtAlpha,float tgtBeta,float tgtFOV)
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

void Camera::Run(int factor)
{
    for (int i=0; i < factor; i++)
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
        Roundtrip(factor);
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
        GLint   viewport[4];

        glGetIntegerv(GL_VIEWPORT, viewport);
        gluPickMatrix(x, (viewport[3]- y), 1.0f, 1.0f, viewport);
    }
    gluPerspective(FOV, AspectRatio, Near, Far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            // Set camera to the origin

    glRotatef(Alpha,-1,0,0);     // Rotate up by Alpha
    glRotatef(Beta,0,0,1);       // Rotate right by Beta
    glTranslatef(-Pos_x,-Pos_y,-Pos_z); // Move to the target position
}

// Switches to one of the pre-initialized camera positions.
void Camera::SetPosition(int index)
{
    SetTargetPosition(Positions.at(index));
    IsRoundtrip = false;
}

void Camera::RecalculateViewFrustum()
{
    GLfloat ax = fabs(Pos_x);
    GLfloat ay = fabs(Pos_y);
    GLfloat az = Pos_z;

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


void Camera::Roundtrip(int factor)
{
    if (target_Pos_y == 0)
    {
        target_Pos_y = 0.00001f;
    }

    //target_Beta = factor * 0.1f + atan(target_Pos_x / target_Pos_y) * degprad;
    //if (target_Pos_y > 0) {target_Beta -= 180;}
    target_Beta += 0.1f * factor;
    target_Pos_x = (-30.0f * sin(target_Beta * M_PIf / 180.0f) - 280.0f) *
                    sin(target_Beta * M_PIf / 180.0f);
    target_Pos_y = (-30.0f * sin(target_Beta * M_PIf / 180.0f) - 280.0f) *
                    cos(target_Beta * M_PIf / 180.0f);
    // Sw: unfinished
    //target_Pos_z=100-50*sin(target_Beta*M_PI/180);
    target_Pos_z = 150.0f - 25.0f * sin(target_Beta * M_PIf / 180.0f);
    target_FOV = 36.8f;
    target_Alpha = degprad * atan(sqrt(target_Pos_x * target_Pos_x +
                   target_Pos_y * target_Pos_y) / target_Pos_z);
}
