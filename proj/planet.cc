// planet.cc
// class implementation for Planet
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include <GL/glut.h>
#include "planet.h"
#include "globals.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

// used for animation
extern float t;

// constructor
// initialize all data
Planet::Planet()
{
  planetObj = gluNewQuadric();
  gluQuadricDrawStyle(planetObj, GLU_FILL);
  gluQuadricNormals(planetObj, GLU_SMOOTH);
  gluQuadricTexture(planetObj, GL_TRUE);
  gluQuadricOrientation(planetObj, GLU_OUTSIDE);
  chased = false;
  numMoons = 0;
}

// destructor
// cleans up quadric object
Planet::~Planet()
{
  gluDeleteQuadric(planetObj);
}

// addMoon
// adds the moon pointed to by m
// used when building the solar system
// for the first time
void Planet::addMoon(Moon* m)
{
  m->setID(numMoons);
  moons[numMoons] = m;
  numMoons++;
}

// getMoon
// returns a pointer to the moon with 
// ID = i
Moon* Planet::getMoon(int i)
{
  return moons[i];
}

// chaseMe
// tells the planet it's being chased
void Planet::chaseMe()
{
  chased = true;
}

// stopChasingMe
// tells the planet it's no longer being
// chased
void Planet::stopChasingMe()
{
  chased = false;
}
  
// draw
// draws the planet
// uses rotation/translation combinations
// from the sun
void Planet::draw()
{
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texName);

  // avoid corrupting current MODELVIEW
  glPushMatrix();

  // tilt of orbit plane
  glRotatef(inclination, 0.0, 0.0, 1.0);
  
  // location in orbital circle
  float oa = orbitAngle * t;
  glRotatef(oa, 0.0, 1.0, 0.0);
  
  // distance from sun
  glTranslatef(orbitDist, 0.0, 0.0);
  
  // prevent corruption for drawing moons
  glPushMatrix();

  // rotational axis
  glRotatef(axialTilt, 1.0, 0.0, 0.0);
  
  // rotation speed
  float sp = spin * t;
  glRotatef(sp, 0.0, 1.0, 0.0);

  // tweak so texture looks right
  glRotatef(90.0, 1.0, 0.0, 0.0);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  gluQuadricDrawStyle(planetObj, GLU_FILL);
  gluQuadricNormals(planetObj, GLU_SMOOTH);
  gluQuadricTexture(planetObj, GL_TRUE);
  gluQuadricOrientation(planetObj, GLU_OUTSIDE);
  gluSphere(planetObj, size, 20, 20);
  
  glPopMatrix();
  
  // draw the moons
  for(int i = 0; i < numMoons; i++)
    {
      moons[i]->draw();
    }

  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
}

// parseTex
// read in and store texture 
// information from file associated
// with the planet
void Planet::parseTex()
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

  // read actual data
  for(int i = 0; i < texWidth*texHeight*3; i++)
    {
      inFile.read(&x, 1);
      texArr[i] = x;
    }
  inFile.close();

  glGenTextures(1, &texName);
  cout << "binding " << name << " texture..." << endl;
  glBindTexture(GL_TEXTURE_2D, texName);
  cout << "building " << name << " mipmap..." << endl;
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texWidth, texHeight, GL_RGB, 
		    GL_UNSIGNED_BYTE, texArr);
}

// setData
// assigns values to data fields
// in the planet object
void Planet::setData(string n, float inc, float oa, float od, float at,
		     float sp, float sz, string t, int i)
{
  name = n;
  inclination = inc;
  orbitAngle = oa;
  orbitDist = od;
  axialTilt = at;
  spin = sp;
  size = sz;
  texFile = t;
  planetID = i;
}

// ACCESSORS
// used to chase planets

// getName
// name accessor
string Planet::getName()
{
  return name;
}

// getInclination
// inclination accessor
float Planet::getInclination()
{
  return inclination;
}

// getOrbitAngle
// orbitAngle accessor
float Planet::getOrbitAngle()
{
  return orbitAngle;
}

// getOrbitDist
// orbitDist accessor
float Planet::getOrbitDist()
{
  return orbitDist;
}

// getAxialTilt
// axialTilt accessor
float Planet::getAxialTilt()
{
  return axialTilt;
}

// getSpin
// spin accessor
float Planet::getSpin()
{
  return spin;
}

// getSize
// size accessor
float Planet::getSize()
{
  return size;
}
