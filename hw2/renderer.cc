// renderer.cc
// class implementation for Renderer
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "renderer.h"
#include "globals.h"
#include <fstream>
#include <string>
#include <math.h>
using namespace std;
#include <stdio.h>
#include <ctype.h>
#include "camera.h"
#include "utility.h"
#include "light.h"
#include <limits.h>
#include "polygonedge.h"

class Camera;

int tokenize(char* token);

Renderer::Renderer(int s)
{
  theModelToRender = new Model;
  numLights = 0;
  shadingType = s;
}

Renderer::~Renderer()
{
  delete framebuffer;
  delete theModelToRender;
}

// scanline
// determines shadingType and calls appropriate scan function
void Renderer::scanline(int face)
{
  if(shadingType == SHADE_FLAT)
    scanFlat(face);

  else if(shadingType == SHADE_GOURAUD)
    scanGouraud(face);

  else if(shadingType == SHADE_PHONG)
    scanPhong(face);
}

//scanFlat
//flat shading, uses face normal
void Renderer::scanFlat(int face)
{
  RGBColor faceColor = theModelToRender->getFaceColor(face);
  Vector A, B, C;
  
  A = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[0]);
  B = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[1]);
  C = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[2]);

  PolygonEdge e1; 
  PolygonEdge e2; 
  PolygonEdge e3;
  
  e1.setEdge(A, faceColor, B, faceColor);
  e2.setEdge(B, faceColor, C, faceColor);
  e3.setEdge(C, faceColor, A, faceColor);

  // edge list
  PolygonEdge edges[3];

  // sort edge list by ascending y
  // edges[0] is bottom

  if(e1.getStart().getElement(1) < e2.getStart().getElement(1) && 
     e1.getStart().getElement(1) < e3.getStart().getElement(1))
    {
      edges[0] = e1;

      if(e2.getStart().getElement(1) < e3.getStart().getElement(1))
	{
	  edges[1] = e2;
	  edges[2] = e3;
	}

      else
	{
	  edges[1] = e3;
	  edges[2] = e2;
	}
    }
  
  else if(e2.getStart().getElement(1) < e3.getStart().getElement(1) && 
	  e2.getStart().getElement(1) < e1.getStart().getElement(1))
    {
      edges[0] = e2;

      if(e1.getStart().getElement(1) < e3.getStart().getElement(1))
	{
	  edges[1] = e1;
	  edges[2] = e3;
	}

      else
	{
	  edges[1] = e3;
	  edges[2] = e1;
	}
    }
  
  else
    {
      edges[0] = e3;

      if(e1.getStart().getElement(1) < e2.getStart().getElement(1))
	{
	  edges[1] = e1;
	  edges[2] = e2;
	}
      
      else
	{
	  edges[1] = e2;
	  edges[2] = e1;
	}
    }

  // edge list now sorted
  
  int yMin = round(edges[0].getStart().getElement(1));
  int yMax = round(edges[2].getEnd().getElement(1));
  
  int xLeft = 0;
  int xRight = 0;
  float zLeft = INT_MAX;
  float zRight = INT_MAX;
  float curZ = INT_MAX;
  int curX = 0;
  int xTemp = 0;
  PolygonEdge tempEdge;
  bool swapped = false;

  int curY = yMin;
  PolygonEdge active[2]; // 0 = left, 1 = right

  // check for horizontal line on bottom
  if(edges[0].getDelY() == 0)
    {
      // draw horizontal line
      
      xTemp = round(edges[0].getStart().getElement(0));

      // make sure we draw from left to right
      if(xTemp < round(edges[0].getEnd().getElement(0)))
	{
	  xLeft = xTemp;
	  xRight = round(edges[0].getEnd().getElement(0));
	}
      else
	{
	  xLeft = round(edges[0].getStart().getElement(0));
	  xRight = round(edges[0].getEnd().getElement(0));
	  swapped = true;
	}

      if(xLeft > xRight)
	{
	  xTemp = xLeft;
	  xLeft = xRight;
	  xRight = xTemp;
	}

      curX = xLeft;

      // for each x until 1 before the right-hand edge
      while(curX < xRight)
	{
	  // find z @ curX by interpolation from z @ start and end of edge
	  if(swapped)
	    {
	      curZ = interpolate(xRight, xLeft, curX, edges[0].getStart().getElement(2), 
				 edges[0].getEnd().getElement(2));
	    }
	  else
	    {
	      curZ = interpolate(xRight, xLeft, curX, edges[0].getEnd().getElement(2), 
				 edges[0].getStart().getElement(2));
	    }

	  framebuffer->fillPixel(curX, curY, curZ, faceColor);
	  curX++;
	}

      curY++;
      
      // put final two edges in active edge list, left in 0, right in 1
      if(edges[1].getDelX() < edges[2].getDelX())
	{
	  active[0] = edges[1];
	  active[1] = edges[2];
	}
      
      else
	{
	  active[0] = edges[2];
	  active[1] = edges[1];
	}
    }

  // no horizontal line on bottom
  else
  {
      // sort active edge list from left to right
      if(edges[0].getDelX() < edges[1].getDelX())
	{
	  active[0] = edges[0];
	  active[1] = edges[1];
	}
      
      else
	{
	  active[0] = edges[1];
	  active[1] = edges[0];
	}
  }

  curX = round(active[0].getStart().getElement(0));

  // for each scanline
  while(curY < yMax)
    {
      // vertex event with left edge
      if(curY == active[0].getDelY() + yMin)
	active[0] = edges[2];

      // vertex event with right edge
      else if(curY == active[1].getDelY() + yMin)
	active[1] = edges[2];

      // find xLeft and xRight on the edge at curY
      xLeft = active[0].findIntersection(curY);
      xRight = active[1].findIntersection(curY);

      // make sure these points are on the edge
      if(xLeft == -1 || xRight == -1)
	{
	  //cout << "error, point not found on edge" << endl;
	  curY++;
	  continue;
	}

      // make sure xLeft is actually left of xRight
      // if not, swap them
      if (xLeft > xRight)
	{
	  tempEdge = active[0];
	  active[0] = active[1];
	  active[1] = tempEdge;
	  xLeft = active[0].findIntersection(curY);
	  xRight = active[1].findIntersection(curY);
	}
	 
      curX = xLeft;

      // get zLeft and zRight for this scanline
      zLeft = interpolate(round(active[0].getEnd().getElement(1)), 
			  round(active[0].getStart().getElement(1)), curY,
			  active[0].getEnd().getElement(2), active[0].getStart().getElement(2));

      zRight = interpolate(round(active[1].getEnd().getElement(1)), 
			   round(active[1].getStart().getElement(1)), curY,
			   active[1].getEnd().getElement(2), active[1].getStart().getElement(2));

      // if we are at the very bottom vertex, fill it
      if(curY == yMin)
	framebuffer->fillPixel(curX, curY, zLeft, faceColor);
      
      // don't draw top pixel, shadow edges
      else if (curY == yMax)
	break;
	
      // move across the scanline at curY from left to right
      while(curX < xRight)
	{ 
	  // determine z value for this point using interpolation
	  curZ = interpolate(xRight, xLeft, curX, zRight, zLeft);
	  
	  // fill the pixel with the face color
	  framebuffer->fillPixel(curX, curY, curZ, faceColor);
	  curX++;
	}
      curY++;
      }
}

