// SolarSystem.cc
// main file for project
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#include <stdio.h>
#include "globals.h"
#include "planet.h"
#include "matrix.h"
#include "vector.h"

Matrix transformer;
Planet planets[NUMPLANETS];

// animation parameters
float t = 0;
int s = 1;
float ms = 0;
bool animate = true;

// user interaction
int myState = 0;
int startX = 0; 
int startY = 0; 
int keyState = 0;
int motionCounter = 0;
bool lButtonDown = false;

// camera chase
Vector cameraLoc;
Vector cameraOrient;
Vector cameraUp;
int camState;
int chasedPlanet = 0;

// initSolarSystem
// reads in solar system data from planets and moons files
// prepares textures and data
// CALLED ONLY ONCE
void initSolarSystem()
{
  transformer.identity();
  glColor3f(1.0, 1.0, 1.0);

  // texturing parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		  GL_LINEAR_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		  GL_LINEAR_MIPMAP_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  string buffer;
  char* name = new char[20];
  char* texFile = new char[30];
  float inclination, orbitAngle, orbitDist, axialTilt, spin, size;

  // open planets data file
  ifstream inFile("planets.dat", ios::in);
  if(!inFile)
    {
      cout << "Error, cannot open \'planets.dat\'...Aborting."
	   << endl;
      exit(-1);
    }

  getline(inFile, buffer, '\n');

  // get planet data and add to solar system
  for(int i = 0; i < NUMPLANETS; i++)
    {
      getline(inFile, buffer, '\n');
      sscanf(buffer.c_str(), "%s %f %f %f %f %f %f %s", name, &inclination, 
	     &orbitAngle, &orbitDist, &axialTilt, &spin, &size, texFile);
      
      planets[i].setData(name, inclination, orbitAngle, orbitDist, 
			  axialTilt, spin, size, texFile, i);
      }
  inFile.close();

  // open moons data file
  ifstream moonFile("moons.dat", ios::in);
  getline(moonFile, buffer, '\n');

  int pID, mID;
  Moon** tempMoons = new Moon*[NUMMOONS];

  // add moons to solar system
  for(int i = 0; i < NUMMOONS; i++)
    {
      tempMoons[i] = new Moon;
      getline(moonFile, buffer, '\n');
      sscanf(buffer.c_str(), "%d %d %f %f %f %f %f %f %s", &pID, &mID, 
	     &inclination, &orbitAngle, &orbitDist, &axialTilt, &spin, &size, 
	     texFile);
      tempMoons[i]->setData(pID, inclination, orbitAngle, orbitDist, 
			    axialTilt, spin, size, texFile, mID);
      tempMoons[i]->parseTex();
      planets[pID].addMoon(tempMoons[i]);
    }
  moonFile.close();

  // load planetary textures
  for(int i = 0; i < NUMPLANETS; i++)
    planets[i].parseTex();
}

// initCamera
// sets up camera and viewport/frustum
// CALLED ONLY ONCE
void initCamera()
{  
  camState = STATIONARY;
  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION);   
  glLoadIdentity();   
  glFrustum(LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // initialize camera position/orientation and up vector
  cameraLoc.setElement(0,0);
  cameraLoc.setElement(1,0);
  cameraLoc.setElement(2,500);
  cameraOrient.setElement(0,0);
  cameraOrient.setElement(1,0);
  cameraOrient.setElement(2,-1);
  cameraUp.setElement(0,0);
  cameraUp.setElement(1,1);
  cameraUp.setElement(2,0);

  gluLookAt(cameraLoc.getElement(0), cameraLoc.getElement(1), cameraLoc.getElement(2),
	    cameraOrient.getElement(0), cameraOrient.getElement(1), cameraOrient.getElement(2),
	    cameraUp.getElement(0), cameraUp.getElement(1), cameraUp.getElement(2));
}

// adjustCamera
// moves camera to address user commands
// also used to chase planet
// called by myDisplay
void adjustCamera()
{
  // make sure we're the first thing on MODELVIEW
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if(camState == STATIONARY)
    { 
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glFrustum(LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR);
      glMatrixMode(GL_MODELVIEW);
      gluLookAt(0, 0, 1000, 0, 0, -1, 0, 1, 0);
      
      // used for user rotation
      cameraLoc = transformer * cameraLoc;
      cameraOrient = transformer * cameraOrient;
      
      // apply user transformations
      float* mat1d = new float[16];
      mat1d = transformer.transpose1d();
      glMultMatrixf(mat1d);
    }
  
  else if(camState == CHASE)
    {
      Matrix temp, temp2;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glFrustum(LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR);
      
      // start at the center of the universe (SUN)
      temp.identity();
      temp2.identity();
      
      // tilt to the plane of the planet's orbit
      temp2.rotation(0,0,1,planets[chasedPlanet].getInclination());
      temp = temp2;
      
      // rotate to face the planet
      temp2.rotation(0,1,0,planets[chasedPlanet].getOrbitAngle()*t);
      temp = temp * temp2;
      
      // move out to the center of the planet
      temp2.translation(planets[chasedPlanet].getOrbitDist(), 0,0);
      temp = temp * temp2;
      
      // turn back to face the sun
      temp2.rotation(0,0,1,180);
      temp = temp * temp2;
      
      // back up to see the planet
      temp2.translation(0,0, planets[chasedPlanet].getSize()*4);
      temp = temp * temp2;
      
      temp2 = temp;
      
      cameraLoc.zero();
      cameraOrient.zero();
      cameraOrient.setElement(2,-1);
      cameraLoc = temp2 * cameraLoc;
      cameraOrient = temp2 * cameraOrient;
      
      gluLookAt(cameraLoc.getElement(0), cameraLoc.getElement(1), cameraLoc.getElement(2),
		cameraOrient.getElement(0), cameraOrient.getElement(1), 
		cameraOrient.getElement(2), 0, 1, 0);

      glMatrixMode(GL_MODELVIEW);

      // apply user transformations
      float* mat1d = new float[16];
      mat1d = transformer.transpose1d();
      glMultMatrixf(mat1d);
      
    }
}

