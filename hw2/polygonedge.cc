// polygonedge.cc
// class implementation for PolygonEdge
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "polygonedge.h"
#include "utility.h"
#include <iostream>
using namespace std;

PolygonEdge::PolygonEdge()
{
  numOfPoints = 0;
}

// assignment operator
// does a deep assignment of the points array
PolygonEdge& PolygonEdge::operator=(PolygonEdge& rhs)
{
  start = rhs.getStart();
  end = rhs.getEnd();

  delX = rhs.getDelX();
  delY = rhs.getDelY();
  numOfPoints = rhs.getNumPoints();

  for(int i = 0; i < MAXEDGELENGTH; i++)
  {
    points[i][0] = rhs.getX(i);
    points[i][1] = rhs.getY(i);
  }
  
  startColor = rhs.getStartColor();
  endColor = rhs.getEndColor();

  startNormal = rhs.getStartNormal();
  endNormal = rhs.getEndNormal();

  return *this;
}

Vector PolygonEdge::getStartNormal()
{
  return startNormal;
}

Vector PolygonEdge::getEndNormal()
{
  return endNormal;
}

int PolygonEdge::getNumPoints()
{
  return numOfPoints;
}

int PolygonEdge::getX(int i)
{
  return points[i][0];
}

int PolygonEdge::getY(int i)
{
  return points[i][1];
}

// setPixel
// used to set pixels that define an edge
void PolygonEdge::setPixel(int x, int y)
{
  points[numOfPoints][0] = x;
  points[numOfPoints][1] = y;

  numOfPoints++;
}

// setEdge
// takes in two vectors and two colors for those vectors
// rasterizes a pseudo-line definining an edge between a and b
void PolygonEdge::setEdge(Vector a, RGBColor ca, Vector b, RGBColor cb)
{
  // make sure start is always below end
  if(a.getElement(1) > b.getElement(1))
    {
      start = b;
      startColor = cb;
      end = a;
      endColor = ca;
    }

  else
    {
      start = a;
      startColor = ca;
      end = b;
      endColor = cb;
    }
  
  delX = round(end.getElement(0) - start.getElement(0));
  delY = round(end.getElement(1) - start.getElement(1));

  int x0 = round(start.getElement(0));
  int x1 = round(end.getElement(0));
  int y0 = round(start.getElement(1));
  int y1 = round(end.getElement(1));
  int oct = findOctant(x0, y0, x1, y1);

  switch(oct)
    {
    case OCT1:
      // draw oct1
      line1(x0, y0, x1, y1);
      break;
      
    case OCT2:
      // draw oct2
      line2(x0, y0, x1, y1);
      break;
      
    case OCT3:
      // draw oct3
      line3(x0, y0, x1, y1);
      break;
      
    case OCT4:
      // draw oct4
      line4(x0, y0, x1, y1);
      break;
      
    case OCT5:
      // draw oct5
      line5(x0, y0, x1, y1);
      break;
      
    case OCT6:
      // draw oct6
      line6(x0, y0, x1, y1);
      break;
      
    case OCT7:
      // draw oct7
      line7(x0, y0, x1, y1);
      break;
      
    case OCT8:
      // draw oct8
      line8(x0, y0, x1, y1);
      break;
      
    case HORIZONTAL:
      // draw horizontal
      lineh(x0, y0, x1, y1);
      break;
      
    case VERTICAL:
      // draw vertical
      linev(x0, y0, x1, y1);
      break;
      
    default:
      cout << "Error: no valid octant specified." << endl;
      break;
    }
}

