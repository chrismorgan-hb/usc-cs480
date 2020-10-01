// framebuffer.cc
// class implementation for Framebuffer
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "framebuffer.h"
#include "utility.h"
#include <iostream>
#include <fstream>
using namespace std;

//constructor
//framebuffer must be created with width and height
Framebuffer::Framebuffer(int w, int h)
{
  width = w;
  height = h;
  buffer = new int* [w];
  for(int k = 0; k < w; k++)
    buffer[k] = new int[h];
  
  //set all pixels to black
  for(int i = 0; i < w; i++)
    for(int j = 0; j < h; j++)
      buffer[i][j] = 0;
}

Framebuffer::~Framebuffer()
{
  for(int i = 0; i < width; i++)
    delete buffer[i];

  delete [] buffer;
}

void Framebuffer::drawLine(int x0, int y0, int x1, int y1)
{
  int octant = findOctant(x0, y0, x1, y1);
  
  switch(octant)
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

void Framebuffer::setPixel(int x, int y)
{
  if(x < 0 || x > width)
    return;
  else if(y < 0 || y > height)
    return;
  else
    buffer[x][y] = 255;
}

void Framebuffer::writeOut()
{
  // write the framebuffer to a file <image.ppm>
  // remember to write each pixel's R, G, & B values as 0 0 0 or 255 255 225
  
  ofstream outFile("image.ppm", ios::out);
  if(!outFile)
    {
      cout << "Error, could not create output file... aborting." << endl;
      exit(-1);
    }

  outFile << "P6" << endl << width << " " << height << endl << "255" << endl;

  for(int i = 0; i < height; i++)
    {
      for(int j = 0; j < width; j++)
	{
	  for(int k = 0; k < 3; k++)
	    {
	      //cast to char for P6
	      outFile << (char)(buffer[j][height - i - 1]);
	    }
	}
    }
}

//line1
//draw line in OCT1
void Framebuffer::line1(int x0, int y0, int x1, int y1)
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
void Framebuffer::line2(int x0, int y0, int x1, int y1)
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
void Framebuffer::line3(int x0, int y0, int x1, int y1)
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
void Framebuffer::line4(int x0, int y0, int x1, int y1)
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
void Framebuffer::line5(int x0, int y0, int x1, int y1)
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
void Framebuffer::line6(int x0, int y0, int x1, int y1)
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
void Framebuffer::line7(int x0, int y0, int x1, int y1)
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
void Framebuffer::line8(int x0, int y0, int x1, int y1)
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
void Framebuffer::lineh(int x0, int y0, int x1, int y1)
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
void Framebuffer::linev(int x0, int y0, int x1, int y1)
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



