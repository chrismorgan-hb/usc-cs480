// rgbcolor.cc
// class implementation for RGBColor
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "rgbcolor.h"

RGBColor::RGBColor()
{
  r = 0;
  g = 0;
  b = 0;
}

//constructor w/ args
//ensures that values are between 0-255
RGBColor::RGBColor(unsigned int a, unsigned int c, unsigned int d)
{
  if(a < 256)
    r = a;
  else
    r = 255;

  if(c < 256)
    g = c;
  else
    g = 255;

  if(d < 256)
    b = d;
  else
    b = 255;
}

//setColor
//ensures that values are [0-255]
void RGBColor::setColor(unsigned int a, unsigned int c, unsigned int d)
{
  if(a < 256)
    r = a;
  else
    r = 255;

  if(c < 256)
    g = c;
  else
    g = 255;

  if(d < 256)
    b = d;
  else
    b = 255;
}

unsigned int RGBColor::getR()
{
  return r;
}

unsigned int RGBColor::getG()
{
  return g;
}

unsigned int RGBColor::getB()
{
  return b;
}