// scanGouraud
// uses interpolation of vertex colors to determine a color at any one point
// interpolation is linear
void Renderer::scanGouraud(int face)
{
  Vector A, B, C;
  
  A = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[0]);
  B = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[1]);
  C = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[2]);

  RGBColor a, b, c;
  
  // get the vertex colors
  a = theModelToRender->getVertexColor((theModelToRender->getFace(face))[0]);
  b = theModelToRender->getVertexColor((theModelToRender->getFace(face))[1]);
  c = theModelToRender->getVertexColor((theModelToRender->getFace(face))[2]);

  PolygonEdge e1; 
  PolygonEdge e2; 
  PolygonEdge e3;
  
  e1.setEdge(A, a, B, b);
  e2.setEdge(B, b, C, c);
  e3.setEdge(C, c, A, a);

  // edge list
  PolygonEdge edges[3];

  // sort edge list by ascending y
  
  // e1 is lowest
  if(e1.getStart().getElement(1) < e2.getStart().getElement(1) && 
     e1.getStart().getElement(1) < e3.getStart().getElement(1))
    {
      edges[0] = e1;

      if(e2.getStart().getElement(1) < e3.getStart().getElement(1))
	{
	  edges[1] = e2;
	  edges[2] = e3;
	}

      else
	{
	  edges[1] = e3;
	  edges[2] = e2;
	}
    }
  
  // e2 is lowest
  else if(e2.getStart().getElement(1) < e3.getStart().getElement(1) && 
	  e2.getStart().getElement(1) < e1.getStart().getElement(1))
    {
      edges[0] = e2;

      if(e1.getStart().getElement(1) < e3.getStart().getElement(1))
	{
	  edges[1] = e1;
	  edges[2] = e3;
	}

      else
	{
	  edges[1] = e3;
	  edges[2] = e1;
	}
    }
  
  // e3 is lowest
  else
    {
      edges[0] = e3;

      if(e1.getStart().getElement(1) < e2.getStart().getElement(1))
	{
	  edges[1] = e1;
	  edges[2] = e2;
	}
      
      else
	{
	  edges[1] = e2;
	  edges[2] = e1;
	}
    }

  // edge list now sorted
  
  int yMin = round(edges[0].getStart().getElement(1));
  int yMax = round(edges[2].getEnd().getElement(1));
  int xLeft = 0;
  int xRight = 0;
  float zLeft = INT_MAX;
  float zRight = INT_MAX;
  float curZ = INT_MAX;
  int curX = 0;
  int xTemp = 0;
  PolygonEdge tempEdge;
  bool swapped = false;
  RGBColor tempColor;

  int curY = yMin;
  PolygonEdge active[2]; // 0 = left, 1 = right

  // check for horizontal line on bottom
  if(edges[0].getDelY() == 0)
    {
      // draw horizontal line

      xTemp = round(edges[0].getStart().getElement(0));
      if(xTemp < round(edges[0].getEnd().getElement(0)))
	{
	  xLeft = xTemp;
	  xRight = round(edges[0].getEnd().getElement(0));
	}
      else
	{
	  xLeft = round(edges[0].getStart().getElement(0));
	  xRight = round(edges[0].getEnd().getElement(0));
	  swapped = true;
	}

      // make sure that xLeft is really left of xRight
      // if not, swap them
      if(xLeft > xRight)
	{
	  xTemp = xLeft;
	  xLeft = xRight;
	  xRight = xTemp;
	} 

      curX = xLeft;
      
      while(curX < xRight)
	{
	  // find z @ curX by interpolation
	  // find color @ curX by interpolation
	  if(swapped)
	    {
	      curZ = interpolate(xRight, xLeft, curX, edges[0].getStart().getElement(2), 
				 edges[0].getEnd().getElement(2));
	      tempColor = interpolateColor(xRight, xLeft, curX, edges[0].getStartColor(), 
					   edges[0].getEndColor());
	    }
	  else
	    {
	      curZ = interpolate(xRight, xLeft, curX, edges[0].getEnd().getElement(2), 
				 edges[0].getStart().getElement(2));
	      tempColor = interpolateColor(xRight, xLeft, curX, edges[0].getEndColor(), 
					   edges[0].getStartColor());
	    }

	  framebuffer->fillPixel(curX, curY, curZ, tempColor);
	  curX++;
	}

      curY++;
      
      // put remaining two edges in active edge list
      // sorted from left to right

      if(edges[1].getDelX() < edges[2].getDelX())
	{
	  active[0] = edges[1];
	  active[1] = edges[2];
	}
      
      else
	{
	  active[0] = edges[2];
	  active[1] = edges[1];
	}
    }

  // no horizontal line on bottom
  else
    {
      if(edges[0].getDelX() < edges[1].getDelX())
	{
	  active[0] = edges[0];
	  active[1] = edges[1];
	}
      
      else
	{
	  active[0] = edges[1];
	  active[1] = edges[0];
	}
    }

  curX = round(active[0].getStart().getElement(0));

  while(curY < yMax)
    {
      // vertex event with left edge
      if(curY == active[0].getDelY() + yMin)
	active[0] = edges[2];

      // vertex event with right edge
      else if(curY == active[1].getDelY() + yMin)
	active[1] = edges[2];

      // find intersection of scanline @ curY with left and right edges
      xLeft = active[0].findIntersection(curY);
      xRight = active[1].findIntersection(curY);
      
      // make sure we do contact the edge
      if(xLeft == -1 || xRight == -1)
	{
	  //cout << "error, point not found on edge" << endl;
	  curY++;
	  continue;
	}

      // make sure xLeft is really to the left
      // if not, swap
      if (xLeft > xRight)
	{
	  tempEdge = active[0];
	  active[0] = active[1];
	  active[1] = tempEdge;
	  xLeft = active[0].findIntersection(curY);
	  xRight = active[1].findIntersection(curY);
	}
	  
      curX = xLeft;

      // determine zLeft and zRight by interpolating along edges

      zLeft = interpolate(round(active[0].getEnd().getElement(1)), 
			  round(active[0].getStart().getElement(1)), curY,
			  active[0].getEnd().getElement(2), active[0].getStart().getElement(2));

      zRight = interpolate(round(active[1].getEnd().getElement(1)), 
			   round(active[1].getStart().getElement(1)), curY,
			   active[1].getEnd().getElement(2), active[1].getStart().getElement(2));

      // if at the bottom vertex, just fill it
      if(curY == yMin)
	framebuffer->fillPixel(curX, curY, zLeft, active[0].getStartColor());
      
      // don't draw top line/vertex
      else if (curY == yMax)
	break;
     
      RGBColor leftColor, rightColor;
      
      // determine color on edges @ curY by interpolating along edges
      leftColor = interpolateColor(round(active[0].getEnd().getElement(1)), 
				   round(active[0].getStart().getElement(1)), round(curY),
				   active[0].getEndColor(), active[0].getStartColor());
      
      rightColor = interpolateColor(round(active[1].getEnd().getElement(1)), 
				    round(active[1].getStart().getElement(1)), round(curY),
				    active[1].getEndColor(), active[1].getStartColor());
      // for entire scanline @ curY
      while(curX < xRight)
	{ 
	  // determine z by interpolation between z's on left and right of scanline
	  curZ = interpolate(xRight, xLeft, curX, zRight, zLeft);

	  // determine color at current point by interpolating between 
	  // left and right edge colors
	  tempColor = interpolateColor(xRight, xLeft, curX, rightColor, leftColor);
	  
	  framebuffer->fillPixel(curX, curY, curZ, tempColor);
	  curX++;
	}
      curY++;
    }
}

