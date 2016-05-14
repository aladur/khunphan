/*******************************************************************************
**
** light.cpp   Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2016 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#include "stdafx.h"
#include <GL/gl.h>
#include "light.h"
#include "blogger.h"

Light::Light(bool AmbientLight /* = true*/, int aLightSources /*= 1*/,
             bool Reflections /*= false */) :
    posX(100.0), posY(180.0), posZ(50.0)
{
    Update(AmbientLight, aLightSources, Reflections);
}

void Light::Update(bool AmbientLight, int aLightSources /*= 1*/,
                   bool Reflections /*= false */)
{
    LightSources = aLightSources;

    if (LightSources < 1)
    {
        LightSources = 1;
    }

    if (LightSources > 3)
    {
        LightSources = 3;
    }

    glEnable(GL_LIGHTING);

    if (AmbientLight)
    {
        GLfloat ambient_light [] = {0.25, 0.25, 0.25, 1.0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    }
    else
    {
        GLfloat ambient_light [] = {0.0, 0.0, 0.0, 1.0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    }

    // Sw: unfinished: not supported on WIN32
    //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

    //GLfloat Brightness = (GLfloat)(1.0 / LightSources);
    auto Brightness = 1.0f;

    BLogger::Log("Light initialization");
    BLogger::Log(" Ambient Light: ", ON_OFF(AmbientLight));
    BLogger::Log(" Light Sources: ", LightSources);
    BLogger::Log(" Brightness:    ", std::fixed, std::setprecision(2),
                 Brightness);

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);

    auto x = posX + 100.0f;
    auto y = posY - 150.0f;

    if (LightSources >= 2)
    {
        //Middle Light:
        GLfloat light_position [] = {x, y, posZ, 1.0};
        GLfloat white_light[] = {Brightness, Brightness, Brightness, 1};
        glLightfv(GL_LIGHT1, GL_POSITION, light_position);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
        glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
        glEnable(GL_LIGHT1);
    }

    x -= 100.0;
    y += 300.0;

    if (LightSources == 1 || LightSources == 3)
    {
        //Light on the Right side:
        GLfloat light_position [] = {x, y, posZ, 1.0};
        GLfloat white_light[] = {Brightness, Brightness, Brightness, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
        glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
        glEnable(GL_LIGHT0);
    }

    x -= 100.0;
    y -= 300.0;

    if (LightSources >= 2)
    {
        //Light on the Left side:
        GLfloat light_position [] = {x, y, posZ, 1.0};
        GLfloat white_light[] = {Brightness, Brightness, Brightness, 1};
        glLightfv(GL_LIGHT2, GL_POSITION, light_position);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, white_light);
        glLightfv(GL_LIGHT2, GL_SPECULAR, white_light);
        glEnable(GL_LIGHT2);
    }

    if (Reflections)
    {
        glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    }
}

void Light::SetPosition(float px, float py, float pz)
{
    posX = px;
    posY = py;
    posZ = pz;

    auto X = px;

    if (LightSources >= 2)
    {
        //Middle Light:
        GLfloat light_position[] = {X, posY, posZ, 1.0};
        glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    }

    X -= 100.0;

    if (LightSources == 1 || LightSources == 3)
    {
        //Light on the Right side:
        GLfloat light_position [] = {X, posY, posZ, 1.0};
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    }

    X -= 100.0;

    if (LightSources >= 2)
    {
        //Light on the Left side:
        GLfloat light_position [] = {X, posY, posZ, 1.0};
        glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    }

}

void Light::GetPosition(float &x, float &y, float &z) const
{
    x = posX;
    y = posY;
    z = posZ;
}

