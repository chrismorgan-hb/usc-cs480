// framebuffer.cc
// class implementation for Framebuffer
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "framebuffer.h"
#include "utility.h"
#include <iostream>
#include <fstream>
using namespace std;
#include <limits.h>

//constructor
//framebuffer must be created with width and height
Framebuffer::Framebuffer(int w, int h)
{
  width = w;
  height = h;
  buffer = new int* [w];
  colors = new RGBColor* [w];
  zBuffer = new float* [w];
  for(int k = 0; k < w; k++)
    {
      buffer[k] = new int[h];
      colors[k] = new RGBColor[h];
      zBuffer[k] = new float[h];
    }
  
  //set all pixels to black initially
  RGBColor black(0,0,0);
  for(int i = 0; i < w; i++)
    for(int j = 0; j < h; j++)
      {
	buffer[i][j] = 0;
	colors[i][j] = black;
	zBuffer[i][j] = -1;
      }
}

Framebuffer::~Framebuffer()
{
  for(int i = 0; i < width; i++)
    delete buffer[i];

  delete [] buffer;
}

int Framebuffer::getWidth()
{
  return width;
}

int Framebuffer::getHeight()
{
  return height;
}

//fillPixel
//uses zBuffering to determine if pixel is on top
//if so, fills with color c
//otherwise, does nothing
void Framebuffer::fillPixel(int x, int y, float z, RGBColor c)
{
  if(x < 0 || x > width)
    return;
  else if(y < 0 || y > height)
    return;
  else
    {
      if(z < zBuffer[x][y] || zBuffer[x][y] == -1)
      {
	colors[x][y] = c;
	zBuffer[x][y] = z;
      } 
    }
}

// writeOut
// write the framebuffer to a file <image.ppm>
void Framebuffer::writeOut()
{
  ofstream outFile("image.ppm", ios::out);
  
  // make sure the file opened
  if(!outFile)
    {
      cout << "Error, could not create output file... aborting." << endl;
      exit(-1);
    }

  // header information
  outFile << "P6" << endl << width << " " << height << endl << "255" << endl;

  char r, g, b;
  for(int i = 0; i < height; i++)
    {
      for(int j = 0; j < width; j++)
	{
	  // get rgb values from colors array and
	  // cast to char for P6
	  r = (char)(colors[j][height - i - 1]).getR();
	  g = (char)(colors[j][height - i - 1]).getG();
	  b = (char)(colors[j][height - i - 1]).getB();
	  
	  // write rgb to file
	  outFile << r;
	  outFile << g;
	  outFile << b;
	}
    }
}
