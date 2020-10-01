// light.h
// class definition for Light
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef LIGHT_H
#define LIGHT_H

#include "rgbcolor.h"
#include "vector.h"

class Light
{
 protected:
  // x,y,z = position of light
  float x;
  float y;
  float z;

  RGBColor c;

 public:
  void setLocation(float x, float y, float z);
  void setColor(RGBColor c);
  Vector getLocation();
  RGBColor getColor();
};

#endif

