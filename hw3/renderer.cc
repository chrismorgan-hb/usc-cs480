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
#include "GL/glut.h"

class Camera;

extern Matrix transformer; // used by callback functions to do translations, zooms, and rotations

int tokenize(char* token);

// Renderer
// creates renderer with t as texture file
Renderer::Renderer(char* t)
{
  numLights = 0;
  numModels = 0;
  displayType = WIREFRAME;
  texFile = t;
}

Renderer::~Renderer()
{
  delete framebuffer;
}

// setDisplayType
// changes between wireframe, shaded, textured, and shaded texturing
void Renderer::setDisplayType(int d)
{
  if(d == WIREFRAME || d == SHADED || d == TEXTURE || d == SHADED_TEXTURE)
    displayType = d;
}

int Renderer::getDisplayType()
{
  return displayType;
}

Camera* Renderer::getCamera()
{
  return &myCamera;
}

void Renderer::setTexFile(char* t)
{
  texFile = t;
}

// parseTex
// turns the texture file into a 1d array that OpenGL can use
void Renderer::parseTex()
{
  ifstream inFile(texFile, ios::in);
  
  // make sure file exists
  if(!inFile)
    {
      cout << "Error, cannot open file: " << texFile << " ...Aborting."
	   << endl;
      exit(-1);
    }

  string buffer;

  getline(inFile, buffer, '\n'); //P6, ignore
  getline(inFile, buffer, '\n'); //w, h
  sscanf(buffer.c_str(), "%d %d", &texWidth, &texHeight);
  getline(inFile, buffer, '\n'); //intensity, ignore

  texArr = new unsigned char[texWidth*texHeight*3];
  
  unsigned char x;

  // get R, G, B for each pixel of texture, add to array sequentially
  for(int i = 0; i < texWidth*texHeight*3; i++)
    {
      inFile.read(&x, 1);
      texArr[i] = x;
    }
}

//allocate
//sets up framebuffer
void Renderer::allocate(int w, int h)
{
  framebuffer = new Framebuffer(w, h);
}

void Renderer::setWidth(int w)
{
  framebuffer->setWidth(w);
}

void Renderer::setHeight(int h)
{
  framebuffer->setHeight(h);
}

int Renderer::getTexWidth()
{
  return texWidth;
}

int Renderer::getTexHeight()
{
  return texHeight;
}

unsigned char* Renderer::getTexArr()
{
  return texArr;
}

