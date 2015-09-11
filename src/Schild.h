/*******************************************************************************
**
** Schild.h    Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2015 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef __SCHILD_H__
#define __SCHILD_H__


#include "misc1.h"

class KPUIBase;
class KPConfig;


class Schild
{
public:
    Schild();
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
    void KopieVon(Schild);
    int SchildIndex;
    int SchildTyp;
    float Aspekt;
private:
    float ax,ay,bx,by;
    float Alpha;
    float soll_ax,soll_ay,soll_bx,soll_by;
    float soll_Alpha;
    float alt_ax,alt_ay,alt_bx,alt_by;
    float alt_Alpha;
    int InAnimation;
    int Signal;
    int Zeit;
    float r, g, b; // Color
    unsigned int texture;
    unsigned short textureSource;
};

#endif // #ifndef __SCHILD_H__
