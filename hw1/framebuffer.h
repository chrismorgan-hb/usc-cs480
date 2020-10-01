#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

// framebuffer.h
// class definition for the Framebuffer
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "globals.h"

class Framebuffer
{
 protected:
  int **buffer;
  int width;
  int height;
 public:
  Framebuffer(int w, int h);
  ~Framebuffer();
  void drawLine(int x0, int y0, int x1, int y1);
  void setPixel(int x, int y);
  void writeOut();
  void line1(int x0, int y0, int x1, int y1);
  void line2(int x0, int y0, int x1, int y1);
  void line3(int x0, int y0, int x1, int y1);
  void line4(int x0, int y0, int x1, int y1);
  void line5(int x0, int y0, int x1, int y1);
  void line6(int x0, int y0, int x1, int y1);
  void line7(int x0, int y0, int x1, int y1);
  void line8(int x0, int y0, int x1, int y1);
  void lineh(int x0, int y0, int x1, int y1);
  void linev(int x0, int y0, int x1, int y1);
};
#endif
