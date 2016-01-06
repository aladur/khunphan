/*******************************************************************************
**
** plate.h     Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2016 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef __PLATE_H__
#define __PLATE_H__


#include "stdafx.h"
#include <string>

class KPUIBase;
class KPConfig;
class BTexture;


class Plate
{
public:
    Plate(float R = 1.0, float G = 1.0, float B = 1.0);
    Plate(const Plate &src);
    virtual ~Plate();
    Plate &operator=(const Plate &src);
    bool Update(std::string &, unsigned int, bool, bool, const char *,
                const KPConfig &config);
    void Draw();
    void SetPosition(float,float,float,float);
    void SetFadeOut();
    void SetSelected();
    void SetFadeIn();
    void SetFullyVisible();
    void StartAnimation();
    int Animate(int);
    int MouseEvent(tMouseButton,tMouseEvent,int,int, KPUIBase &);
    void SetSignal(int);
    void RecreateDisplayList(BTexture *pTexture = NULL);
private:
    int DisplayList;
    int Type;
    float AspectRatio;
    float ax, ay;
    float bx, by;
    float Alpha;
    float target_ax, target_ay;
    float target_bx, target_by;
    float target_Alpha;
    float old_ax, old_ay;
    float old_bx, old_by;
    float old_Alpha;
    int InAnimation;
    int Signal;
    int Time;
    float r, g, b; // Color
    std::string TextureSource;
    unsigned int Texture;
    unsigned int TextureSize;
    bool Nearest;
    bool WithAlpha;
};

#endif // #ifndef __PLATE_H__
