/**********************************************************************************
**
**  Schild.cpp     Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**                 Modified 2002-2015 for KhunPhan by W. Schwotzer
**
***********************************************************************************/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#endif
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include "misc1.h"
#ifdef HAVE_UNISTD_H
  #include <sys/types.h>
  #include <unistd.h>  // needed for access
#endif
#include "kpconfig.h"
#include "Schild.h"
#include "btexture.h"
#include "kpuibase.h"


Schild::Schild() :     SchildIndex(0), SchildTyp(0), Aspekt(0.0),
                       ax(0), ay(0), bx(0), by(0), Alpha(0),
                       soll_ax(0), soll_ay(0), soll_bx(0), soll_by(0), soll_Alpha(0),
                       alt_ax(0),  alt_ay(0), alt_bx(0),  alt_by(0), alt_Alpha(0),
                       InAnimation(0), Signal(0), Zeit(0),
                       r(1.0), g(1.0), b(1.0), texture(0), textureSource(0)
{
}

bool Schild::Initialisiere(const char     *TextureName,
                           unsigned int    TextureSize,
                           bool            Nearest,
                           bool            withAlpha,
                           const char     *Name,
                           const KPConfig *pConfig,
                           bool            always)
{
  // The texture size must be a power of 2 (1, 2, 4, 8, ...)
  if (BTexture::GetExpToBase2(TextureSize) == -1)
    return false;

  BTexture *pTexture = NULL;
  BString file1, file2;
  const char *texels;

  file1 = pConfig->GetDirectory(KP_TEXTURE_DIR) + TextureName + PATHSEPARATORSTRING + Name + ".png";
  file2 = pConfig->GetDirectory(KP_TEXTURE_DIR) + Name + ".png";

  if (!always)
  {
    if ((!always && !access(file1.c_str(), R_OK) && textureSource == 1) ||
        (!always && !access(file2.c_str(), R_OK) && textureSource == 2))
    {
    return true; // texture from file1 already loaded
    }
  }

  pTexture = new BTexture;

    // TEX_WITH_ALPHA
  if ((texels = pTexture->ReadTextureFromFile(file1.c_str(), withAlpha ? TEX_RGB_ALPHA : TEX_RGB)) == NULL)
  {
    if ((texels = pTexture->ReadTextureFromFile(file2.c_str(), withAlpha ? TEX_RGB_ALPHA : TEX_RGB)) == NULL)
    {
      message(mtError, "*** Error reading texture from file '%s'\n", file2.c_str());
      delete pTexture;
      exit(1);
    } else
      textureSource = 2;
  } else
    textureSource = 1;
  

  texels = pTexture->Rescale(BTexture::GetExpToBase2(TextureSize), TEX_SMALLER | TEX_RESCALE_AVG);

  unsigned int width  = pTexture->GetWidth();
  unsigned int height = pTexture->GetHeight();

  if (!BTexture::IsPowerOf2(width) || !BTexture::IsPowerOf2(height))
     message(mtWarning, "*** Warning: width or height of '%s' is not a power of 2\n", file2.c_str());
  
  if (SchildIndex == 0) SchildIndex=glGenLists(1);
  if (texture == 0) glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY,   1.0);

  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  static_cast<GLfloat>(Nearest ? GL_NEAREST : GL_LINEAR));
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  static_cast<GLfloat>(Nearest ? GL_NEAREST : GL_LINEAR));

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
               withAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texels);

  glNewList(SchildIndex,GL_COMPILE);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0,0.0); glVertex2f(0.0,0.0);
  glTexCoord2f(1.0,0.0); glVertex2f(1.0,0.0);
  glTexCoord2f(1.0,1.0); glVertex2f(1.0,1.0);
  glTexCoord2f(0.0,1.0); glVertex2f(0.0,1.0);
  glEnd();     
  glEndList();
  //glBindTexture(GL_TEXTURE_2D, 0);

  Aspekt=(width+1.0f)/(height+1.0f);

  SchildTyp=1;
  delete pTexture;

  return true;
}

void Schild::Initialisiere(float R, float G, float B)
{
  r = R;
  g = G;
  b = B;

  if (!SchildIndex) SchildIndex=glGenLists(1);

  glNewList(SchildIndex,GL_COMPILE);
  glDisable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glVertex2f(0.0,0.0);
  glVertex2f(1.0,0.0);
  glVertex2f(1.0,1.0);
  glVertex2f(0.0,1.0);
  glEnd();     
  glEndList();

  ax=alt_ax=soll_ax=8;
  ay=alt_ay=soll_ay=6;
  bx=alt_bx=soll_bx=8.1f;
  by=alt_by=soll_by=6.1f;

  Alpha=alt_Alpha=soll_Alpha=MOD_AUSGEBLENDET;
  InAnimation=0; 

  Aspekt=0.0;

  SchildTyp=3;
}

void Schild::KopieVon(Schild Vorbild) {
  SchildIndex=Vorbild.SchildIndex;
  SchildTyp=Vorbild.SchildTyp;
  Aspekt=Vorbild.Aspekt;
  ax=alt_ax=soll_ax=0;
  ay=alt_ay=soll_ay=0;
  bx=alt_bx=soll_bx=0;
  by=alt_by=soll_by=0;
  Alpha=alt_Alpha=soll_Alpha=MOD_AUSGEBLENDET;
  InAnimation=0; 
  Signal=0;
}

