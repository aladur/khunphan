/**********************************************************************************
**
**  light.cpp      Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**                 Modified 2002,2003 for KhunPhan by W. Schwotzer
**
***********************************************************************************/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <stdio.h>
#include "light.h"
#include "misc1.h"

Light::Light(bool AmbientLight /* = true*/, int LightSources /*= 1*/, bool Reflections /*= true*/)
{
  Update(AmbientLight, LightSources, Reflections);
}

Light::~Light(){
}

void Light::Update(bool AmbientLight, int LightSources /*= 1*/, bool Reflections /*= true*/)
{
  glEnable(GL_LIGHTING);

  if (AmbientLight) {
    GLfloat ambient_light []={.25,.25,.25,1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
  }
  // Sw: unfinished: not supported on WIN32
  //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
  

  //GLfloat Brightness = (GLfloat)(1.0 / LightSources);
  GLfloat Brightness = 1.0;

  DEBUGPRINT("Light initialization\n");
  DEBUGPRINT1(" Ambient Light: %s\n",   AmbientLight ? "On" : "Off");
  DEBUGPRINT1(" Light Sources: %d\n",   LightSources);
  DEBUGPRINT1(" Reflections:   %s\n",   Reflections ? "On" : "Off");
  DEBUGPRINT1(" Brightness:    %.2f\n", Brightness);

  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHT2);
  glDisable(GL_LIGHT3);

  GLfloat x = 63.5;
  GLfloat y = 0.0;
  GLfloat z = 40.0;
  
  if (LightSources >= 2) {
    //Middle Light:
    GLfloat light_position [] = {x,y,z,1.0};
    GLfloat white_light[] = {Brightness,Brightness,Brightness,1};
    glLightfv(GL_LIGHT1,GL_POSITION, light_position);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,white_light);
    glLightfv(GL_LIGHT1,GL_SPECULAR,white_light);
    glEnable(GL_LIGHT1);
  }
  x -= 63.5;
  if (LightSources == 1 || LightSources == 3) {
    //Light on the Right side:
    GLfloat light_position [] = {x,y,z,1.0};
    GLfloat white_light[] = {Brightness,Brightness,Brightness,1};
    glLightfv(GL_LIGHT0,GL_POSITION, light_position);
    glLightfv(GL_LIGHT0,GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT0,GL_SPECULAR, white_light);
    glEnable(GL_LIGHT0);
  }
  x -= 63.5;
  if (LightSources >= 2) {
    //Light on the Left side:
    GLfloat light_position [] = {x,y,z,1.0};
    GLfloat white_light[] = {Brightness,Brightness,Brightness,1};
    glLightfv(GL_LIGHT2,GL_POSITION, light_position);
    glLightfv(GL_LIGHT2,GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT2,GL_SPECULAR, white_light);
    glEnable(GL_LIGHT2);
  }

  if (Reflections)
    glLightModelf(33272,33274);

  //glEndList();

}

void Light::Draw() const
{
}
