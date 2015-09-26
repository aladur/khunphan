/*******************************************************************************
**
** Kamera.cpp  Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**             Modified 2002-2015 for KhunPhan by W. Schwotzer
**
*******************************************************************************/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include "camera.h"
#include "misc1.h"


// define constant for degree per radian */
const float Kamera::degprad = 180.0f / M_PIf;

/* --------- Konstruktor ---------- */
Kamera::Kamera() :   Aspekt(0.0),
    Pos_x(0.0),      Pos_y(0.0),       Pos_z(0.0),
    Alpha(0.0),      Beta(0.0),        FOV(0.0),
    Soll_Pos_x(0.0), Soll_Pos_y(0.0),  Soll_Pos_z(0.0),
    Soll_Alpha(0.0), Soll_Beta(0.0),   Soll_FOV(0.0),
    d_Pos_x(0.0),    d_Pos_y(0.0),     d_Pos_z(0.0),
    d_Alpha(0.0),    d_Beta(0.0),      d_FOV(0.0),
    Nah(0.0),        Fern(0.0),        KameraNummer(0),
    IsRundflug(0),   BewegFaktor(0.3f),DrehFaktor(0.3f)
{
    // Initialize default camera positions with the parameter list:
    //   Pos_x, Pos_y, Pos_z, Alpha, Beta, FOV

    // Position with index 0
    Positionen.push_back(SPosition(-100.0f, -50.0f, 50.0f, 60.0f, 60.0f, 38.6f));

    // Position with index 1
    Positionen.push_back(SPosition(0.0f, 160.0f, 80.0f, 60.0f, 180.0f, 38.6f));

    // Position with index 2
    Positionen.push_back(SPosition(0.0f, 140.0f, 120.0f, 45.0f, 180.0f, 38.6f));

    // Position with index 3
    Positionen.push_back(SPosition(0.0f, 115.0f, 180.0f, 30.0f, 180.0f, 38.6f));

    // Position with index 4
    Positionen.push_back(SPosition(0.0f, 0.0f, 220.0f, 0.0f, 180.0f, 38.6f));

    // Position with index 5
    Positionen.push_back(SPosition(0.0f, 0.0f, 300.0f, 0.0f, 180.0f, 38.6f));

    // Position with index 6
    Positionen.push_back(SPosition(-60.0f, 160.0f, 140.0f, 45.0f, 155.0f, 38.6f));

    // Position with index 7
    Positionen.push_back(SPosition(60.0f, 160.0f, 140.0f, 45.0f, 205.0f, 38.6f));

    Soll_Pos_x=-60;
    Soll_Pos_y=-30;
    Rundflug(0);
    Alpha=Soll_Alpha;
    Beta=Soll_Beta;
    Pos_x=Soll_Pos_x;
    Pos_y=Soll_Pos_y;
    Pos_z=Soll_Pos_z;

    Aspekt= 4.0f / 3.0f;

    IsRundflug = false;
}

void Kamera::SetAspekt(float a)
{
    Aspekt = a;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV,Aspekt,Nah,Fern);
    glMatrixMode(GL_MODELVIEW);
}

// This method can be used for both display update and Selection
// Parameters:
//   For Display update: not necessary
//   For Selection:      the X and Y Screen coordinates to select

void Kamera::male(int x /* = -1 */, int y /* = -1 */) const
{
    glMatrixMode(GL_PROJECTION); // Kameraparameter!
    glLoadIdentity();            // zuruecksetzen
    if (x >= 0 && y >= 0)        // only necessary in selection mode:
    {
        GLint   viewport[4];

        glGetIntegerv(GL_VIEWPORT, viewport);
        gluPickMatrix(x, (viewport[3]- y), 1.0f, 1.0f, viewport);
    }
    gluPerspective(FOV,Aspekt,Nah,Fern);

    glMatrixMode(GL_MODELVIEW);  // Blickpunkt!
    glLoadIdentity();            // Kamera an den Ursprung setzen

    glRotatef(Alpha,-1,0,0);     // um Alpha nach oben und Beta nach Rechts
    // drehen
    glRotatef(Beta,0,0,1);
    // an die gewuenschte Position setzen
    glTranslatef(-Pos_x,-Pos_y,-Pos_z);
}

