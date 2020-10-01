// OglRenderer.cc
// Main file for HW3
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include <GL/glut.h>
#include <iostream>
using namespace std;
#include "renderer.h"
#include "matrix.h"
#include <stdlib.h>
#include <math.h>
#include "globals.h"

// global variables for callback functions
Renderer myRenderer("Checkerboard.ppm");
Matrix transformer;
int startX, startY;
bool lButtonDown = false;
int keyState = 0;
int myState = 0;
int motionCounter = 0;

// myDisplay
// display callback, called when DISPLAY message is posted
void myDisplay(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  myRenderer.render();
  glutSwapBuffers();
  glFlush();
}

// myReshape
// reshape callback, called when window is reshaped
void myReshape(int w, int h)
{
  glViewport(0, 0, w, h);
  myRenderer.setWidth(w);
  myRenderer.setHeight(h);

  myRenderer.render();
}

// myIdle
// idle callback
// called periodically when nothing else is happening
void myIdle(void)
{
  glutPostRedisplay();
}

// myKeyboard
// keyboard callback
// has functionality for panning, rotating, and zooming, as well as resetting the view, and changing the display type
void myKeyboard(unsigned char key, int x, int y)
{
  Matrix temp;
  Camera* theCam = myRenderer.getCamera();
  switch(key)
    {
    case 'w':
      if(myRenderer.getDisplayType() == WIREFRAME)
	myRenderer.setDisplayType(SHADED);

      else if(myRenderer.getDisplayType() == SHADED)
	myRenderer.setDisplayType(TEXTURE);

      else if(myRenderer.getDisplayType() == TEXTURE)
	myRenderer.setDisplayType(SHADED_TEXTURE);

      else if(myRenderer.getDisplayType() == SHADED_TEXTURE)
	myRenderer.setDisplayType(WIREFRAME);
      break;

    case 'r': // pan right
      temp.translation(1, 0, 0);
      transformer = temp * transformer;
      break;

    case 'l': // pan left
      temp.translation(-1, 0, 0);
      transformer = temp * transformer;
      break;

    case 'u': // pan up
      temp.translation(0, 1, 0);
      transformer = temp * transformer;
      break;

    case 'd': // pan down
      temp.translation(0, -1, 0);
      transformer = temp * transformer;
      break;

    case '+': // zoom in
      temp.translation(0, 0, 1);
      transformer = temp * transformer;
      break;

    case '-': // zoom out
      temp.translation(0, 0, -1);
      transformer = temp * transformer;
      break;

    case 'p': // rotate right
      temp.rotation(0, -1.0, theCam->getPosition()[2] - 3, 1.0);
      transformer = temp * transformer;
      break;
      
    case 'q': // rotate left
      temp.rotation(0, -1.0, theCam->getPosition()[2] - 3, -1.0);
      transformer = temp * transformer;
      break;
 
    case 'R': // reset
      transformer.identity();
      break;
    }
  glutPostRedisplay();
}

// myMouse
// mouse callback function
// called whenever a mouse button is clicked or unclicked
// sets a start position used to determine the length of the drag
void myMouse(int button, int state, int x, int y)
{
  if(button == GLUT_LEFT_BUTTON)
    {
      lButtonDown = (state == GLUT_DOWN) ? true : false;
      keyState = glutGetModifiers();
      if(lButtonDown && keyState == GLUT_ACTIVE_SHIFT) // zooming
	{
	  myState = ZOOM;
	  startX = x;
	  startY = y;
	}

      else if(lButtonDown && keyState == GLUT_ACTIVE_CTRL) // translating
	{
	  myState = TRANSLATE;
	  startX = x;
	  startY = y;
	}

      else if(lButtonDown) // rotating
	{
	  myState = ROTATE;
	  startX = x;
	  startY = y;
	}

      else // button up
	{
	  myState = 0;
	  startX = 0;
	  startY = 0;
	}
    }
}

// myMotion
// mouse motion callback function
// called when dragging happens
// used to translate, rotate, and zoom
void myMotion(int x, int y)
{
  Matrix temp;
  int delX = x - startX;
  int delY = y - startY;
  float d = sqrt(delX*delX + delY*delY);
  float z = myRenderer.getCamera()->getPosition()[2];
  
  // if actively dragging
  if(lButtonDown)
    {
      motionCounter++;
      // only refresh every 3 times it's called
      if(motionCounter > 3)
	{
	  if(myState == ZOOM)
	    {
	      if(delY < 0)
		temp.translation(0, 0, d/5.0);
	      else
		temp.translation(0, 0, -d/5.0);

	      transformer = temp * transformer;
	      startX = x;
	      startY = y;
	      glutPostRedisplay();
	    }

	  else if(myState == TRANSLATE)
	    {
	      temp.translation(delX/2.0, -delY/2.0, 0);
	      transformer = temp * transformer;
	      startX = x;
	      startY = y;
	      glutPostRedisplay();
	    }

	  else if(myState == ROTATE)
	    {
	      temp.identity();
	      if(delX > 0)
		temp.rotation(-delY, delX, z-3, -1);
	      else
		temp.rotation(-delY, delX, z-3, 1);

	      transformer = temp * transformer;
	      glutPostRedisplay();
	    }
	      
	  motionCounter = 0;
	}
    }
}

// main
// main function for OglRenderer
// initializes openGL and GLUT stuff, parses command line and associated files
int main(int argc, char* argv[])
{
  int width = 0;
  int height = 0;
  char* filename = "";
  char* texFile = "";
  transformer.identity();

  // check that there is width, height, and filename, texture file optional
  if(argc < 4)
    {
      cout << "Error, too few parameters." << endl;
      cout << "USAGE: OglRenderer <xRes> <yRes> <iv-file> [<texture-file>]" << endl;
      return -1;
    }

  else
    {
      width = atoi(argv[1]);
      height = atoi(argv[2]);
      filename = argv[3];
      if(argc == 5)
	texFile = argv[4];
      else
	texFile = "Checkerboard.ppm";

      myRenderer.setTexFile(texFile);

      unsigned int texName;

      myRenderer.parse(filename);
      myRenderer.allocate(width, height);

      glutInit(&argc, argv);
      glutInitWindowSize(width, height);
      glutInitWindowPosition(0,0);
      glutCreateWindow("OglRenderer");
      glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

      myRenderer.parseTex();
      
      // generate/bind texture obj
      glGenTextures(1, &texName);
      glBindTexture(GL_TEXTURE_2D, texName);
      
      // only build once... super-slow
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, myRenderer.getTexWidth(), myRenderer.getTexHeight(), GL_RGB, GL_UNSIGNED_BYTE, myRenderer.getTexArr());

      // register callbacks
      glutDisplayFunc(myDisplay);
      glutReshapeFunc(myReshape);
      glutIdleFunc(NULL);
      glutKeyboardFunc(myKeyboard);
      glutMouseFunc(myMouse);
      glutMotionFunc(myMotion);

      glClearDepth(1.0);
      glClearColor(0, 0, 0, 1);

      // no return from here
      glutMainLoop();
    }

  return 0;
}
