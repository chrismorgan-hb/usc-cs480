// polygonedge.h
// class definition for PolygonEdge
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef POLYGONEDGE_H
#define POLYGONEDGE_H

#include "vector.h"
#include "rgbcolor.h"
#include "globals.h"

// abstraction of an "edge" between two verticies
// start.y < end.y
// used to determine x-intercept for edges in scanline functions
class PolygonEdge
{
 protected:
  Vector start; // always smallest y-value
  Vector end;   // always greatest y-value
  Vector startNormal;
  Vector endNormal;
  int delX;
  int delY;     // always >= 0
  int numOfPoints;
  int points[MAXEDGELENGTH][2]; // [i][0] = x, [i][1] = y

  RGBColor startColor;
  RGBColor endColor;

 public:
  PolygonEdge();
  void setEdge(Vector a, RGBColor ca, Vector b, RGBColor cb);
  void setEdge(Vector a, RGBColor ca, Vector vA, Vector b, RGBColor cb, Vector vB);
  
  int getDelX();
  int getDelY();
  Vector getStart();
  Vector getEnd();
  RGBColor getStartColor();
  RGBColor getEndColor();
  Vector getStartNormal();
  Vector getEndNormal();
  int getX(int i);
  int getY(int i);
  int getNumPoints();

  int findIntersection(int y);

  PolygonEdge& operator=(PolygonEdge& rhs);

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
  
  void setPixel(int x, int y);
};

#endif