void Schild::male(){
  if (Alpha) {
    GLfloat color[] = {r, g, b, Alpha};
    glPushMatrix();
    glTranslatef(ax,ay,0);
    glScalef(bx-ax,by-ay,1);
    if (texture != 0)
    {
      glEnable(GL_TEXTURE_2D); /*glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,color);*/
    }
    glColor4fv(color);
    glCallList(SchildIndex);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
  }
}

void Schild::Positioniere(float ax_,float ay_,float bx_,float by_) {
    // Korrigiere Aspekt
    if (Aspekt!=0.0 && Aspekt!=(bx_-ax_)/(by_-ay_)) {
      GLfloat cx=(ax_+bx_)*0.5f;
      ax_=cx-Aspekt*(by_-ay_)*0.5f;
      bx_=cx+Aspekt*(by_-ay_)*0.5f;
    }
    
    soll_ax=ax_;
    soll_ay=ay_;
    soll_bx=bx_;
    soll_by=by_;
    
    if (SchildTyp==3) {
      soll_Alpha=MOD_TRANSPARENT;
    } else {
      soll_Alpha=MOD_EINGEBLENDET;
    }  
    
    if (Alpha==MOD_AUSGEBLENDET) {
      
      ax=((soll_ax-8)/1.5f)+8;
      ay=((soll_ay-6)/1.5f)+6;
      bx=((soll_bx-8)/1.5f)+8;
      by=((soll_by-6)/1.5f)+6;
      
      /*
	ax=((24.0*rand())/RAND_MAX)-4;
	ay=((14.0*rand())/RAND_MAX)-1;
	bx=ax+2*(bx_-ax_);
	by=ay+2*(by_-ay_);     
      */
    }
    
    Signal=0;
    
    StarteAnimation();
}

void Schild::Desaktiviere() {
  soll_Alpha=MOD_AUSGEBLENDET;
  
  soll_ax=((ax-8)*1.5f)+8;
  soll_ay=((ay-6)*1.5f)+6;
  soll_bx=((bx-8)*1.5f)+8;
  soll_by=((by-6)*1.5f)+6;
  
  /*  
      soll_ax=((24.0*rand())/RAND_MAX)-4;
      soll_ay=((14.0*rand())/RAND_MAX)-1;
      soll_bx=soll_ax+.5*(bx-ax);
      soll_by=soll_ay+.5*(by-ay);     
  */
  if (soll_Alpha!=Alpha) StarteAnimation();
}

void Schild::Angewaehlt() {
  Alpha=MOD_ANGEWAEHLT;
  soll_Alpha=MOD_EINGEBLENDET;
  if (soll_Alpha!=Alpha) StarteAnimation();
}

void Schild::Eingeblendet() {
  soll_Alpha=MOD_EINGEBLENDET;
  if (soll_Alpha!=Alpha) StarteAnimation();
}

void Schild::VollSichtbar() {
  soll_Alpha=MOD_VOLLSICHTBAR;
  if (soll_Alpha!=Alpha) StarteAnimation();
}

void Schild::StarteAnimation(){
  InAnimation=1;
  Zeit=0;
  alt_ax=ax;
  alt_ay=ay;
  alt_bx=bx;
  alt_by=by;
  alt_Alpha=Alpha;
}  

int Schild::Animiere(int Faktor){
  if (!InAnimation) {return 0;}
  Zeit+=Faktor;
  if (Zeit>=TOTAL_ANIMATIONTIME) {
    ax=soll_ax;
    ay=soll_ay;
    bx=soll_bx;
    by=soll_by;
    Alpha=soll_Alpha;
    InAnimation=0;
    return 1;
  } else {
    GLfloat Faktor=
            static_cast<GLfloat>(0.5-0.5*cos(M_PI*Zeit/TOTAL_ANIMATIONTIME));
    ax=(soll_ax-alt_ax)*Faktor+alt_ax;
    ay=(soll_ay-alt_ay)*Faktor+alt_ay;
    bx=(soll_bx-alt_bx)*Faktor+alt_bx;
    by=(soll_by-alt_by)*Faktor+alt_by;
    Alpha=(soll_Alpha-alt_Alpha)*Faktor+alt_Alpha;
    return 0;
  }
}

int Schild::Maustaste(int Taste,int Richtung,int x,int y, KPUIBase &ui) {
  GLfloat xf=16.0f*x/ ui.GetValue(KP_WINDOW_WIDTH);
  GLfloat yf=12.0f-12.0f*y/ ui.GetValue(KP_WINDOW_HEIGHT);

  if (soll_Alpha>0.0 &&\
      Signal!=0 &&\
      ax<=xf && xf<=bx && ay<=yf && yf<=by) {
    if (Taste==KP_LEFT_MB) {
      if (Richtung==KP_BUTTON_PRESS) {
	Angewaehlt();
	return -1;
      } else {
	return Signal;
      }
    } else {
      return -1;
    }
  } else {
    return 0;
  }
}

void Schild::SetzeSignal(int NeuesSignal) {
  Signal=NeuesSignal;
}
