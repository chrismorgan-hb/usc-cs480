// wireframe.cc
// main file for hw1
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
  Renderer myRenderer;

  // check that there is width, height, and filename
  if(argc < 4)
    {
      cout << "Error, too few parameters." << endl;
      cout << "USAGE: wireframe <xRes> <yRes> <iv-file>" << endl;
      return -1;
    }

  else
    {
      width = atoi(argv[1]);
      height = atoi(argv[2]);
      filename = argv[3];
      
      myRenderer.allocate(width, height);
      myRenderer.parse(filename);
      myRenderer.render();
      myRenderer.dump();
    }

  return 0;
}
