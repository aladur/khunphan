/*******************************************************************************
**
** camera.h    Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2016 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

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
    void MoveIn(float);
    void MoveOut(float);
    void Animate(unsigned int);
    void Draw(int x=-1, int y=-1) const;
    void SetPosition(int);
    void Roundtrip(int);
    void SetTargetPosition(SPosition &target_position);
    void SetTargetPosition(float,float,float,float,float,float);
    void RecalculateViewFrustum();
    inline void SetRoundtrip(bool b = true)
    {
        IsRoundtrip = b;
    };
    void SetAspectRatio(float aspectRatio);

protected:
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

    // The vector stores some pre-initialized, fixed camera positions
    std::vector<SPosition> Positions;
};

#endif
