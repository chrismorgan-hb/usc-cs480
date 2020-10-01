// moon.cc
// class implementation for Moon
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "moon.h"
#include <GL/glut.h>
#include <fstream>
#include <stdio.h>
#include <iostream>
using namespace std;

// used for animation
extern float ms;

// constructor
// handles all initialization
Moon::Moon()
{
  moonObj = gluNewQuadric();
  gluQuadricDrawStyle(moonObj, GLU_FILL);
  gluQuadricNormals(moonObj, GLU_SMOOTH);
  gluQuadricTexture(moonObj, GL_TRUE);
  gluQuadricOrientation(moonObj, GLU_OUTSIDE);
}

// destructor
// cleans up the quadric object
Moon::~Moon()
{
  gluDeleteQuadric(moonObj);
}

// draw
// called by Planet::draw()
// already at planet location when called
// simply transform based on moon data
// and draw
void Moon::draw()
{
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texName);

  glPushMatrix();
  // orbital plane tilt
  glRotatef(inclination, 0.0, 0.0, 1.0);
  
  // location on orbital circle
  float oa = orbitAngle * ms;
  glRotatef(oa, 0.0, 1.0, 0.0);
  
  // distance from sun
  glTranslatef(orbitDist, 0.0, 0.0);
  
  // tilt on rotation axis
  glRotatef(axialTilt, 1.0, 0.0, 0.0);
  
  // rotation speed
  float sp = spin * ms;
  glRotatef(sp, 0.0, 1.0, 0.0);
  
  // tweak to make texture look better
  glRotatef(90.0, 1.0, 0.0, 0.0);

  // actual drawing
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  gluQuadricDrawStyle(moonObj, GLU_FILL);
  gluQuadricNormals(moonObj, GLU_SMOOTH);
  gluQuadricTexture(moonObj, GL_TRUE);
  gluQuadricOrientation(moonObj, GLU_OUTSIDE);
  gluSphere(moonObj, size, 20, 20);
  
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
}

// parseTex
// parses the texture file associated with this
// Moon object and creates a mipmap
void Moon::parseTex()
{
  // open texture file
  string filename = "./textures/" + texFile + ".ppm";
  
  ifstream inFile(filename.c_str(), ios::in);
  
  // make sure file exists
  if(!inFile)
    {
      cout << "Error, cannot open file: " << filename << " ...Aborting."
	   << endl;
      exit(-1);
    }

  string buffer;

  // begin parsing
  getline(inFile, buffer, '\n'); //P6, ignore
  getline(inFile, buffer, '\n'); //comment, ignore
  getline(inFile, buffer, '\n'); //w, h
  sscanf(buffer.c_str(), "%d %d", &texWidth, &texHeight);
  getline(inFile, buffer, '\n'); //intensity, ignore
  
  texArr = new unsigned char[texWidth*texHeight*3];
  
  char x;

  // get the pixel data
  for(int i = 0; i < texWidth*texHeight*3; i++)
    {
      inFile.read(&x, 1);
      texArr[i] = x;
    }
  inFile.close();

  glGenTextures(1, &texName);
  cout << "binding moon #" << moonID << " texture for planet #" << planetID << "..." << endl;
  glBindTexture(GL_TEXTURE_2D, texName);
  cout << "building moon #" << moonID << " mipmap for planet #" << planetID << "..." << endl;
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texWidth, texHeight, GL_RGB, 
		    GL_UNSIGNED_BYTE, texArr);
}

// setData
// fills data fields based on passed data
void Moon::setData(int pID, float inc, float oa, float od, float at, float sp,
		    float sz, char* t, int i)
{
  inclination = inc;
  orbitAngle = oa;
  orbitDist = od;
  axialTilt = at;
  spin = sp;
  size = sz;
  texFile = t;
  planetID = pID;
  moonID = i;
}

// setID
// sets the Moon's ID
// this code is obsolete
void Moon::setID(int i)
{
  moonID = i;
}
