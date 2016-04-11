/*******************************************************************************
**
** extendedcamera.cpp   Copyright (C) 2001 by Stefan Disch, Tobias Nopper,
**                      Martina Welte
**                      Modified 2002-2016 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#include "stdafx.h"
#include <GL/gl.h>
#include <math.h>
#include "extendedcamera.h"


ExtendedCamera::ExtendedCamera() : Camera()
{
}

// Move the camera towards the view direction. Does not change the height.
void ExtendedCamera::MoveForward(float factor)
{
    target_Pos_x += 2 * MoveFactor * factor * sin(target_Beta / degprad);
    target_Pos_y += 2 * MoveFactor * factor * cos(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera away from the view direction. Does not change the height.
void ExtendedCamera::MoveBackward(float factor)
{
    target_Pos_x -= 2 * MoveFactor * factor * sin(target_Beta / degprad);
    target_Pos_y -= 2 * MoveFactor * factor * cos(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera to the right. Does not change the view direction.
void ExtendedCamera::MoveRight(float factor)
{
    target_Pos_x += MoveFactor * factor * cos(target_Beta / degprad);
    target_Pos_y -= MoveFactor * factor * sin(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera to the left. Does not change the view direction.
void ExtendedCamera::MoveLeft(float factor)
{
    target_Pos_x -= MoveFactor * factor * cos(target_Beta / degprad);
    target_Pos_y += MoveFactor * factor * sin(target_Beta / degprad);
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera to the up. Does not change the view direction.
void ExtendedCamera::MoveUp(float factor)
{
    target_Pos_z += MoveFactor * factor;
    //  if (Pos_z>400) {Pos_z=400;}
    RecalculateViewFrustum();
    IsRoundtrip = false;
}

// Move the camera to the down. Does not change the view direction.
void ExtendedCamera::MoveDown(float factor)
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
void ExtendedCamera::ZoomIn(float factor)
{
    target_FOV -= MoveFactor * factor;

    if (target_FOV < 1)
    {
        target_FOV = 1;
    }

    IsRoundtrip = false;
}

// Increases the field of view angle, decreases the zoom factor.
void ExtendedCamera::ZoomOut(float factor)
{
    target_FOV += MoveFactor * factor;
    IsRoundtrip = false;
}

// Increases the vertigo
void ExtendedCamera::VertigoIn(float factor)
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
void ExtendedCamera::VertigoOut(float factor)
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
void ExtendedCamera::RotateRight(float factor)
{
    target_Beta += factor * RotateFactor;
    IsRoundtrip = false;
}

// Rotates the cameras view direction to the left. Keeps the camera position.
void ExtendedCamera::RotateLeft(float factor)
{
    target_Beta -= factor * RotateFactor;
    IsRoundtrip = false;
}

// Rotates the cameras view direction to the up. Keeps the camera position.
void ExtendedCamera::RotateUp(float factor)
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
void ExtendedCamera::RotateDown(float factor)
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

void ExtendedCamera::PanLeft(float factor, float center_x, float center_y)
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

void ExtendedCamera::PanRight(float factor, float center_x, float center_y)
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

void ExtendedCamera::PanUp(float factor, float center_x, float center_y)
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

void ExtendedCamera::PanDown(float factor, float center_x, float center_y)
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

void ExtendedCamera::FocusUp(float focus_x, float focus_y)
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

void ExtendedCamera::FocusUp2(float focus_x, float focus_y)
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

void ExtendedCamera::FocusUp3(float focus_x, float focus_y)
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

    if (tgtPosy > focus_y)
    {
        tgtBeta -= 180.0f;
    }

    GLfloat tgtFOV = 38.6f;

    SetTargetPosition(tgtPosx, tgtPosy, tgtPosz, tgtAlpha, tgtBeta, tgtFOV);
    IsRoundtrip = false;
}
