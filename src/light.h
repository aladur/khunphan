/**********************************************************************************
**
**  light.h        Copyright (C) 2001 by Stefan Disch, Tobias Nopper, Martina Welte
**                 Modified 2002,2003 for KhunPhan by W. Schwotzer
**
***********************************************************************************/

#ifndef LIGHT_H
#define LIGHT_H

class Light {
public: 
  Light(bool AmbientLight = true, int LightSources = 1, bool Reflections = false);
  ~Light();

  void Update(bool AmbientLight, int LightSources = 1, bool Reflections = false);
  void Draw() const;
  void GetPosition(float &px, float &py, float &pz) const;
  void SetPosition(float px, float py, float pz);
private:
  int LightSources;
  float posX, posY, posZ;
};

#endif
