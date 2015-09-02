/**********************************************************************************
**
**  Textfeld.h     Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**                 Modified 2002-2015 for KhunPhan by W. Schwotzer
**
***********************************************************************************/

#ifndef __TEXTFELD_H__
#define __TEXTFELD_H__

#include "misc1.h"

#include <set>
#ifdef HAVE_STDARG_H
  #include <stdarg.h>
#endif
#include "kpconfig.h"



class Textfeld;
class KPConfig;
class KPUIBase;

typedef std::set<Textfeld *> tActivated;


class Textfeld {
 public:
  Textfeld();
  virtual ~Textfeld();
  static void PreInitialize(const char *, unsigned int, bool, const KPConfig *, bool always = true);
  static void DeactivateAll();
  static void SetActive(Textfeld *pTextfeld);
  void Initialisiere();
  void Initialisiere(const char[]);
  void InitialisiereKDL(const char[]);
  void male();
  void Positioniere(float,float,float,int);
  void SetzeText(const char[]);
  void SetzeTextKDL(const char[]);
  int  FormatText(const char *format, ...);
  int  FormatTextKDL(const char *format, ...);
  int  vsprintf(const char *format, va_list arg_ptr);
  bool HasInputFocus() { return hasInputFocus; };
  void SetMaxCharacters(unsigned int value) { MaxCharacters = value; };
  unsigned int GetMaxCharacters() { return MaxCharacters; };
  bool Zeichen(char);
  const char* Text();

  bool IsDeactivated() const;
  void Desaktiviere();
  void Angewaehlt();
  void Eingeblendet();
  void VollSichtbar();
  int Animiere(int);
  void SetzeSignal(int);
  int GetSignal(void) { return Signal; };
  int Maustaste(int,int,int,int, KPUIBase &);
  
  void SetzeMaxBreite(float);

  void SetInputFocus(bool state);

  float TextfeldHoehe();

  void GeneriereDisplayList();

 private:
  static tActivated activated;
  static unsigned int texture;
 
  void SetzeFormat(const char *); 
  void StarteAnimation();
  void CheckValidString(int min_size, const char *srcString = NULL);
  char *pFormat;
  char *pString;
  int   size;
  float x,y,Hoehe,Aspekt,Alpha;
  float alt_x,alt_y,alt_Hoehe,alt_Alpha;
  float soll_x,soll_y,soll_Hoehe,soll_Alpha;
  int Ausrichtung;
  bool  InAnimation;
  int Signal;
  int Zeit; 
  int TextfeldIndex;
  bool  hasInputFocus;
  int lineCount;
  unsigned int MaxCharacters;
  float maxWidth;

  static int   links[];
  static int   rechts[];
  static short textureSource;
};

#endif