//setzt die Kamera an eine neue Position
void Kamera::neuePosition(SPosition &position)
{
    Soll_Pos_x = position.Pos_x;
    Soll_Pos_y = position.Pos_y;
    Soll_Pos_z = position.Pos_z;
    Soll_Alpha = position.Alpha;
    Soll_Beta  = position.Beta;
    Soll_FOV   = position.FOV;
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

//gibt die aktuelle Kameraposition zur"uck
SPosition Kamera::Position()
{
    SPosition position(Pos_x, Pos_y, Pos_z, Alpha, Beta, FOV);

    return position;
}

float Kamera::Pos_xCM()
{
    return Pos_x;
}

float Kamera::Pos_yCM()
{
    return Pos_y;
}

float Kamera::Pos_zCM()
{
    return Pos_z;
}

// l"adt eine Kameraposition aus der Tabelle
void Kamera::ladePosition(int Platz)
{
    setzeSollPosition(Positionen.at(Platz));
    IsRundflug = false;
}

// Bewegt die Kamera in Blickrichtung
void Kamera::Beweg_Rein(float Faktor)
{
    Soll_Pos_x+=2*BewegFaktor*Faktor*sin(Soll_Alpha/degprad)*sin(
                    Soll_Beta/degprad);
    Soll_Pos_y+=2*BewegFaktor*Faktor*sin(Soll_Alpha/degprad)*cos(
                    Soll_Beta/degprad);
    Soll_Pos_z-=2*BewegFaktor*Faktor*cos(Soll_Alpha/degprad);
    //  if (Pos_z > 400.0f) {Pos_z = 400.0f;}
    if (Soll_Pos_z < 2.8f)
    {
        Soll_Pos_z = 2.8f;
    }
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

// Bewegt die Kamera gegen die Blickrichtung
void Kamera::Beweg_Raus(float Faktor)
{
    Soll_Pos_x-=2*BewegFaktor*Faktor*sin(Soll_Alpha/degprad)*sin(
                    Soll_Beta/degprad);
    Soll_Pos_y-=2*BewegFaktor*Faktor*sin(Soll_Alpha/degprad)*cos(
                    Soll_Beta/degprad);
    Soll_Pos_z+=2*BewegFaktor*Faktor*cos(Soll_Alpha/degprad);
    //  if (Pos_z > 400.0f) {Pos_z = 400.0f;}
    if (Soll_Pos_z < 2.8f)
    {
        Soll_Pos_z = 2.8f;
    }
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

// Bewegt die Kamera in Blickrichtung, aber unter Beibehaltung der H"ohe
void Kamera::Beweg_Vor(float Faktor)
{
    Soll_Pos_x+=2*BewegFaktor*Faktor*sin(Soll_Beta/degprad);
    Soll_Pos_y+=2*BewegFaktor*Faktor*cos(Soll_Beta/degprad);
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

// Bewegt die Kamera gegen die Blickrichtung, aber unter Beibehaltung der H"ohe
void Kamera::Beweg_Zurueck(float Faktor)
{
    Soll_Pos_x-=2*BewegFaktor*Faktor*sin(Soll_Beta/degprad);
    Soll_Pos_y-=2*BewegFaktor*Faktor*cos(Soll_Beta/degprad);
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

// Bewegt die Kamera nach rechts
void Kamera::Beweg_Rechts(float Faktor)
{
    Soll_Pos_x+=BewegFaktor*Faktor*cos(Soll_Beta/degprad);
    Soll_Pos_y-=BewegFaktor*Faktor*sin(Soll_Beta/degprad);
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

// Bewegt die Kamera nach links
void Kamera::Beweg_Links(float Faktor)
{
    Soll_Pos_x-=BewegFaktor*Faktor*cos(Soll_Beta/degprad);
    Soll_Pos_y+=BewegFaktor*Faktor*sin(Soll_Beta/degprad);
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

// Bewegt die Kamera nach oben
void Kamera::Beweg_Hoch(float Faktor)
{
    Soll_Pos_z+=BewegFaktor*Faktor;
    //  if (Pos_z>400) {Pos_z=400;}
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

// Bewegt die Kamera nach unten
void Kamera::Beweg_Runter(float Faktor)
{
    Soll_Pos_z-=BewegFaktor*Faktor;
    if (Soll_Pos_z < 2.8f)
    {
        Soll_Pos_z = 2.8f;
    }
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

// Vergr"o"sert den Zoom
void Kamera::Zoom_Rein(float Faktor)
{
    Soll_FOV-=BewegFaktor*Faktor;
    if (Soll_FOV<1)
    {
        Soll_FOV=1;
    }
    IsRundflug = false;
}

// Verkleinert den Zoom
void Kamera::Zoom_Raus(float Faktor)
{
    Soll_FOV+=BewegFaktor*Faktor;
    IsRundflug = false;
}

// Vergr"o"sert den Vertigo
void Kamera::Vertigo_Rein(float Faktor)
{
    Soll_Pos_x-=2*BewegFaktor*Faktor*sin(Soll_Alpha/degprad)*sin(
                    Soll_Beta/degprad);
    Soll_Pos_y-=2*BewegFaktor*Faktor*sin(Soll_Alpha/degprad)*cos(
                    Soll_Beta/degprad);
    Soll_Pos_z+=2*BewegFaktor*Faktor*cos(Soll_Alpha/degprad);
    Soll_FOV=2077*cos(Soll_Alpha/degprad)/Soll_Pos_z;
    if (Soll_FOV<1)
    {
        Soll_FOV=1;
    }
    IsRundflug = false;
}

// Verkleinert den Vertigo
void Kamera::Vertigo_Raus(float Faktor)
{
    Soll_Pos_x+=2*BewegFaktor*Faktor*sin(Soll_Alpha/degprad)*sin(
                    Soll_Beta/degprad);
    Soll_Pos_y+=2*BewegFaktor*Faktor*sin(Soll_Alpha/degprad)*cos(
                    Soll_Beta/degprad);
    Soll_Pos_z-=2*BewegFaktor*Faktor*cos(Soll_Alpha/degprad);
    Soll_FOV=2077*cos(Soll_Alpha/degprad)/Soll_Pos_z;
    IsRundflug = false;
}

// Dreht die Kamera nach rechts
void Kamera::Dreh_Rechts(float Faktor)
{
    Soll_Beta+=Faktor*DrehFaktor;
    IsRundflug = false;
}

// Dreht die Kamera nach links
void Kamera::Dreh_Links(float Faktor)
{
    Soll_Beta-=Faktor*DrehFaktor;
    IsRundflug = false;
}

// Dreht die Kamera nach oben
void Kamera::Dreh_Hoch(float Faktor)
{
    Soll_Alpha+=Faktor*DrehFaktor;
    if (Soll_Alpha>90)
    {
        Soll_Alpha=90;
    }
    if (Soll_Alpha<0)
    {
        Soll_Alpha=0;
    }
    IsRundflug = false;
}

// Dreht die Kamera nach unten
void Kamera::Dreh_Runter(float Faktor)
{
    Soll_Alpha-=Faktor*DrehFaktor;
    if (Soll_Alpha>90)
    {
        Soll_Alpha=90;
    }
    if (Soll_Alpha<0)
    {
        Soll_Alpha=0;
    }
    IsRundflug = false;
}

void Kamera::Schwenk_Links(float Faktor, float Mitte_x, float Mitte_y)
{
    GLfloat Abstand=sqrt((Mitte_x-Soll_Pos_x)*(Mitte_x-Soll_Pos_x)+
                         (Mitte_y-Soll_Pos_y)*(Mitte_y-Soll_Pos_y));
    if (Abstand<5)
    {
        Abstand=5;
    }
    Soll_Beta += Faktor * DrehFaktor * 7.338f / sqrt(Abstand);
    Soll_Pos_x=Mitte_x-Abstand*sin(Soll_Beta/degprad);
    Soll_Pos_y=Mitte_y-Abstand*cos(Soll_Beta/degprad);
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

void Kamera::Schwenk_Rechts(float Faktor, float Mitte_x, float Mitte_y)
{
    GLfloat Abstand=sqrt((Mitte_x-Soll_Pos_x)*(Mitte_x-Soll_Pos_x)+
                         (Mitte_y-Soll_Pos_y)*(Mitte_y-Soll_Pos_y));
    if (Abstand<5)
    {
        Abstand=5;
    }
    Soll_Beta -= Faktor * DrehFaktor * 7.338f / sqrt(Abstand);
    Soll_Pos_x=Mitte_x-Abstand*sin(Soll_Beta/degprad);
    Soll_Pos_y=Mitte_y-Abstand*cos(Soll_Beta/degprad);
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

void Kamera::Schwenk_Hoch(float Faktor, float Mitte_x, float Mitte_y)
{
    GLfloat Abstand=sqrt((Mitte_x-Soll_Pos_x)*(Mitte_x-Soll_Pos_x)+
                         (Mitte_y-Soll_Pos_y)*(Mitte_y-Soll_Pos_y)+
                         (Soll_Pos_z - 2.8f) * (Soll_Pos_z - 2.8f));
    if (Abstand==0)
    {
        Soll_Alpha=0;
    }
    else
    {
        Soll_Alpha += Faktor * DrehFaktor * 7.338f / sqrt(Abstand);
        if (Soll_Alpha<0)
        {
            Soll_Alpha=0;
        }
        if (Soll_Alpha>90)
        {
            Soll_Alpha=90;
        }
    }
    Soll_Pos_x=Mitte_x-Abstand*sin(Soll_Beta/degprad)*sin(Soll_Alpha/degprad);
    Soll_Pos_y=Mitte_y-Abstand*cos(Soll_Beta/degprad)*sin(Soll_Alpha/degprad);
    Soll_Pos_z = 2.8f + Abstand * cos(Soll_Alpha / degprad);
    //  if (Pos_z > 400.0f) {Pos_z = 400.0f;}
    if (Soll_Pos_z < 2.8f)
    {
        Soll_Pos_z = 2.8f;
    }
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

void Kamera::Schwenk_Runter(float Faktor, float Mitte_x, float Mitte_y)
{
    GLfloat Abstand=sqrt((Mitte_x-Soll_Pos_x)*(Mitte_x-Soll_Pos_x)+
                         (Mitte_y-Soll_Pos_y)*(Mitte_y-Soll_Pos_y)+
                         (Soll_Pos_z - 2.8f) * (Soll_Pos_z - 2.8f));
    if (Abstand==0)
    {
        Soll_Alpha=0;
    }
    else
    {
        Soll_Alpha -= Faktor * DrehFaktor * 7.338f / sqrt(Abstand);
        if (Soll_Alpha<0)
        {
            Soll_Alpha=0;
        }
        if (Soll_Alpha>90)
        {
            Soll_Alpha=90;
        }
    }
    Soll_Pos_x=Mitte_x-Abstand*sin(Soll_Beta/degprad)*sin(Soll_Alpha/degprad);
    Soll_Pos_y=Mitte_y-Abstand*cos(Soll_Beta/degprad)*sin(Soll_Alpha/degprad);
    Soll_Pos_z = 2.8f + Abstand * cos(Soll_Alpha / degprad);
    //  if (Pos_z > 400.0f) {Pos_z = 400.0f;}
    if (Soll_Pos_z < 2.8f)
    {
        Soll_Pos_z = 2.8f;
    }
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

void Kamera::setzeSollPosition(SPosition &Soll_Pos)
{
    Soll_Pos_x = Soll_Pos.Pos_x;
    Soll_Pos_y = Soll_Pos.Pos_y;
    Soll_Pos_z = Soll_Pos.Pos_z;
    Soll_Alpha = Soll_Pos.Alpha;
    Soll_Beta  = Soll_Pos.Beta;
    Soll_FOV   = Soll_Pos.FOV;

    Beta = fmod(Beta, 360.0f);
    Soll_Beta = fmod(Soll_Beta, 360.0f);

    if (Soll_Beta>Beta+180)
    {
        Soll_Beta-=360;
    }
    if (Soll_Beta<Beta-180)
    {
        Soll_Beta+=360;
    }
    BlickTiefeNeuBestimmen();
    IsRundflug = false;
}

void Kamera::setzeSollPosition(float SollPosx, float SollPosy,float SollPosz,
                               float SollAlpha,float SollBeta,float SollFOV)
{
    Soll_Pos_x = SollPosx;
    Soll_Pos_y = SollPosy;
    Soll_Pos_z = SollPosz;
    Soll_Alpha = SollAlpha;
    Soll_Beta  = SollBeta;
    Soll_FOV   = SollFOV;

    Beta = fmod(Beta, 360.0f);
    Soll_Beta = fmod(Soll_Beta, 360.0f);

    if (Soll_Beta>Beta+180)
    {
        Soll_Beta-=360;
    }
    if (Soll_Beta<Beta-180)
    {
        Soll_Beta+=360;
    }
    IsRundflug = false;
}

void Kamera::BlickeAuf(float Blickpunkt_x,float Blickpunkt_y)
{
    GLfloat SollPosx=50*(Pos_x-Blickpunkt_x)/
                     sqrt((Blickpunkt_x-Pos_x)*(Blickpunkt_x-Pos_x)+
                          (Blickpunkt_y-Pos_y)*(Blickpunkt_y-Pos_y))+
                     Blickpunkt_x;
    GLfloat SollPosy=50*(Pos_y-Blickpunkt_y)/
                     sqrt((Blickpunkt_x-Pos_x)*(Blickpunkt_x-Pos_x)+
                          (Blickpunkt_y-Pos_y)*(Blickpunkt_y-Pos_y))+
                     Blickpunkt_y;
    GLfloat SollPosz=20;
    GLfloat SollAlpha = 71.0167f;
    GLfloat SollBeta=atan((Blickpunkt_x-SollPosx)/(Blickpunkt_y-SollPosy))*
                     degprad;
    if (SollPosy>Blickpunkt_y)
    {
        SollBeta -= 180.0f;
    }
    GLfloat SollFOV = 38.6f;

    setzeSollPosition(SollPosx,SollPosy,SollPosz,SollAlpha,SollBeta,SollFOV);
    IsRundflug = false;
}

void Kamera::BlickeAuf2(float Blickpunkt_x,float Blickpunkt_y)
{
    GLfloat SollPosx=80*(Pos_x-Blickpunkt_x)/
                     sqrt((Blickpunkt_x-Pos_x)*(Blickpunkt_x-Pos_x)+
                          (Blickpunkt_y-Pos_y)*(Blickpunkt_y-Pos_y))+
                     Blickpunkt_x;
    GLfloat SollPosy=80*(Pos_y-Blickpunkt_y)/
                     sqrt((Blickpunkt_x-Pos_x)*(Blickpunkt_x-Pos_x)+
                          (Blickpunkt_y-Pos_y)*(Blickpunkt_y-Pos_y))+
                     Blickpunkt_y;
    GLfloat SollPosz=50;
    GLfloat SollAlpha=72;
    GLfloat SollBeta=atan((Blickpunkt_x-SollPosx)/(Blickpunkt_y-SollPosy))*
                     degprad;
    if (SollPosy>Blickpunkt_y)
    {
        SollBeta -= 180.0f;
    }
    GLfloat SollFOV = 38.6f;

    setzeSollPosition(SollPosx,SollPosy,SollPosz,SollAlpha,SollBeta,SollFOV);
    IsRundflug = false;
}

void Kamera::BlickeAuf3(float Blickpunkt_x,float Blickpunkt_y)
{
    GLfloat SollPosx=80*(Pos_x-Blickpunkt_x)/
                     sqrt((Blickpunkt_x-Pos_x)*(Blickpunkt_x-Pos_x)+
                          (Blickpunkt_y-Pos_y)*(Blickpunkt_y-Pos_y))+
                     Blickpunkt_x;
    GLfloat SollPosy=80*(Pos_y-Blickpunkt_y)/
                     sqrt((Blickpunkt_x-Pos_x)*(Blickpunkt_x-Pos_x)+
                          (Blickpunkt_y-Pos_y)*(Blickpunkt_y-Pos_y))+
                     Blickpunkt_y;
    GLfloat SollPosz=50;
    GLfloat SollAlpha=58;
    GLfloat SollBeta=atan((Blickpunkt_x-SollPosx)/(Blickpunkt_y-SollPosy))*
                     degprad;
    if (SollPosy>Blickpunkt_y)
    {
        SollBeta -= 180.0f;
    }
    GLfloat SollFOV = 38.6f;

    setzeSollPosition(SollPosx,SollPosy,SollPosz,SollAlpha,SollBeta,SollFOV);
    IsRundflug = false;
}

void Kamera::Fahrt(int Faktor)
{
    for (int i=0; i<Faktor; i++)
    {

        if (Soll_Beta-Beta<-180)
        {
            Beta-=360;
        }
        if (Soll_Beta-Beta> 180)
        {
            Beta+=360;
        }

        d_Pos_x = d_Pos_x * 0.9f + 0.003f * (Soll_Pos_x - Pos_x);
        d_Pos_y = d_Pos_y * 0.9f + 0.003f * (Soll_Pos_y - Pos_y);
        d_Pos_z = d_Pos_z * 0.9f + 0.003f * (Soll_Pos_z - Pos_z);
        d_Alpha = d_Alpha * 0.9f + 0.003f * (Soll_Alpha - Alpha);
        d_Beta  = d_Beta  * 0.9f + 0.003f * (Soll_Beta - Beta);
        d_FOV   = d_FOV   * 0.9f + 0.003f * (Soll_FOV - FOV);

        Pos_x += d_Pos_x;
        Pos_y += d_Pos_y;
        Pos_z += d_Pos_z;
        Alpha += d_Alpha;
        Beta  += d_Beta;
        FOV   += d_FOV;

        BlickTiefeNeuBestimmen();

    }
    if (IsRundflug)
    {
        Rundflug(Faktor);
    }
}

void Kamera::BlickTiefeNeuBestimmen()
{
    GLfloat ax=fabs(Pos_x),ay=fabs(Pos_y),az=Pos_z;

    if (ax<150)
    {
        if (ay<80)
        {
            Nah=az-5;
        }
        else
        {
            Nah=sqrt((ay-80)*(ay-80)+(az-5)*(az-5));
        }
    }
    else
    {
        if (ay<80)
        {
            Nah=sqrt((ax-150)*(ax-150)+(az-5)*(az-5));
        }
        else
        {
            Nah=sqrt((ax-150)*(ax-150)+(ay-80)*(ay-80)+(az-5)*(az-5));
        }
    }

    Nah *= 0.8f;

    if (Nah<1)
    {
        Nah=1;
    }

    Fern=sqrt((ax+150)*(ax+150)+(ay+80)*(ay+80)+az*az);
}


void Kamera::Rundflug(int Faktor)
{
    if (Soll_Pos_y==0)
    {
        Soll_Pos_y = 0.00001f;
    }
    //Soll_Beta=Faktor*.1+atan(Soll_Pos_x/Soll_Pos_y)*degprad;
    //if (Soll_Pos_y>0) {Soll_Beta-=180;}
    Soll_Beta += 0.1f * Faktor;
    Soll_Pos_x = (-30.0f * sin(Soll_Beta * M_PIf / 180.0f) - 280.0f) * sin(Soll_Beta * M_PIf / 180.0f);
    Soll_Pos_y = (-30.0f * sin(Soll_Beta * M_PIf / 180.0f) - 280.0f) * cos(Soll_Beta * M_PIf / 180.0f);
    // Sw: unfinished
    //Soll_Pos_z=100-50*sin(Soll_Beta*M_PI/180);
    Soll_Pos_z = 150.0f - 25.0f * sin(Soll_Beta * M_PIf / 180.0f);
    Soll_FOV = 36.8f;
    Soll_Alpha=atan(sqrt(Soll_Pos_x*Soll_Pos_x+Soll_Pos_y*Soll_Pos_y)/
                    Soll_Pos_z)*degprad;
}
