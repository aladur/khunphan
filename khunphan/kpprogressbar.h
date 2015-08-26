/*
    kpprogressbar.h

    Automatic solution finder for KhunPhan game
    Copyright (C) 2001,2002,2003  W. Schwotzer

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef KPPROGRESSBAR_H
#define KPPROGRESSBAR_H


class KPprogressBar {
public: 
	KPprogressBar();
	virtual ~KPprogressBar();

  void Initialize();
  void Deactivate();
  void Blended();
  void FullVisible();
  void SetPosition(float X, float Y, float Width, float Height, int Alignment);
  void SetPercentage(int Percent);
  bool Animate(int factor);
  void StartAnimation();
  void SetBarColor(float R, float G, float B);
  void Draw();

protected:
  void GenerateDisplayList();
  bool InAnimation;
  float x, old_x, target_x;
  float y, old_y, target_y;
  float alpha, old_Alpha, target_Alpha;
  float height, old_Height, target_Height;
  float width,  old_Width,  target_Width;
  int   Time;
  int   percent;
  int   displayList;
  float barColor[4];
};

#endif
