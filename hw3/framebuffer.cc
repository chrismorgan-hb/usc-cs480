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
}

int Framebuffer::getWidth()
{
  return width;
}

int Framebuffer::getHeight()
{
  return height;
}

void Framebuffer::setWidth(int w)
{
  width = w;
}

void Framebuffer::setHeight(int h)
{
  height = h;
}

