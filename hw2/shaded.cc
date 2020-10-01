// shaded.cc
// main file for hw2
// CSCI 480 Sp2004
// Author: Christopher Morgan

#include <iostream>
using namespace std;
#include "framebuffer.h"
#include "matrix.h"
#include "vector.h"
#include "utility.h"
#include "globals.h"
#include "stack.h"
#include "renderer.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int width = 0;
  int height = 0;
  char* filename = "";
  char* shadingType = "";
  int s = -1;

  // check that there is shadingType, width, height, and filename
  if(argc < 5)
    {
      cout << "Error, too few parameters." << endl;
      cout << "USAGE: shaded <flat|gouraud|phong> <xRes> <yRes> <iv-file>" << endl;
      return -1;
    }

  // make sure there is a valid shading type
  else
    {
      shadingType = argv[1];
      width = atoi(argv[2]);
      height = atoi(argv[3]);
      filename = argv[4];
      
      if(!strcmp(shadingType, "flat"))
	s = SHADE_FLAT;

      else if(!strcmp(shadingType, "gouraud"))
	s = SHADE_GOURAUD;
      
      else if(!strcmp(shadingType, "phong"))
	s = SHADE_PHONG;

      else
	{
	  cout << "Error, invalid shading type." << endl;
	  cout << "Please specify flat, gouraud, or phong." << endl;
	  return -1;
	}
      
      // render the file
      Renderer myRenderer(s);
      myRenderer.allocate(width, height);
      myRenderer.parse(filename);
      myRenderer.render();
      myRenderer.dump();
    }

  return 0;
}