// scanPhong
// uses phong shading, which interpolates normals at every
// point and calculates colors based on that
void Renderer::scanPhong(int face)
{
  Vector A, B, C;
  
  // get points
  A = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[0]);
  B = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[1]);
  C = theModelToRender->getWindowSpacePoint((theModelToRender->getFace(face))[2]);

  RGBColor a, b, c;
  
  // get vertex colors
  a = theModelToRender->getVertexColor((theModelToRender->getFace(face))[0]);
  b = theModelToRender->getVertexColor((theModelToRender->getFace(face))[1]);
  c = theModelToRender->getVertexColor((theModelToRender->getFace(face))[2]);

  Vector vA, vB, vC;
  
  // get vertex normals
  vA = theModelToRender->getVertexNormal((theModelToRender->getFace(face))[0]);
  vB = theModelToRender->getVertexNormal((theModelToRender->getFace(face))[1]);
  vC = theModelToRender->getVertexNormal((theModelToRender->getFace(face))[2]);

  PolygonEdge e1; 
  PolygonEdge e2; 
  PolygonEdge e3;
  
  e1.setEdge(A, a, vA, B, b, vB);

  e2.setEdge(B, b, vB, C, c, vC);

  e3.setEdge(C, c, vC, A, a, vA);

  // edge list
  PolygonEdge edges[3];

  // sort edge list by ascending y

  // e1 is lowest
  if(e1.getStart().getElement(1) < e2.getStart().getElement(1) && 
     e1.getStart().getElement(1) < e3.getStart().getElement(1))
    {
      edges[0] = e1;
     
      if(e2.getStart().getElement(1) < e3.getStart().getElement(1))
	{
	  edges[1] = e2;
	  edges[2] = e3;
	}

      else
	{
	  edges[1] = e3;
	  edges[2] = e2;
	}
    }
  
  // e2 is lowest
  else if(e2.getStart().getElement(1) < e3.getStart().getElement(1) && 
	  e2.getStart().getElement(1) < e1.getStart().getElement(1))
    {
      edges[0] = e2;

      if(e1.getStart().getElement(1) < e3.getStart().getElement(1))
	{
	  edges[1] = e1;
	  edges[2] = e3;
	}

      else
	{
	  edges[1] = e3;
	  edges[2] = e1;
	}
    }
  
  // e3 is lowest
  else
    {
      edges[0] = e3;

      if(e1.getStart().getElement(1) < e2.getStart().getElement(1))
	{
	  edges[1] = e1;
	  edges[2] = e2;
	}
      
      else
	{
	  edges[1] = e2;
	  edges[2] = e1;
	}
    }

  // edge list now sorted
  
  int yMin = round(edges[0].getStart().getElement(1));
  int yMax = round(edges[2].getEnd().getElement(1));
  int xLeft = 0;
  int xRight = 0;
  float zLeft = INT_MAX;
  float zRight = INT_MAX;
  float curZ = INT_MAX;
  int curX = 0;
  int xTemp = 0;
  PolygonEdge tempEdge;
  bool swapped = false;
  RGBColor tempColor;
  Vector pointNormal, I, ILd, ILs, L, V, H, N, temp, start, end, normalLeft, normalRight;
  float maxColor = 255;
  float NdotH, NdotL;

  int curY = yMin;
  PolygonEdge active[2]; // 0 = left, 1 = right

  // V is fixed at (0,0,1) for Phong shading as per homework hints on website
  V.setElement(0,0);
  V.setElement(1,0);
  V.setElement(2,1);

  // check for horizontal line on bottom
  if(edges[0].getDelY() == 0)
    {
      // draw horizontal line
      xTemp = round(edges[0].getStart().getElement(0));
      
      // make sure we're drawing from left to right
      if(xTemp < round(edges[0].getEnd().getElement(0)))
	{
	  xLeft = xTemp;
	  xRight = round(edges[0].getEnd().getElement(0));
	}
      else
	{
	  xLeft = round(edges[0].getStart().getElement(0));
	  xRight = round(edges[0].getEnd().getElement(0));
	  swapped = true;
	}

      // make sure xLeft is actually left of xRight
      // if not, swap them
      if(xLeft > xRight)
	{
	  xTemp = xLeft;
	  xLeft = xRight;
	  xRight = xTemp;
	}
     
      curX = xLeft;

      // along the bottom horizontal edge
      while(curX < xRight)
	{
	  // get normals at each end of the horizontal line
	  start = edges[0].getStartNormal();
	  end = edges[0].getEndNormal();

	  // find z @ curX by interpolation between xLeft and xRight
	  // find pointNormal by interpolation between start and end normals
	  if(swapped)
	    {
	      curZ = interpolate(xRight, xLeft, curX, edges[0].getStart().getElement(2), 
				 edges[0].getEnd().getElement(2));
	      pointNormal = interpolateVector(xRight, xLeft, curX, start, end);
	    }
	  else
	    {
	      curZ = interpolate(xRight, xLeft, curX, edges[0].getEnd().getElement(2), 
				 edges[0].getStart().getElement(2));
	      pointNormal = interpolateVector(xRight, xLeft, curX, end, start);
	    }

	  // I always initialized to ambient color
	  I = (theModelToRender->getAmbient()) * maxColor;
	  
	  // N must be normalized
	  N = pointNormal;
	  N.norm();

	  // for each light in the scene, add to the I vector
	  for(unsigned int n = 0; n < numLights; n++)
	    {
	      // L is directional light, as per homework hints on webpage
	      L = myLights[n].getLocation();
	      L.norm();
	      
	      H = L + V;
	      H.norm();
	      
	      // get diffuse parameters for light[n]
	      ILd.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getDiffuse()).getElement(0)));
	      ILd.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getDiffuse()).getElement(1)));
	      ILd.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getDiffuse()).getElement(2)));
	      
	      // get specular parameters for light[n]
	      ILs.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getSpecular()).getElement(0)));
	      ILs.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getSpecular()).getElement(1)));
	      ILs.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getSpecular()).getElement(2)));
	      
	      NdotH = pow(N.dot(&H), theModelToRender->getShininess());
	      
	      NdotL = N.dot(&L);
	      
	      // overall phong equation calcluation
	      temp = ILd * NdotL;
	      I = I + temp;
	      temp = ILs * NdotH;
	      I = I + temp;
	    }

	  // don't allow negative colors
	  if(I.getElement(0) < 0)
		I.setElement(0,0);

	  if(I.getElement(1) < 0)
	    I.setElement(1,0);
	  
	  if(I.getElement(2) < 0)
	    I.setElement(2,0);

	  // set the color and fill the pixel
	  tempColor.setColor(round(I.getElement(0)), round(I.getElement(1)), 
			     round(I.getElement(2)));

	  framebuffer->fillPixel(curX, curY, curZ, tempColor);
	  curX++;
	}

      curY++;
      
      // add remaining edges to active edge list from left to right
      if(edges[1].getDelX() < edges[2].getDelX())
	{
	  active[0] = edges[1];
	  active[1] = edges[2];
	}
      
      else
	{
	  active[0] = edges[2];
	  active[1] = edges[1];
	}
    }

  // no horizontal line on bottom
  else
    {
      // add first two edges from edge list to active
      // edge list from left to right
      if(edges[0].getDelX() < edges[1].getDelX())
	{
	  active[0] = edges[0];
	  active[1] = edges[1];
	}
      
      else
	{
	  active[0] = edges[1];
	  active[1] = edges[0];
	}
    }

  curX = round(active[0].getStart().getElement(0));

  // for each scanline
  while(curY < yMax)
    {
      // vertex event with left edge
      if(curY == active[0].getDelY() + yMin)
	  active[0] = edges[2];

      // vertex event with right edge
      else if(curY == active[1].getDelY() + yMin)
	active[1] = edges[2];

      // determine left and right bounds by looking
      // for edge intersections @ curY
      xLeft = active[0].findIntersection(curY);
      xRight = active[1].findIntersection(curY);
      
      // make sure point is on edge
      if(xLeft == -1 || xRight == -1)
	{
	  //cout << "error, point not found on edge." << endl;
	  curY++;
	  continue;
	}

      // make sure that xLeft is actually left of xRight
      // if not, swap them
      if (xLeft > xRight)
	{
	  tempEdge = active[0];
	  active[0] = active[1];
	  active[1] = tempEdge;
	  xLeft = active[0].findIntersection(curY);
	  xRight = active[1].findIntersection(curY);
	}
	  
      curX = xLeft;

      // determine zLeft and zRight by interpolating along edges
      zLeft = interpolate(active[0].getEnd().getElement(1), 
			  active[0].getStart().getElement(1), curY,
			  active[0].getEnd().getElement(2), active[0].getStart().getElement(2));

      zRight = interpolate(active[1].getEnd().getElement(1), 
			   active[1].getStart().getElement(1), curY,
			   active[1].getEnd().getElement(2), active[1].getStart().getElement(2));

      // get start and end normals and interpolate normals
      // along edges using start and end normals
      start = active[0].getStartNormal();
      end = active[0].getEndNormal();
      normalLeft = interpolateVector(active[0].getEnd().getElement(1), active[0].getStart().getElement(1),
				     curY, end, start);

      start = active[1].getStartNormal();
      end = active[1].getEndNormal();

      normalRight = interpolateVector(active[1].getEnd().getElement(1), active[1].getStart().getElement(1),
				      curY, end, start);

      // if we're at the lowest vertex, fill the pixel there
      if(curY == yMin)
	{
	  framebuffer->fillPixel(curX, curY, zLeft, active[0].getStartColor());
	}
      
      // don't draw the top pixel
      else if (curY == yMax)
	break;
	
      // for entire scanline at curY
      while(curX < xRight)
	{ 
	  // determine z by interpolation between zLeft and zRight
	  curZ = interpolate(xRight, xLeft, curX, zRight, zLeft);

	  // initialize I to ambient light
	  I = (theModelToRender->getAmbient()) * maxColor;
	  
	  // get normal at current point by interpolation of 
	  // left and right normals, make sure it's normalized
	  N = interpolateVector(xRight, xLeft, curX, normalRight, normalLeft);
	  N.norm();

	  // for each light
	  for(unsigned int n = 0; n < numLights; n++)
	    {
	      // L is a directional light as per homework hints on website
	      L = myLights[n].getLocation();
	      L.norm();
	      
	      H = L + V;
	      H.norm();
	      
	      // get diffuse parameters for light[n]
	      ILd.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getDiffuse()).getElement(0)));
	      ILd.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getDiffuse()).getElement(1)));
	      ILd.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getDiffuse()).getElement(2)));
	      
	      // get specular parameters for light[n]
	      ILs.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getSpecular()).getElement(0)));
	      ILs.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getSpecular()).getElement(1)));
	      ILs.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getSpecular()).getElement(2)));
	      
	      NdotH = pow(N.dot(&H), theModelToRender->getShininess());
	      
	      NdotL = N.dot(&L);
	      
	      // overall phong equation calculation
	      temp = ILd * NdotL;
	      I = I + temp;
	      temp = ILs * NdotH;
	      I = I + temp;
	    }

	  // don't allow negative colors
	  if(I.getElement(0) < 0)
		I.setElement(0,0);

	  if(I.getElement(1) < 0)
	    I.setElement(1,0);
	  
	  if(I.getElement(2) < 0)
	    I.setElement(2,0);

	  // set color and fill pixel
	  tempColor.setColor(round(I.getElement(0)), round(I.getElement(1)), 
			     round(I.getElement(2)));

	  framebuffer->fillPixel(curX, curY, curZ, tempColor);
	  curX++;
	}
      curY++;
    }
}

