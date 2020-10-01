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
  int width;
  int height;

 public:
  Framebuffer(int w, int h);
  int getWidth();
  int getHeight();
  void setWidth(int w);
  void setHeight(int h);  
};
#endif
