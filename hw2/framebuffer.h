#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

// framebuffer.h
// class definition for the Framebuffer
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "globals.h"
#include "rgbcolor.h"

// abstraction of the screen pixel array
// pixels can have rgb values from 0-255 integer
class Framebuffer
{
 protected:
  int **buffer; // pixel array
  RGBColor **colors; // color array
  float **zBuffer; // depth array
  int width;
  int height;
 public:
  Framebuffer(int w, int h);
  ~Framebuffer();
  int getWidth();
  int getHeight();
  
  void fillPixel(int x, int y, float z, RGBColor c);
  void writeOut();
};
#endif