//allocate
//sets up framebuffer
//pushes window coordinate matrix onto stack
void Renderer::allocate(int w, int h)
{
  framebuffer = new Framebuffer(w, h);
  Matrix temp;
  temp.identity();
  temp.setElement(0,0, w/2);
  temp.setElement(0,3, w/2);
  temp.setElement(1,1, h/2);
  temp.setElement(1,3, h/2);
  temp.setElement(2,2, INT_MAX/2);
  temp.setElement(2,3, INT_MAX/2);

  matrixStack.push(temp);
}

//render
//finishes filling matrix stack
//goes through faces array of model and transforms points
//determines vertex colors, face colors, and center points
//calls scanline to fill faces based on shading type
void Renderer::render()
{
  // get camera parameters
  float camNear = myCamera.getNear();
  float camFar = myCamera.getFar();
  float camTop = myCamera.getTop();
  float camBottom = myCamera.getBottom();
  float camLeft = myCamera.getLeft();
  float camRight = myCamera.getRight();

  // NP matrix
  Matrix NP;
  NP.zero();
  NP.setElement(0,0, (2*camNear)/(camRight - camLeft));
  NP.setElement(0,2, (camRight + camLeft)/(camRight - camLeft));
  NP.setElement(1,1, (2*camNear)/(camTop - camBottom));
  NP.setElement(1,2, (camTop + camBottom)/(camTop - camBottom));
  NP.setElement(2,2, -(camFar + camNear)/(camFar - camNear));
  NP.setElement(2,3, -(2*camNear*camFar)/(camFar - camNear));
  NP.setElement(3,2, -1);

  //NP Matrix
  matrixStack.push(NP); 
  
  // camera matrix
  Matrix camMatrix;
  camMatrix.zero();
  float* camOrient = myCamera.getOrientation();
  camMatrix.rotation(camOrient[0], camOrient[1], camOrient[2], camOrient[3]);

  Vector axis;
  
  axis.xAxis();
  Vector u;
  u = camMatrix * axis;

  axis.yAxis();
  Vector v;
  v = camMatrix * axis;

  axis.zAxis();
  Vector n;
  n = camMatrix * axis;

  float* camPosition = myCamera.getPosition();
  Vector r;

  r.setData(camPosition);
  
  camMatrix.identity();

  camMatrix.setElement(0,0, u.getElement(0));
  camMatrix.setElement(0,1, u.getElement(1));
  camMatrix.setElement(0,2, u.getElement(2));
  camMatrix.setElement(0,3, -(r.dot(&u)));

  camMatrix.setElement(1,0, v.getElement(0));
  camMatrix.setElement(1,1, v.getElement(1));
  camMatrix.setElement(1,2, v.getElement(2));
  camMatrix.setElement(1,3, -(r.dot(&v)));
  
  camMatrix.setElement(2,0, n.getElement(0));
  camMatrix.setElement(2,1, n.getElement(1));
  camMatrix.setElement(2,2, n.getElement(2));
  camMatrix.setElement(2,3, -(r.dot(&n)));
  
  matrixStack.push(camMatrix); // camera matrix
  objToCamera.push(camMatrix);
  worldToCamera.push(camMatrix);

  // transformation matricies
  Matrix transform;
  transform.zero();
  for(int k = 0; k < theModelToRender->getNumTransformations(); k++)
    {
      transform = theModelToRender->getTransformation(k);
      matrixStack.push(transform);
      objToCamera.push(transform);
    }

  //for all shadings, transform L's to camera space
  Vector L;
  
  // for each light, transform L to camera space
  for(unsigned int a = 0; a < numLights; a++)
    {
      L = myLights[a].getLocation();
      L = worldToCamera.getTop() * L;
      myLights[a].setLocation(L.getElement(0), L.getElement(1), L.getElement(2));
    }

  Vector A;
  Vector B;
  Vector C;
  Vector P;
  Vector I;
  Vector H;

  // for all shadings, transform points to camera space
  for(int b = 0; b < theModelToRender->getNumPoints(); b++)
    {
      A = theModelToRender->getPoint(b);
      A = objToCamera.getTop() * A;
      theModelToRender->setCameraSpacePoint(b, A);
    }

  // for each face, determine the center point
  for(int c = 0; c < theModelToRender->getNumFaces(); c++)
    {
      A = theModelToRender->getCameraSpacePoint((theModelToRender->getFace(c))[0]);
      B = theModelToRender->getCameraSpacePoint((theModelToRender->getFace(c))[1]);
      C = theModelToRender->getCameraSpacePoint((theModelToRender->getFace(c))[2]);
      
      P = average(A,B,C);
      
      theModelToRender->setCenterPoint(c, P);
    }

  
  Vector N, V, temp;
  Matrix R;
  float neg1 = -1.0;
  float NdotH = 0;
  float NdotL = 0;

  // flat shading, uses face normals
  if(shadingType == SHADE_FLAT)
    {     
      // normals only have to be transformed using the rotation matrix
      R = theModelToRender->getTotalRotations();

      // for each face normal, calculate faceColor
      for(int m = 0; m < theModelToRender->getNumFaceNormals(); m++)
	{
	  N = theModelToRender->getFaceNormal(m); 
	  N = R * N;
	  N.norm();
	  theModelToRender->setFaceNormal(m, N);
	  
	  // V = -P
	  V = (theModelToRender->getCenterPoint(m)) * neg1;
	  V.norm();

	  Vector ILd;
	  Vector ILs;
	  RGBColor tempColor;
	  float maxColor = 255;
	  
	  // initialize I to ambient color
	  I = (theModelToRender->getAmbient()) * maxColor;

	  // for each light, calculate contribution to phong equation
	  for(unsigned int n = 0; n < numLights; n++)
	    {
	      L = myLights[n].getLocation();
	      P = theModelToRender->getCenterPoint(m);
	      L = L - P;
	      L.norm();
	      
	      H = L + V;
	      H.norm();

	      // get diffuse parameters for light[n]
	      ILd.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getDiffuse()).getElement(0)));
	      ILd.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getDiffuse()).getElement(1)));
	      ILd.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getDiffuse()).getElement(2)));

	      // get specular parameters for light[n]
	      ILs.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getSpecular()).getElement(0)));
	      ILs.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getSpecular()).getElement(1)));
	      ILs.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getSpecular()).getElement(2)));

	      NdotH = pow(N.dot(&H), theModelToRender->getShininess());
	      NdotL = N.dot(&L);
	      
	      // phong equation calculation
	      temp = ILd * NdotL;
	      I = I + temp;
	      temp = ILs * NdotH;
	      I = I + temp;

	      // don't allow negative colors
	      if(I.getElement(0) < 0)
		I.setElement(0,0);

	      if(I.getElement(1) < 0)
		I.setElement(1,0);

	      if(I.getElement(2) < 0)
		I.setElement(2,0);

	      // set color and assign to face color
	      tempColor.setColor(round(I.getElement(0)), round(I.getElement(1)), 
				 round(I.getElement(2)));
	      
	      theModelToRender->setFaceColor(m, tempColor);
	    }
	}

      
    }

  // gouraud shading, uses vertex colors/normals
  if(shadingType == SHADE_GOURAUD)
    {
      // normals only transformed by rotation matrix
      R = theModelToRender->getTotalRotations();
      
      // for each vertex, calculate the color
      for(int m = 0; m < theModelToRender->getNumPoints(); m++)
	{
	  // transform vertex normal
	  N = theModelToRender->getVertexNormal(m); 
	  N = R * N;
	  N.norm();
	  theModelToRender->setVertexNormal(m, N);
	  
	  // V = -P
	  V = (theModelToRender->getCameraSpacePoint(m)) * neg1;
	  V.norm();
	  
	  Vector ILd;
	  Vector ILs;
	  RGBColor tempColor;
	  float maxColor = 255;
	  
	  // initialize I to ambient color
	  I = (theModelToRender->getAmbient()) * maxColor;
	  
	  // for each light, calculate contribution to phong equation
	  for(unsigned int n = 0; n < numLights; n++)
	    {
	      L = myLights[n].getLocation();
	      P = theModelToRender->getPoint(m);
	      L = L - P;
	      L.norm();
	      
	      H = L + V;
	      H.norm();
	      
	      // get diffuse parameters for light[n]
	      ILd.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getDiffuse()).getElement(0)));
	      ILd.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getDiffuse()).getElement(1)));
	      ILd.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getDiffuse()).getElement(2)));
	      
	      // get specular parameters for light[n]
	      ILs.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getSpecular()).getElement(0)));
	      ILs.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getSpecular()).getElement(1)));
	      ILs.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getSpecular()).getElement(2)));
	      
	      NdotH = pow(N.dot(&H), theModelToRender->getShininess());
	      NdotL = N.dot(&L);
	      
	      // phong equation calculation
	      temp = ILd * NdotL;
	      I = I + temp;
	      temp = ILs * NdotH;
	      I = I + temp;
	      
	      // don't allow negative colors
	      if(I.getElement(0) < 0)
		I.setElement(0,0);
	      
	      if(I.getElement(1) < 0)
		I.setElement(1,0);
	      
	      if(I.getElement(2) < 0)
		I.setElement(2,0);
	      
	      // set color and assign vertexColor
	      tempColor.setColor(round(I.getElement(0)), round(I.getElement(1)), 
				 round(I.getElement(2)));
	      
	      theModelToRender->setVertexColor(m, tempColor);
	    }
	}
    }
   
  // transform to window coordinates
  Matrix W;
  W.identity();
  W.setElement(0,0, (framebuffer->getWidth())/2);
  W.setElement(0,3, (framebuffer->getWidth())/2);
  W.setElement(1,1, (framebuffer->getHeight())/2);
  W.setElement(1,3, (framebuffer->getHeight())/2);
  W.setElement(2,2, INT_MAX/2);
  W.setElement(2,3, INT_MAX/2);
  
  // transform each point
  for(int i = 0; i < theModelToRender->getNumPoints(); i++)
    {
      A = theModelToRender->getPoint(i);
      A = matrixStack.getTop() * A;
      A.homogenize();
      theModelToRender->setWindowSpacePoint(i, A);
    }
  
  // phong shading
  // uses normal interpolation
  // need to "fudge" camera space normals using window space points
  if(shadingType == SHADE_PHONG)
    {
      float gz = 0;
      float den = 0;
      Vector Np;

      // normals only transformed by rotation matrix
      R = theModelToRender->getTotalRotations();
      
      // for each vertex, calculate the color
      for(int m = 0; m < theModelToRender->getNumPoints(); m++)
	{
	  // transform vertex normal
	  N = theModelToRender->getVertexNormal(m); 
	  N = R * N;
	  N.norm();
	  theModelToRender->setVertexNormal(m, N);
	  
	  // V = -P
	  V = (theModelToRender->getCameraSpacePoint(m)) * neg1;
	  V.norm();
	  
	  Vector ILd;
	  Vector ILs;
	  RGBColor tempColor;
	  float maxColor = 255;
	  
	  // initialize I to ambient color
	  I = (theModelToRender->getAmbient()) * maxColor;
	  
	  // for each light, calculate contribution to phong equation
	  for(unsigned int n = 0; n < numLights; n++)
	    {
	      L = myLights[n].getLocation();
	      P = theModelToRender->getPoint(m);
	      L = L - P;
	      L.norm();
	      
	      H = L + V;
	      H.norm();
	      
	      // get diffuse parameters for light[n]
	      ILd.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getDiffuse()).getElement(0)));
	      ILd.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getDiffuse()).getElement(1)));
	      ILd.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getDiffuse()).getElement(2)));
	      
	      // get specular parameters for light[n]
	      ILs.setElement(0, (myLights[n].getColor().getR()) * 
			     ((theModelToRender->getSpecular()).getElement(0)));
	      ILs.setElement(1, (myLights[n].getColor().getG()) * 
			     ((theModelToRender->getSpecular()).getElement(1)));
	      ILs.setElement(2, (myLights[n].getColor().getB()) * 
			     ((theModelToRender->getSpecular()).getElement(2)));
	      
	      NdotH = pow(N.dot(&H), theModelToRender->getShininess());
	      NdotL = N.dot(&L);
	      
	      // phong equation calculation
	      temp = ILd * NdotL;
	      I = I + temp;
	      temp = ILs * NdotH;
	      I = I + temp;
	      
	      // don't allow negative colors
	      if(I.getElement(0) < 0)
		I.setElement(0,0);
	      
	      if(I.getElement(1) < 0)
		I.setElement(1,0);
	      
	      if(I.getElement(2) < 0)
		I.setElement(2,0);
	      
	      // set color and assign vertexColor
	      tempColor.setColor(round(I.getElement(0)), round(I.getElement(1)), 
				 round(I.getElement(2)));
	      
	      theModelToRender->setVertexColor(m, tempColor);
	    }
	}

      // for each vertex normal, convert to camera space
      for(int m = 0; m < theModelToRender->getNumPoints(); m++)
	{
	  // transform vertex normal
	  N = theModelToRender->getVertexNormal(m); 
	  N = R * N;
	  theModelToRender->setVertexNormal(m, N);
	}
      
      // fudge each of the vertex normals
      // N' = N/(Gz + 1)
      // Gz = Pz/(INT_MAX - Pz)
      for(int i = 0; i < theModelToRender->getNumPoints(); i++)
	{
	  gz = theModelToRender->getWindowSpacePoint(i).getElement(2)/
	    (INT_MAX - theModelToRender->getWindowSpacePoint(i).getElement(2));
	  den = (1/(gz + 1));
	  Np = theModelToRender->getVertexNormal(i) * den;
	  Np.norm();
	  theModelToRender->setVertexNormal(i, Np);
	}
    }
    
  // scanline/zBuffer
  for(int j = 0; j < theModelToRender->getNumFaces(); j++)
    scanline(j); // change back to j
}

