/*******************************************************************************
**
** extendedcamera.h    Copyright (C) 2001 by Stefan Disch, Tobias Nopper,
**                     Martina Welte 
**                     Modified 2002-2016 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef __extendedcamera_h__
#define __extendedcamera_h__

#include <vector>
#include "camera.h"


class ExtendedCamera : public Camera
{
public:
    ExtendedCamera();
    void MoveForward(float);
    void MoveBackward(float);
    void MoveRight(float);
    void MoveLeft(float);
    void MoveUp(float);
    void MoveDown(float);
    void ZoomIn(float);
    void ZoomOut(float);
    void VertigoIn(float);
    void VertigoOut(float);
    void RotateRight(float);
    void RotateLeft(float);
    void RotateUp(float);
    void RotateDown(float);
    void PanRight(float,float,float);
    void PanLeft(float,float,float);
    void PanUp(float,float,float);
    void PanDown(float,float,float);
    void Follow(int);
    void FocusUp(float,float);
    void FocusUp2(float,float);
    void FocusUp3(float,float);
};

#endif
