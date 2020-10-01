// light.cc
// class implementation for Light
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "light.h"

void Light::setLocation(float a, float b, float c)
{
  x = a;
  y = b;
  z = c;
}

void Light::setColor(RGBColor cl)
{
  c = cl;
}

Vector Light::getLocation()
{
  float arr[4];
  arr[0] = x;
  arr[1] = y;
  arr[2] = z;
  arr[3] = 1;

  Vector v;
  v.setData(arr);
  
  return v;
}

RGBColor Light::getColor()
{
  return c;
}