//dump
//writes rendered framebuffer to file
void Renderer::dump()
{
  framebuffer->writeOut();
}

//parse
//reads iv file
//populates appropriate matricies
//uses nested switches inside pairs of curly braces in the file
//assumes that file is in a similar format to the examples
void Renderer::parse(char* filename)
{
  ifstream inFile(filename, ios::in);
  if(!inFile)
    {
      cout << "Error, cannot open file: " << filename << " ...Aborting."
	   << endl;
      exit(-1);
    }

  string buffer = "";
  char* c_buffer = new char[100];
  char* garbage = new char[100];
  char* garbage2 = new char[100];
  int tok = -1;
  int tok2 = -1;
  int tok3 = -1;
  int tok4 = -1;
  int tok5 = -1;
  int tok6 = -1;
  int tok7 = -1;
  unsigned int r, g, b;
  bool flag = false;
  float x, y, z, theta;
  Matrix tempMatrix;
  float fr, fg, fb, fs;
  Vector tempVector, tempVector2, faceVector, v1, v2, v3;

  getline(inFile, buffer, '\n'); //get first line of file
  
  // while not EOF or last }
  while(!flag)
    {
      // tokenize first line of file
      sscanf(buffer.c_str(), "%s", c_buffer);
      tok = tokenize(c_buffer);

      switch(tok)
	{
	case CAMERA:
	  // while not end of camera block
	  while(strcmp(c_buffer, "}"))
	    {
	      // get camera parameters
	      getline(inFile, buffer, '\n');
	      sscanf(buffer.c_str(), "%s", c_buffer);
	      tok2 = tokenize(c_buffer);

	      // figure out which camera parameter we got
	      switch(tok2)
		{
		case POSITION:
		  sscanf(buffer.c_str(), "%s %f %f %f", garbage, &x, &y, &z);
		  myCamera.setPosition(x,y,z);
		  break;
		  
		case ORIENTATION:
		  sscanf(buffer.c_str(), "%s %f %f %f %f", garbage, 
			 &x, &y, &z, &theta);
		  myCamera.setOrientation(x,y,z,theta);
		  break;
		  
		case NEAR:
		  sscanf(buffer.c_str(), "%s %f", garbage, &x);
		  myCamera.setNear(x);
		  break;
		  
		case FAR:
		  sscanf(buffer.c_str(), "%s %f", garbage, &x);
		  myCamera.setFar(x);
		  break;
		  
		case LEFT:
		  sscanf(buffer.c_str(), "%s %f", garbage, &x);
		  myCamera.setLeft(x);
		  break;
		  
		case RIGHT:
		  sscanf(buffer.c_str(), "%s %f", garbage, &x);
		  myCamera.setRight(x);
		  break;
		  
		case TOP:
		  sscanf(buffer.c_str(), "%s %f", garbage, &x);
		  myCamera.setTop(x);
		  break;
		  
		case BOTTOM:
		  sscanf(buffer.c_str(), "%s %f", garbage, &x);
		  myCamera.setBottom(x);
		  break;
		  
		default:
		  break;
		}//switch CAMERA
	    }//while != }
	  break;
	  
	case POINTLIGHT:
	  // get POINTLIGHT parameters
	  getline(inFile, buffer, '\n');
	  
	  // wait for last } or more lights than MAXLIGHTS
	  while(strcmp(c_buffer, "}") && numLights < MAXLIGHTS)
	    {
	      // decide which light parameter we got
	      sscanf(buffer.c_str(), "%s", c_buffer);
	      tok5 = tokenize(c_buffer);

	      switch(tok5)
		{
		case LOCATION:
		  sscanf(buffer.c_str(), "%s %f %f %f", garbage, &x, &y, &z);
		  myLights[numLights].setLocation(x, y, z);
		  break;

		case COLOR:
		  sscanf(buffer.c_str(), "%s %f %f %f", garbage, &fr, &fg, &fb);
		  
		  RGBColor c;
		  r = (unsigned int)round(fr*255);
		  g = (unsigned int)round(fg*255);
		  b = (unsigned int)round(fb*255);

		  c.setColor(r,g,b);
		  myLights[numLights].setColor(c);
		  break;
		} // switch POINTLIGHT
	      
	      getline(inFile, buffer, '\n');
	      sscanf(buffer.c_str(), "%s", c_buffer);
	    } // while(not })

	  numLights++;
	  break;

	case SEPARATOR:
	  // SEPARATOR is the start of the model
	  int p1, p2, p3, neg1;
	  getline(inFile, buffer, '\n');

	  // while we don't hit the closing }
	  while(strcmp(c_buffer, "}"))
	    {
	      // figure out which parameter we got
	      sscanf(buffer.c_str(), "%s", c_buffer);
	      tok3 = tokenize(c_buffer);
	      
	      switch(tok3)
		{	
		case TRANSFORM:		
		  // get translation, rotation, and scaleFactor
		  getline(inFile, buffer, '\n');
		  
		  // while we don't hit the closing }
		  while(strcmp(c_buffer, "}"))
		    {
		      sscanf(buffer.c_str(), "%s", c_buffer);
		      tok4 = tokenize(c_buffer);
		      switch(tok4)
			{
			case TRANSLATION:
			  sscanf(buffer.c_str(), "%s %f %f %f", garbage, 
				 &x, &y, &z);
			  theModelToRender->addTranslation(x,y,z);
			  break;
			  
			case ROTATION:
			  sscanf(buffer.c_str(), "%s %f %f %f %f", garbage, 
				 &x, &y, &z, &theta);
			  theModelToRender->addRotation(x,y,z,theta);
			  break;

			case SCALEFACTOR:
			  sscanf(buffer.c_str(), "%s %f %f %f", garbage, 
				 &x, &y, &z);
			  theModelToRender->addScale(x,y,z);
			  break;

			default:
			  break;
			} //switch(transform)

		      getline(inFile, buffer, '\n');
		      sscanf(buffer.c_str(), "%s", c_buffer);
		    }// while TRANSFORM

		  getline(inFile, buffer, '\n');
		  getline(inFile, buffer, '\n');
		  sscanf(buffer.c_str(), "%s", c_buffer);

		  theModelToRender->makeTransformation();
		  break;
		  
		case MATERIAL:
		  // MATERIAL determines light properties of object
		  getline(inFile, buffer, '\n');
		  
		  // while we don't hit closing }
		  while(strcmp(c_buffer, "}"))
		    {
		      sscanf(buffer.c_str(), "%s", c_buffer);
		      tok6 = tokenize(c_buffer);
		      
		      // get parameters for material
		      switch(tok6)
			{
			case AMBIENTCOLOR:
			  sscanf(buffer.c_str(), "%s %f %f %f", garbage, &fr, &fg, &fb);
			  tempVector.zero();
			  tempVector.setElement(0, fr);
			  tempVector.setElement(1, fg);
			  tempVector.setElement(2, fb);
			  theModelToRender->setAmbient(tempVector);
			  break;

			case DIFFUSECOLOR:
			  sscanf(buffer.c_str(), "%s %f %f %f", garbage, 
				 &fr, &fg, &fb);
			  tempVector.zero();
			  tempVector.setElement(0, fr);
			  tempVector.setElement(1, fg);
			  tempVector.setElement(2, fb);
			  theModelToRender->setDiffuse(tempVector);
			  break;

			case SPECULARCOLOR:
			  sscanf(buffer.c_str(), "%s %f %f %f", garbage, 
				 &fr, &fg, &fb);
			  tempVector.zero();
			  tempVector.setElement(0, fr);
			  tempVector.setElement(1, fg);
			  tempVector.setElement(2, fb);
			  theModelToRender->setSpecular(tempVector);
			  break;
			  
			case SHININESS:
			  sscanf(buffer.c_str(), "%s %f", garbage, &fs);
			  theModelToRender->setShininess(fs);
			  break;
			} // switch(MATERIAL)

		      getline(inFile, buffer, '\n');
		      sscanf(buffer.c_str(), "%s", c_buffer);
		    } // while( not } )
	   
		  getline(inFile, buffer, '\n');
		  sscanf(buffer.c_str(), "%s", c_buffer);
		break;
		  
		case COORDINATE3:
		  // COORDINATE3 describes an object using points and faces, as well as vertex normals
		  
		  getline(inFile, buffer, '\n'); //get first point
		  sscanf(buffer.c_str(), "%s", c_buffer);
		  
		  // while not at closing }
		  while(strcmp(c_buffer, "}"))
		    {
		      tok7 = tokenize(c_buffer);
		      
		      switch(tok7)
			{
			case POINT:
			  // POINT begins the list of points, ends w/ ]
			  sscanf(buffer.c_str(), "%s %s %f %f %f", garbage, garbage2, 
				 &x, &y, &z);
			  
			  theModelToRender->addPoint(x,y,z);
		      
			  getline(inFile, buffer, '\n');
			  
			  while(strcmp(c_buffer, "]")) //get rest of points
			    {
			      sscanf(buffer.c_str(), "%f %f %f", &x, &y, &z);
			      theModelToRender->addPoint(x,y,z);
			      getline(inFile, buffer, '\n');
			      sscanf(buffer.c_str(), "%s", c_buffer);
			    }
			  getline(inFile, buffer, '\n');
			  sscanf(buffer.c_str(), "%s", c_buffer);
			  break;
			  
			case NORMAL:
			  // NORMAL begins the list of normals, ends with ]
			  getline(inFile, buffer, '\n');
			  sscanf(buffer.c_str(), "%s %s %f %f %f", garbage, garbage2, &x, &y, &z);

			  tempVector.zero();
			  tempVector.setElement(0,x);
			  tempVector.setElement(1,y);
			  tempVector.setElement(2,z);
			  theModelToRender->addVertexNormal(tempVector);

			  getline(inFile, buffer, '\n');
			  sscanf(buffer.c_str(), "%s", c_buffer);
			  
			  while(strcmp(c_buffer, "}"))
			    {
			      while(strcmp(c_buffer, "]"))
				{
				  sscanf(buffer.c_str(), "%f %f %f", &x, &y, &z);
				  tempVector.zero();
				  tempVector.setElement(0,x);
				  tempVector.setElement(1,y);
				  tempVector.setElement(2,z);
				  theModelToRender->addVertexNormal(tempVector);
				  
				  getline(inFile, buffer, '\n');
				  sscanf(buffer.c_str(), "%s", c_buffer);
				} // while( not ] )
			      
			      getline(inFile, buffer, '\n');
			      sscanf(buffer.c_str(), "%s", c_buffer);
			    
			    } // while ( not } )
			 
			  getline(inFile, buffer, '\n');
			  sscanf(buffer.c_str(), "%s", c_buffer);
			  break;
			  
		        default:
			  break;
			} // switch (COORD3)
		      
		      sscanf(buffer.c_str(), "%s", c_buffer);
		    
		    } //while (not })
		  
		  getline(inFile, buffer, '\n');
		  sscanf(buffer.c_str(), "%s", c_buffer);
		  break;
		  
		case INDEXEDFACESET:
		  // INDEXEDFACESET describes faces by their point index
		  getline(inFile, buffer, '\n'); //get first face
		  sscanf(buffer.c_str(), "%s %s %d %d %d %d", garbage, garbage2, &p1, &p2, &p3, &neg1);
		  theModelToRender->addFace(p1, p2, p3);

		  //add face normal (p2 - p1) x (p3 - p2)
		  v1 = theModelToRender->getPoint(p1);
		  v2 = theModelToRender->getPoint(p2);
		  v3 = theModelToRender->getPoint(p3);
		  
		  tempVector = v2 - v1;
		  tempVector2 = v3 - v2;
		  
		  faceVector = tempVector.cross(&tempVector2);
		  
		  theModelToRender->addFaceNormal(faceVector);
		  
		  getline(inFile, buffer, '\n');

		  while(strcmp(c_buffer, "]")) //get rest of faces
		    {
		      sscanf(buffer.c_str(), "%d %d %d %d", 
			     &p1, &p2, &p3, &neg1);
		      theModelToRender->addFace(p1, p2, p3);

		      //add face normal (p2 - p1) x (p3 - p2)
		      v1 = theModelToRender->getPoint(p1);
		      v2 = theModelToRender->getPoint(p2);
		      v3 = theModelToRender->getPoint(p3);
		      
		      tempVector = v2 - v1;
		      tempVector2 = v3 - v2;
		      
		      faceVector = tempVector.cross(&tempVector2);

		      faceVector.norm();
		      theModelToRender->addFaceNormal(faceVector);

		      getline(inFile, buffer, '\n');
		      sscanf(buffer.c_str(), "%s", c_buffer);
		    } // while( not ] )

		  getline(inFile, buffer, '\n');
		  sscanf(buffer.c_str(), "%s", c_buffer);
		  break;
		  
		default:
		  break;
		} // switch (SEPARATOR)
	      
	      sscanf(buffer.c_str(), "%s", c_buffer);
	      
	    } // while SEPARATOR
	  break;
	  
	case COMMENT:
	  // do nothing for comments
	  break;
	  
	default:
	  break;
	}

      // make sure next line exists
      getline(inFile, buffer, '\n');
      if(inFile.eof())
	{
	  flag = true;
	}
    }
}

