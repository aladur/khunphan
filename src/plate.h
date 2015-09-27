/*******************************************************************************
**
** plate.h     Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2015 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef __PLATE_H__
#define __PLATE_H__


#include "misc1.h"

class KPUIBase;
class KPConfig;


class Plate
{
public:
    Plate();
    void Initialisiere(float R = 1.0, float G = 1.0, float B = 1.0);
    bool Initialisiere(const char *, unsigned int, bool, bool, const char *,
                       const KPConfig *config, bool always = true);
    void male();
    void Positioniere(float,float,float,float);
    void Desaktiviere();
    void Angewaehlt();
    void Eingeblendet();
    void VollSichtbar();
    void StarteAnimation();
    int Animiere(int);
    int Maustaste(tMouseButton,tMouseEvent,int,int, KPUIBase &);
    void SetzeZielZustand(int);
    void SetzeSignal(int);
    void KopieVon(Plate);
    int DisplayList;
    int Type;
    float AspectRatio;
private:
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
    unsigned int texture;
    unsigned short textureSource;
};

#endif // #ifndef __PLATE_H__
