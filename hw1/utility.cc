// utility.cc
// function definitions for utility functions
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "utility.h"
#include <iostream>
using namespace std;

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