//tokenize
//returns an integer representation of file keywords
//returns -1 if keyword not found
int tokenize(char* token)
{
  //only do comparisons on lowercase
  for(unsigned int i = 0; i < strlen(token); i++)
    {
      token[i] = tolower(token[i]);
    }
  
  if(token[0] == '#')
    return COMMENT;
  
  if(!strcmp(token, "perspectivecamera"))
    return CAMERA;
  
  if(!strcmp(token, "separator"))
    return SEPARATOR;
  
  if(!strcmp(token, "coordinate3"))
     return COORDINATE3;
    
  if(!strcmp(token, "point"))
    return POINT;

  if(!strcmp(token, "indexedfaceset"))
    return INDEXEDFACESET;
  
  if(!strcmp(token, "position"))
    return POSITION;
  
  if(!strcmp(token, "orientation"))
    return ORIENTATION;
  
  if(!strcmp(token, "neardistance"))
    return NEAR;
  
  if(!strcmp(token, "fardistance"))
    return FAR;
  
  if(!strcmp(token, "left"))
    return LEFT;
  
  if(!strcmp(token, "right"))
    return RIGHT;
  
  if(!strcmp(token, "top"))
    return TOP;
  
  if(!strcmp(token, "bottom"))
    return BOTTOM;

  if(!strcmp(token, "transform"))
    return TRANSFORM;

  if(!strcmp(token, "translation"))
    return TRANSLATION;

  if(!strcmp(token, "rotation"))
    return ROTATION;

  if(!strcmp(token, "scalefactor"))
    return SCALEFACTOR;

  if(!strcmp(token, "pointlight"))
    return POINTLIGHT;

  if(!strcmp(token, "location"))
    return LOCATION;

  if(!strcmp(token, "color"))
    return COLOR;

  if(!strcmp(token, "material"))
    return MATERIAL;

  if(!strcmp(token, "ambientcolor"))
    return AMBIENTCOLOR;

  if(!strcmp(token, "diffusecolor"))
    return DIFFUSECOLOR;

  if(!strcmp(token, "specularcolor"))
    return SPECULARCOLOR;

  if(!strcmp(token, "shininess"))
    return SHININESS;

  if(!strcmp(token, "normal"))
    return NORMAL;

  if(!strcmp(token, "vector"))
    return VECTOR;
  
  return -1; //no related keyword
}