// setEdge
// same as above, but includes vertex normals for vector a and b
void PolygonEdge::setEdge(Vector a, RGBColor ca, Vector vA, Vector b, RGBColor cb, Vector vB)
{
  // make sure start is always below end
  if(a.getElement(1) > b.getElement(1))
    {
      start = b;
      startColor = cb;
      startNormal = vB;
      end = a;
      endColor = ca;
      endNormal = vA;
    }

  else
    {
      start = a;
      startColor = ca;
      startNormal = vA;
      end = b;
      endColor = cb;
      endNormal = vB;
    }
  
  delX = round(end.getElement(0) - start.getElement(0));
  delY = round(end.getElement(1) - start.getElement(1));

  int x0 = round(start.getElement(0));
  int x1 = round(end.getElement(0));
  int y0 = round(start.getElement(1));
  int y1 = round(end.getElement(1));
  int oct = findOctant(x0, y0, x1, y1);

  switch(oct)
    {
    case OCT1:
      // draw oct1
      line1(x0, y0, x1, y1);
      break;
      
    case OCT2:
      // draw oct2
      line2(x0, y0, x1, y1);
      break;
      
    case OCT3:
      // draw oct3
      line3(x0, y0, x1, y1);
      break;
      
    case OCT4:
      // draw oct4
      line4(x0, y0, x1, y1);
      break;
      
    case OCT5:
      // draw oct5
      line5(x0, y0, x1, y1);
      break;
      
    case OCT6:
      // draw oct6
      line6(x0, y0, x1, y1);
      break;
      
    case OCT7:
      // draw oct7
      line7(x0, y0, x1, y1);
      break;
      
    case OCT8:
      // draw oct8
      line8(x0, y0, x1, y1);
      break;
      
    case HORIZONTAL:
      // draw horizontal
      lineh(x0, y0, x1, y1);
      break;
      
    case VERTICAL:
      // draw vertical
      linev(x0, y0, x1, y1);
      break;
      
    default:
      cout << "Error: no valid octant specified." << endl;
      break;
    }
}

//lineX
//where X = 1-8 or h or v
//draws pseudo-line in octant 1-8 using midpoint algorithm
//h or v draws horizontal or vertical respectively
//these pseudo-lines define edges for scanline algorithm

//line1
//draw line in OCT1
void PolygonEdge::line1(int x0, int y0, int x1, int y1)
{
  int curX = x0;
  int curY = y0;
  int delX = x1 - x0;
  int delY = y1 - y0;
  float d = delY - 0.5*delX;

  setPixel(curX, curY);

  while(curX < x1)
    {
      if(d > 0) //NE
	{
	  curX++;
	  curY++;
	  d += (delY - delX);
	  setPixel(curX, curY);
	}

      else //E
	{
	  curX++;
	  d += delY;
	  setPixel(curX, curY);
	}
    }
}

//line2
//draw line in OCT2
void PolygonEdge::line2(int x0, int y0, int x1, int y1)
{
  int curX = x0;
  int curY = y0;
  int delX = x1 - x0;
  int delY = y1 - y0;
  float d = 0.5*delY - delX;

  setPixel(curX, curY);

  while(curY < y1)
    {
      if(d >= 0) //N
	{
	  curY++;
	  d += -delX;
	  setPixel(curX, curY);
	}

      else //NE
	{
	  curX++;
	  curY++;
	  d += (delY - delX);
	  setPixel(curX, curY);
	}
    }
}

//line3
//draw line in OCT3
void PolygonEdge::line3(int x0, int y0, int x1, int y1)
{
  int curX = x0;
  int curY = y0;
  int delX = x1 - x0;
  int delY = y1 - y0;
  float d = -0.5*delY - delX;

  setPixel(curX, curY);
  
  while(curY < y1)
    {
      if(d <= 0) //N
	{
	  curY++;
	  d += -delX;
	  setPixel(curX, curY);
	}

      else //NW
	{
	  curX--;
	  curY++;
	  d += -(delY + delX);
	  setPixel(curX, curY);
	}
    }
}

//line4
//draw line in OCT4
void PolygonEdge::line4(int x0, int y0, int x1, int y1)
{
  int curX = x0;
  int curY = y0;
  int delX = x1 - x0;
  int delY = y1 - y0;
  float d = -delY - 0.5*delX;

  setPixel(curX, curY);
  
  while(curX > x1)
    {
      if(d >= 0) //W
	{
	  curX--;
	  d += -delY;
	  setPixel(curX, curY);
	}
      
      else //NW
	{
	  curX--;
	  curY++;
	  d += -(delY + delX);
	  setPixel(curX, curY);
	}
    }
}

