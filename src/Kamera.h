/**********************************************************************************
**
**  Kamera.h       Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**                 Modified 2002-2015 for KhunPhan by W. Schwotzer
**
***********************************************************************************/

#ifndef __kamera_h__
#define __kamera_h__


class Kamera {
 public:
  Kamera();
  void male(int x=-1, int y=-1) const;
  void neuePosition(float[5]);
  float* Position();
  void speicherePosition(int);
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
  void BlickeAuf(float[2]);
  void BlickeAuf(float,float);
  void BlickeAuf2(float[2]);
  void BlickeAuf2(float,float);
  void BlickeAuf3(float[2]);
  void BlickeAuf3(float,float);
  void Fahrt(int);
  void Rundflug(int);
  void setzeSollPosition(float[6]);
  void setzeSollPosition(float,float,float,float,float,float);
  void BlickTiefeNeuBestimmen();
  void SetzeRundflug(bool r = true) { IsRundflug = r; };
  void SetAspekt(float a);

 private:
  float Pos_xCM();
  float Pos_yCM();
  float Pos_zCM();
  float Aspekt;
  float Pos_x,Pos_y,Pos_z,Alpha,Beta,FOV;
  float Soll_Pos_x,Soll_Pos_y,Soll_Pos_z,Soll_Alpha,Soll_Beta,Soll_FOV;
  float d_Pos_x,d_Pos_y,d_Pos_z,d_Alpha,d_Beta,d_FOV;
  float Nah,Fern;
  int KameraNummer;
  bool IsRundflug;  

  static float BewegFaktor;
  static float DrehFaktor;
  static float Positionen[12][6]; // Array zum Abspeichern der Kamerapositionen
};

#endif