// chasePlanet
// sets up the state variables to chase planet p
void chasePlanet(int p)
{
  transformer.identity();
  for(int i = 0; i < NUMPLANETS; i++)
    planets[i].stopChasingMe();

  planets[p].chaseMe();
  camState = CHASE;
  chasedPlanet = p;
}

// stopChasing
// returns to stationary mode
void stopChasing()
{
  for(int i = 0; i < NUMPLANETS; i++)
    planets[i].stopChasingMe();

  camState = STATIONARY;
}

// myDisplay
// adjusts camera based on state, draws planets
void myDisplay()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  adjustCamera();
  
  for(int i = 0; i < NUMPLANETS; i++)
    planets[i].draw();
  
  glutSwapBuffers();
}

// myIdle
// sets animation parameters
void myIdle()
{
  if(animate)
    {
      // s is determined by user
      t += 0.1*s;
      ms += 0.01*s;
      glutPostRedisplay();
    }
}

// myKeyboard
// keyboard callback
// has functionality for panning, rotating, and zooming, as well as resetting the view
// can also chase planets and change animation speed and stop animation
void myKeyboard(unsigned char key, int x, int y)
{
  Matrix temp;

  switch(key)
    {
    case 'a': // toggle animation
      animate = (animate) ? false : true;
      break;

    case 'f': // speed up animation
      s += 5;
      break;

    case 's': // slow down animation
      s -= 5;
      if (s < 1)
	s = 1;
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
      temp.translation(0, 0, 10);
      transformer = temp * transformer;
      break;

    case '-': // zoom out
      temp.translation(0, 0, -10);
      transformer = temp * transformer;
      break;

    case 'o': // rotate right
      temp.rotation(1, 1, 0, 10);
      transformer = temp * transformer;
      break;

    case 'R': // reset
      transformer.identity();
      stopChasing();
      break;

    case '0': // solar system
      transformer.identity();
      stopChasing();
      break;

    case '1': // mercury
      chasePlanet(1);
      break;

    case '2': // venus
      chasePlanet(2);
      break;
      
    case '3': // earth
      chasePlanet(3);
      break;

    case '4': // mars
      chasePlanet(4);
      break;

    case '5': // jupiter
      chasePlanet(5);
      break;

    case '6': // saturn
      chasePlanet(6);
      break;

    case '7': // uranus
      chasePlanet(7);
      break;

    case '8': // neptune
      chasePlanet(8);
      break;
      
    case '9': // pluto
      chasePlanet(9);
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
  int delY = startY - y;
  float d = sqrt((float)(delX*delX + delY*delY));
  cameraOrient.norm();
  Vector subVector;
  float three = 3.0;
  subVector = cameraOrient*three;
  Vector tempVector;
  tempVector = cameraLoc + subVector;
  float z = tempVector.getElement(2);
  
  // if actively dragging
  if(lButtonDown)
    {
      motionCounter++;
      // only refresh every 3 times it's called
      if(true)
	{
	  if(myState == ZOOM)
	    {
	      if(delY > 0)
		temp.translation(0, 0, d);
	      else
		temp.translation(0, 0, -d);

	      transformer = temp * transformer;
	      startX = x;
	      startY = y;
	      glutPostRedisplay();
	    }

	  else if(myState == TRANSLATE)
	    {
	      temp.translation(delX, delY, 0);
	      transformer = temp * transformer;
	      startX = x;
	      startY = y;
	      glutPostRedisplay();
	    }

	  else if(myState == ROTATE)
	    {
	      temp.identity();
	      if(delX > 0)
		temp.rotation(-delY, delX, z, -1);
	      else
		temp.rotation(-delY, delX, z, 1);

	      transformer = temp * transformer;
	      glutPostRedisplay();
	    }
	      
	  motionCounter = 0;
	}
    }
}

int main(int argc, char* argv[])
{
  // glutInits must be called before ANYTHING else
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(0,0);
  glutCreateWindow("SolarSystem");

  // enable correct modes
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  // register callbacks
  glutDisplayFunc(myDisplay);
  glutIdleFunc(myIdle);
  glutKeyboardFunc(myKeyboard);
  glutMouseFunc(myMouse);
  glutMotionFunc(myMotion);
  
  glClearDepth(1.0);
  glClearColor(0, 0, 0, 1);
 
  // setup the solar system and camera
  initCamera();
  initSolarSystem();

  // no return from here
  glutMainLoop();

  return 0;
}
