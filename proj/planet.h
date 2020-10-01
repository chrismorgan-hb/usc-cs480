#ifndef PLANET_H
#define PLANET_H
// planet.h
// class definition for Planet
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include <string>
#include <GL/glut.h>
#include "moon.h"

class Planet
{
 protected:
  string name;
  float inclination;
  float orbitAngle;
  float orbitDist;
  float axialTilt;
  float spin;
  float size;
  string texFile;
  unsigned char* texArr;
  int texWidth;
  int texHeight;
  GLuint texName;
  GLUquadricObj* planetObj;
  int planetID;
  bool chased;
  int numMoons;
  Moon* moons[3];

 public:
  Planet();
  ~Planet();
  void chaseMe();
  void stopChasingMe();
  void draw();
  void parseTex();
  void setData(string n, float inc, float oa, float od, float at, float sp,
	       float sz, string t, int i);
  string getName();
  float getInclination();
  float getOrbitAngle();
  float getOrbitDist();
  float getAxialTilt();
  float getSpin();
  float getSize();
  void addMoon(Moon* m);
  Moon* getMoon(int i);
};

#endif
