/*******************************************************************************
**
** plate.h     Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2024 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef PLATE_H
#define PLATE_H


#include "stdafx.h"
#include <string>
#include "bmanualtimer.h"

class KPUIBase;
class KPConfig;
class BTexture;
enum class Signal;


class Plate
{
public:
    Plate(float R = 1.0, float G = 1.0, float B = 1.0);
    Plate(const Plate &src);
    ~Plate();

    Plate &operator=(const Plate &src);
    bool Update(std::string &, unsigned int, bool, bool, const char *,
                const KPConfig &config);
    void Draw() const;
    void SetPosition(float, float, float, float);
    void SetFadeOut();
    void SetSelected();
    void SetFadeIn();
    void SetFullyVisible();
    void StartAnimation();
    void Animate(unsigned int);
    Signal MouseEvent(MouseButton, MouseButtonEvent, int, int, KPUIBase &);
    void SetSignal(Signal);
    void RecreateDisplayList(BTexture *pTexture = nullptr);

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
    Signal signal;
    float r, g, b; // Color
    std::string TextureSource;
    unsigned int Texture;
    unsigned int TextureSize;
    bool Nearest;
    bool WithAlpha;
    BManualTimer animationTimer;
};

#endif
