#ifndef MOON_H
#define MOON_H
// moon.h
// class definition for Moon
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include <string>
using namespace std;
#include <GL/glut.h>

class Moon
{
 protected:
  int planetID;
  int moonID;
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
  GLUquadricObj* moonObj;

 public:
  Moon();
  ~Moon();
  void draw();
  void parseTex();
  void setData(int pID, float inc, float oa, float od, float at, float sp,
	       float sz, char* t, int i);
  void setID(int i);

};

#endif
