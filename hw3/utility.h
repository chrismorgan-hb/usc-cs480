// utility.h
// useful function definitions
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef UTILITY_H
#define UTILITY_H

#include "matrix.h"
#include "vector.h"
#include "rgbcolor.h"
#include "globals.h"

int findOctant(int x0, int y0, int x1, int y1);
Vector average(Vector a, Vector b, Vector c);
int round(float f);
float interpolate(float p1, float p2, float cur, float i1, float i2);
RGBColor interpolateColor(float p1, float p2, float cur, RGBColor i1, RGBColor i2);
int interpolateC(float p1, float p2, float cur, float i1, float i2);
Vector interpolateVector(float p1, float p2, float cur, Vector i1, Vector i2);
float interpolateV(float p1, float p2, float cur, float i1, float i2);

#endif









