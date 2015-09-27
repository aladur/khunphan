/*******************************************************************************
**
** camera.h    Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2015 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef __camera_h__
#define __camera_h__

#include <vector>


// struct containing a camera position
struct SPosition
{
    SPosition()
    {
        Pos_x = Pos_y = Pos_z = Alpha = Beta = FOV = 0.0;
    };
    SPosition(float _Pos_x, float _Pos_y, float _Pos_z,
              float _Alpha, float _Beta,  float _FOV) :
        Pos_x(_Pos_x), Pos_y(_Pos_y), Pos_z(_Pos_z),
        Alpha(_Alpha), Beta(_Beta),   FOV(_FOV)
    { };
    float Pos_x;
    float Pos_y;
    float Pos_z;
    float Alpha;
    float Beta;
    float FOV;
};

class Camera
{
public:
    Camera();
    void male(int x=-1, int y=-1) const;
    void neuePosition(SPosition &position);
    SPosition Position();
    void ladePosition(int);
    void Beweg_Vor(float);
    void Beweg_Zurueck(float);
    void Beweg_Rein(float);
    void Beweg_Raus(float);
    void Beweg_Rechts(float);
    void Beweg_Links(float);
    void Beweg_Hoch(float);
    void Beweg_Runter(float);
    void Zoom_Rein(float);
    void Zoom_Raus(float);
    void Vertigo_Rein(float);
    void Vertigo_Raus(float);
    void Dreh_Rechts(float);
    void Dreh_Links(float);
    void Dreh_Hoch(float);
    void Dreh_Runter(float);
    void Schwenk_Rechts(float,float,float);
    void Schwenk_Links(float,float,float);
    void Schwenk_Hoch(float,float,float);
    void Schwenk_Runter(float,float,float);
    void Verfolge(int);
    void BlickeAuf(float,float);
    void BlickeAuf2(float,float);
    void BlickeAuf3(float,float);
    void Fahrt(int);
    void Rundflug(int);
    void setzeSollPosition(SPosition &target_position);
    void setzeSollPosition(float,float,float,float,float,float);
    void BlickTiefeNeuBestimmen();
    void SetzeRundflug(bool r = true)
    {
        IsRoundtrip = r;
    };
    void SetAspectRatio(float aspectRatio);

private:
    float Pos_xCM();
    float Pos_yCM();
    float Pos_zCM();
    float AspectRatio;
    float Pos_x,Pos_y,Pos_z,Alpha,Beta,FOV;
    float target_Pos_x,target_Pos_y,target_Pos_z;
    float target_Alpha,target_Beta,target_FOV;
    float d_Pos_x,d_Pos_y,d_Pos_z,d_Alpha,d_Beta,d_FOV;
    float Near,Far;
    bool IsRoundtrip;

    const float MoveFactor;
    const float RotateFactor;
    static const float degprad; // degrees per radian

    // vector zum Abspeichern der Kamerapositionen
    std::vector<SPosition> Positions;
};

#endif
