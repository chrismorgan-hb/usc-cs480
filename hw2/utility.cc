// utility.cc
// function definitions for utility functions
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "utility.h"
#include <iostream>
using namespace std;
#include <math.h>

//findOctant
//determines which octant the line defined by (x0,y0)(x1,y1) falls in
//used by drawLine
int findOctant(int x0, int y0, int x1, int y1)
{
  float fx0 = (float)x0;
  float fx1 = (float)x1;
  float fy0 = (float)y0;
  float fy1 = (float)y1;

  if(y1 == y0)
    return HORIZONTAL;
  
  if(x1 == x0)
    return VERTICAL;
  
  float slope = (fy1 - fy0)/(fx1 - fx0);
  
  if(y1 >= y0) // above x-axis
    {
      if(x1 > x0) // right of y-axis
	{
	  if(slope <= 1)
	    return OCT1;
	  else
	    return OCT2;
	}
      else
	{
	  if(slope <= -1)
	    return OCT3;
	  else
	    return OCT4;
	}
    }
  else if(y1 < y0) // below x-axis
    {
      if(x1 < x0) // left of y-axis
	{
	  if(slope <= 1)
	    return OCT5;
	  else
	    return OCT6;
	}
      else
	{
	  if(slope <= -1)
	    return OCT7;
	  else
	    return OCT8;
	}
    }
  
  return -1;
}

//average
//returns a vector where element(0) = avg(a(0),b(0)) etc.
Vector average(Vector a, Vector b, Vector c)
{
  Vector result;
  result.setElement(0, 
		    (a.getElement(0) + b.getElement(0) + c.getElement(0))/3.0);
  result.setElement(1,
		    (a.getElement(1) + b.getElement(1) + c.getElement(1))/3.0);
  result.setElement(2,
		    (a.getElement(2) + b.getElement(2) + c.getElement(2))/3.0);

  return result;
}

//round
//rounds f to nearest integer
int round(float f)
{
  int i;
  if(f > 0)
    i = (int)(floor(f + 0.5));

  else
    {
      f = f * -1;
      i = (int)(floor(f + 0.5));
      i = i * -1;
    }

  return i;
}

// interpolate
// determines a value for point cur based on i1 @ p1 and i2 @ p2
// will always return a value in range [i1, i2]
// p1 = end, p2 = start
float interpolate(float p1, float p2, float cur, float i1, float i2)
{
  int dp = round(p1 - p2);
  if(dp != 0)
    return ((cur - p2)/dp)*i1 + ((p1-cur)/dp)*i2;
  
  else
    return i1;
}

//interpolateC
//special case of interpolate for color elements
int interpolateC(float p1, float p2, float cur, float i1, float i2)
{
  int dp = round(p1 - p2);
  int ret = 0;
  if(dp != 0) 
    {
      ret = round(((cur - p2)/dp)*i1 + ((p1-cur)/dp)*i2);
      return ret;
    }

  else
    {
      return round(i1);
    }
}

//interpolateColor
//interpolates colors using interpolateC
RGBColor interpolateColor(float p1, float p2, float cur, RGBColor i1, RGBColor i2)
{
  int dp = round(p1 - p2);

  int R,G,B;
  RGBColor ret;

  if(dp != 0)
    {
      R = interpolateC(p1, p2, cur, i1.getR(), i2.getR());
      G = interpolateC(p1, p2, cur, i1.getG(), i2.getG());
      B = interpolateC(p1, p2, cur, i1.getB(), i2.getB());

      ret.setColor(R, G, B);
      return ret;
    }
  
  else
      return i1;
}

//interpolateV
//special case of interpolate for vector elements
//helper function for interpolateVector
float interpolateV(float p1, float p2, float cur, float i1, float i2)
{
  int dp = round(p1 - p2);
  float ret = 0;
  if(dp != 0) 
    {
      ret = ((cur - p2)/dp)*i1 + ((p1-cur)/dp)*i2;
      return ret;
    }

  else
    {
      return i1;
    }
}

//interpolateVector
//interpolates Vectors using interpolateV
Vector interpolateVector(float p1, float p2, float cur, Vector i1, Vector i2)
{
  Vector ret;
  int dp = round(p1 - p2);

  if(dp != 0)
    {
      ret.setElement(0, interpolateV(p1, p2, cur, i1.getElement(0), 
				     i2.getElement(0)));
      ret.setElement(1, interpolateV(p1, p2, cur, i1.getElement(1),
				     i2.getElement(1)));
      ret.setElement(2, interpolateV(p1, p2, cur, i1.getElement(2),
				     i2.getElement(2)));

      return ret;
    }

  else
    {
      return i1;
    }
}
