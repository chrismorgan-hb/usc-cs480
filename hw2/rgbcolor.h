// rgbcolor.h
// class definition for RGBColor
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef RGBCOLOR_H
#define RGBCOLOR_H

//abstraction of a RGB color
//using array of unsigned ints
class RGBColor
{
 protected:
  unsigned int r;
  unsigned int g;
  unsigned int b;

 public:
  RGBColor(unsigned int r, unsigned int g, unsigned int b);
  RGBColor();

  void setColor(unsigned int r, unsigned int g, unsigned int b);

  unsigned int getR();
  unsigned int getG();
  unsigned int getB();
};

#endif