// render
// draws the scene
void Renderer::render()
{
  // get camera parameters
  float camNear = myCamera.getNear();
  float camFar = myCamera.getFar();
  float camTop = myCamera.getTop();
  float camBottom = myCamera.getBottom();
  float camLeft = myCamera.getLeft();
  float camRight = myCamera.getRight();
  float camX = myCamera.getPosition()[0];
  float camY = myCamera.getPosition()[1];
  float camZ = myCamera.getPosition()[2];

  // get window parameters
  int w, h;
  w = framebuffer->getWidth();
  h = framebuffer->getHeight();

  // set up window size
  glViewport(0, 0, w, h);

  // projection matrix calculations
  glMatrixMode(GL_PROJECTION);   
  glLoadIdentity();     
  glFrustum(camLeft, camRight, camBottom, camTop, camNear, camFar);
  
  float* mat1d;
  float temp2d[4][4];
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess[] = {1.0};
  Model* theModelToRender;
  
  // object coords to view coords matrix calculations
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(camX, camY, camZ, 0, 0, -1, 0, 1, 0);

  // for each model
  for(int m = 0; m < numModels; m++)
    {
      // to prevent corruption of gluLookAt
      glPushMatrix();
      theModelToRender = getModelToRender(m);
      
      // for each transformation on the model
      for(int k = 0; k < theModelToRender->getNumTransformations(); k++)
	{
	  for(int i = 0; i < 4; i++)
	    for(int j = 0; j < 4; j++)
	      temp2d[i][j] = theModelToRender->getTransformation(k).getElement(i,j);
	  
	  
	  float temp1d[16];
	  
	  // convert to 1d array
	  int counter = 0;
	  for(int i = 0; i < 4; i++)
	    {
	      for(int j = 0; j < 4; j++)
		{
		  temp1d[counter] = temp2d[j][i];
		  counter++;
		}
	    }
	  
	  // apply the transformatino
	  glMultMatrixf((GLfloat*)temp1d);
	}

      // apply user-generated transformations
      mat1d = transformer.transpose1d();
      glMultMatrixf(mat1d);

      // set up material properties
      ambient[0] = theModelToRender->getAmbient().getElement(0);
      ambient[1] = theModelToRender->getAmbient().getElement(1);
      ambient[2] = theModelToRender->getAmbient().getElement(2);
      ambient[3] = 1.0;

      glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

      diffuse[0] = theModelToRender->getDiffuse().getElement(0);
      diffuse[1] = theModelToRender->getDiffuse().getElement(1);
      diffuse[2] = theModelToRender->getDiffuse().getElement(2);
      diffuse[3] = 1.0;
      
      glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

      specular[0] = theModelToRender->getSpecular().getElement(0);
      specular[1] = theModelToRender->getSpecular().getElement(1);
      specular[2] = theModelToRender->getSpecular().getElement(2);
      specular[3] = 1.0;

      glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

      shininess[0] = theModelToRender->getShininess();
      
      glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

      float x,y,z,u,v,w;
      Vector A, B, C, D, E, F;

      // draw the model
      if(displayType == WIREFRAME)
	{
	  // white lines
	  glColor3f(1.0, 1.0, 1.0);
	  
	  // for each face
	  for(int i = 0; i < theModelToRender->getNumFaces(); i++)
	    {
	      glBegin(GL_LINE_LOOP);
	      
	      A = theModelToRender->getPoint((theModelToRender->getFace(i))[0]);
	      x = A.getElement(0);
	      y = A.getElement(1);
	      z = A.getElement(2);
	      
	      glVertex3f(x,y,z);
	      
	      B = theModelToRender->getPoint((theModelToRender->getFace(i))[1]);
	      x = B.getElement(0);
	      y = B.getElement(1);
	      z = B.getElement(2);
	      
	      glVertex3f(x,y,z);
	      
	      C = theModelToRender->getPoint((theModelToRender->getFace(i))[2]);
	      x = C.getElement(0);
	      y = C.getElement(1);
	      z = C.getElement(2);
	      
	      glVertex3f(x,y,z);
	      
	      glEnd();
	    }
	} // if WIREFRAME

      else if(displayType == SHADED)
	{
	  float r, g, b;
	  float color[4];
	  float pos[3];
	  glShadeModel(GL_SMOOTH);
	  glEnable(GL_LIGHTING);
	  glEnable(GL_DEPTH_TEST); 
	  glEnable(GL_NORMALIZE);
	  glDisable(GL_COLOR_MATERIAL);
	 
	  // prevent corruption of original MODELVIEW matrix
	  glPushMatrix();
	  glLoadIdentity();

	  // setup lights
	  for(unsigned int j = 0; j < numLights; j++)
	    {
	      glEnable(GL_LIGHT0 + j);
	      
	      r = (myLights[j].getColor().getR())/255.0;
	      g = (myLights[j].getColor().getG())/255.0;
	      b = (myLights[j].getColor().getB())/255.0;
	      
	      color[0] = r;
	      color[1] = g;
	      color[2] = b;
	      color[3] = 1.0;
	      
	      pos[0] = myLights[j].getLocation().getElement(0);
	      pos[1] = myLights[j].getLocation().getElement(1);
	      pos[2] = myLights[j].getLocation().getElement(2);
	      
	      //glLightfv(GL_LIGHT0 + j, GL_AMBIENT, color); //commented so shading looks better
	      glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, color);
	      glLightfv(GL_LIGHT0 + j, GL_SPECULAR, color);
	      glLightfv(GL_LIGHT0 + j, GL_POSITION, pos);
	    }
	  glPopMatrix();

	  glBegin(GL_TRIANGLES);

	  for(int i = 0; i < theModelToRender->getNumFaces(); i++)
	    {
	      A = theModelToRender->getPoint((theModelToRender->getFace(i))[0]);
	      x = A.getElement(0);
	      y = A.getElement(1);
	      z = A.getElement(2);
	      
	      D = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[0]);
	      u = D.getElement(0);
	      v = D.getElement(1);
	      w = D.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);

	      B = theModelToRender->getPoint((theModelToRender->getFace(i))[1]);
	      x = B.getElement(0);
	      y = B.getElement(1);
	      z = B.getElement(2);
	      
	      E = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[1]);
	      u = E.getElement(0);
	      v = E.getElement(1);
	      w = E.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);

	      C = theModelToRender->getPoint((theModelToRender->getFace(i))[2]);
	      x = C.getElement(0);
	      y = C.getElement(1);
	      z = C.getElement(2);
	      
	      F = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[2]);
	      u = F.getElement(0);
	      v = F.getElement(1);
	      w = F.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);
	    }
	  glEnd();

	  // prevent lighting from carrying over to other modes
	  glDisable(GL_LIGHTING);
	  glDisable(GL_DEPTH_TEST); 
	}
      
      else if(displayType == TEXTURE)
	{
	  // texture already built in initial function
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
	  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	  glEnable(GL_TEXTURE_2D);
	  glEnable(GL_NORMALIZE);
	  glEnable(GL_DEPTH_TEST); 
	  
	  glBegin(GL_TRIANGLES);

	  for(int i = 0; i < theModelToRender->getNumFaces(); i++)
	    {
	      A = theModelToRender->getPoint((theModelToRender->getFace(i))[0]);
	      x = A.getElement(0);
	      y = A.getElement(1);
	      z = A.getElement(2);
	      
	      D = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[0]);
	      u = D.getElement(0);
	      v = D.getElement(1);
	      w = D.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);

	      B = theModelToRender->getPoint((theModelToRender->getFace(i))[1]);
	      x = B.getElement(0);
	      y = B.getElement(1);
	      z = B.getElement(2);
	      
	      E = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[1]);
	      u = E.getElement(0);
	      v = E.getElement(1);
	      w = E.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);

	      C = theModelToRender->getPoint((theModelToRender->getFace(i))[2]);
	      x = C.getElement(0);
	      y = C.getElement(1);
	      z = C.getElement(2);
	      
	      F = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[2]);
	      u = F.getElement(0);
	      v = F.getElement(1);
	      w = F.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);
	    }
	  glEnd();

	  // prevent texturing from corrupting other modes
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
	  glDisable(GL_TEXTURE_2D);
	  glDisable(GL_DEPTH_TEST);
	}
      
      else if(displayType == SHADED_TEXTURE)
	{
	  float r, g, b;
	  float color[4];
	  float pos[3];
	  glShadeModel(GL_SMOOTH);
	  glEnable(GL_LIGHTING);
	  glEnable(GL_DEPTH_TEST); 
	  glEnable(GL_NORMALIZE);
	  glDisable(GL_COLOR_MATERIAL);

	  // prevent MODELVIEW corruption
	  glPushMatrix();
	  glLoadIdentity();
	  
	  // setup lights
	  for(unsigned int j = 0; j < numLights; j++)
	    {
	      glEnable(GL_LIGHT0 + j);
	      
	      r = (myLights[j].getColor().getR())/255.0;
	      g = (myLights[j].getColor().getG())/255.0;
	      b = (myLights[j].getColor().getB())/255.0;
	      
	      color[0] = r;
	      color[1] = g;
	      color[2] = b;
	      color[3] = 1.0;
	      
	      pos[0] = myLights[j].getLocation().getElement(0);
	      pos[1] = myLights[j].getLocation().getElement(1);
	      pos[2] = myLights[j].getLocation().getElement(2);
	      
	      //glLightfv(GL_LIGHT0 + j, GL_AMBIENT, color); //commented so shading looks better
	      glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, color);
	      glLightfv(GL_LIGHT0 + j, GL_SPECULAR, color);
	      glLightfv(GL_LIGHT0 + j, GL_POSITION, pos);
	    }
	  glPopMatrix();

	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
	  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	  glEnable(GL_TEXTURE_2D);
	  glEnable(GL_NORMALIZE);
	  glEnable(GL_DEPTH_TEST); 
	  
	  glBegin(GL_TRIANGLES);

	  for(int i = 0; i < theModelToRender->getNumFaces(); i++)
	    {
	      A = theModelToRender->getPoint((theModelToRender->getFace(i))[0]);
	      x = A.getElement(0);
	      y = A.getElement(1);
	      z = A.getElement(2);
	      
	      D = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[0]);
	      u = D.getElement(0);
	      v = D.getElement(1);
	      w = D.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);

	      B = theModelToRender->getPoint((theModelToRender->getFace(i))[1]);
	      x = B.getElement(0);
	      y = B.getElement(1);
	      z = B.getElement(2);
	      
	      E = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[1]);
	      u = E.getElement(0);
	      v = E.getElement(1);
	      w = E.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);

	      C = theModelToRender->getPoint((theModelToRender->getFace(i))[2]);
	      x = C.getElement(0);
	      y = C.getElement(1);
	      z = C.getElement(2);
	      
	      F = theModelToRender->getVertexNormal((theModelToRender->getFace(i))[2]);
	      u = F.getElement(0);
	      v = F.getElement(1);
	      w = F.getElement(2);
	      
	      glNormal3f(u, v, w);
	      glVertex3f(x, y, z);
	    }
	  glEnd();

	  // prevent shading/texturing from corrupting other modes
	  glDisable(GL_LIGHTING);
	  glDisable(GL_DEPTH_TEST);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
	  glDisable(GL_TEXTURE_2D);
	}
      
      glPopMatrix();
    } // for each model
  glPopMatrix();
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
  int modelNumber = 0;
  Model* theModelToRender;

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
	  modelNumber = addModel();
	  theModelToRender = getModelToRender(modelNumber);
	  getline(inFile, buffer, '\n');
	  sscanf(buffer.c_str(), "%s", c_buffer);

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
	   
		  //getline(inFile, buffer, '\n');
		  getline(inFile, buffer, '\n');
		  sscanf(buffer.c_str(), "%s", c_buffer);
		  if(!strcmp(c_buffer, "}"))
		    {
		      getline(inFile, buffer, '\n');
		      sscanf(buffer.c_str(), "%s", c_buffer);
		    }
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
		  getline(inFile, buffer, '\n');
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

// addModel
// adds a model if there is room
// returns the index of added model
int Renderer::addModel()
{
  numModels++;
  if(numModels < MAXMODELS)
    {
      modelsToRender[numModels-1] = new Model();
      return numModels - 1;
    }
  else
    {
      numModels--;
      return -1;
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

int Renderer::getNumModels()
{
  return numModels;
}

// getModelToRender
// used for compatibility with old code
Model* Renderer::getModelToRender(int i)
{
  if(i < numModels)
    return modelsToRender[i];

  else
    return NULL;
}
