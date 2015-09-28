/*******************************************************************************
**
** camera.h    Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2015 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifndef __camera_h__
#define __camera_h__

#include <vector>

// Uncomment this to get a camera with an extended
// feature set.
// #define CAMERA_EXTENDED

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
#ifdef CAMERA_EXTENDED
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
#endif // #ifdef CAMERA_EXTENDED
    void MoveIn(float);
    void MoveOut(float);
    void Run(int);
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

private:
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