//line5
//draw line in OCT5
void PolygonEdge::line5(int x0, int y0, int x1, int y1)
{
  int curX = x0;
  int curY = y0;
  int delX = x1 - x0;
  int delY = y1 - y0;
  float d = -delY + 0.5*delX;

  setPixel(curX, curY);

  while(curX > x1)
    {
      if(d <= 0) //W
	{
	  curX--;
	  d += -delY;
	  setPixel(curX, curY);
	}
      
      else //SW
	{
	  curX--;
	  curY--;
	  d += -(delY - delX);
	  setPixel(curX, curY);
	}
    }
}

//line6
//draw line in OCT6
void PolygonEdge::line6(int x0, int y0, int x1, int y1)
{
  int curX = x0;
  int curY = y0;
  int delX = x1 - x0;
  int delY = y1 - y0;
  float d = -0.5*delY + delX;

  setPixel(curX, curY);

  while(curY > y1)
    {
      if(d > 0) //S
	{
	  curY--;
	  d += delX;
	  setPixel(curX, curY);
	}

      else //SW
	{
	  curY--;
	  curX--;
	  d += -(delY - delX);
	  setPixel(curX, curY);
	}
    }
}

//draw7
//draw line in OCT7
void PolygonEdge::line7(int x0, int y0, int x1, int y1)
{
  int curX = x0;
  int curY = y0;
  int delX = x1 - x0;
  int delY = y1 - y0;
  float d = 0.5*delY + delX;

  setPixel(curX, curY);
  
  while(curY > y1)
    {
      if(d <= 0) //S
	{
	  curY--;
	  d += delX;
	  setPixel(curX, curY);
	}
      
      else //SE
	{
	  curX++;
	  curY--;
	  d += delY + delX;
	  setPixel(curX, curY);
	}
    }
}

//line8
//draw line in OCT8
void PolygonEdge::line8(int x0, int y0, int x1, int y1)
{
  int curX = x0;
  int curY = y0;
  int delX = x1 - x0;
  int delY = y1 - y0;
  float d = delY + 0.5*delX;

  setPixel(curX, curY);
  
  while(curX < x1)
    {
      if(d >= 0) //E
	{
	  curX++;
	  d += delY;
	  setPixel(curX, curY);
	}
      
      else //SE
	{
	  curX++;
	  curY--;
	  d += delY + delX;
	  setPixel(curX, curY);
	}
    }
}

//lineh
//draw horizontal line
void PolygonEdge::lineh(int x0, int y0, int x1, int y1)
{
  int temp = 0;
  if(x0 > x1)
    {
      temp = x0;
      x0 = x1;
      x1 = temp;
    }

  for(int i = x0; i < x1; i++)
    setPixel(i, y0);
}

//linev
//draw vertical line
void PolygonEdge::linev(int x0, int y0, int x1, int y1)
{
  int temp = 0;
  if(y0 > y1)
    {
      temp = y0;
      y0 = y1;
      y1 = temp;
    }

  for(int i = y0; i < y1; i++)
    setPixel(x0, i);
}

Vector PolygonEdge::getStart()
{
  return start;
}

Vector PolygonEdge::getEnd()
{
  return end;
}

int PolygonEdge::getDelX()
{
  return delX;
}

int PolygonEdge::getDelY()
{
  return delY;
}

RGBColor PolygonEdge::getStartColor()
{
  return startColor;
}

RGBColor PolygonEdge::getEndColor()
{
  return endColor;
}

//findIntersection
//determines the x-intersection of point y on this edge
//returns -1 if no intersection found
int PolygonEdge::findIntersection(int y)
{
  for(int i = 0; i < numOfPoints; i++)
    {
      if(points[i][1] == y)
	return points[i][0];
    }

  return -1;
}
