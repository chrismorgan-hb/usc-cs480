// renderer.cc
// class implementation for Renderer
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "renderer.h"
#include "globals.h"
#include <fstream>
#include <string>
using namespace std;
#include <stdio.h>
#include <ctype.h>
#include "camera.h"

class Camera;

int tokenize(char* token);

Renderer::Renderer()
{
  theModelToRender = new Model;
}

Renderer::~Renderer()
{
  delete framebuffer;
  delete theModelToRender;
}

//allocate
//sets up framebuffer
//pushes window coordinate matrix onto stack
void Renderer::allocate(int w, int h)
{
  framebuffer = new Framebuffer(w, h);
  Matrix temp;
  temp.identity();
  temp.setElement(0,0, w/2);
  temp.setElement(0,3, w/2);
  temp.setElement(1,1, h/2);
  temp.setElement(1,3, h/2);

  matrixStack.push(temp);
}

//render
//finishes filling matrix stack
//goes through faces array of model and transforms points
//draws lines to make faces
void Renderer::render()
{
  float camNear = myCamera.getNear();
  float camFar = myCamera.getFar();
  float camTop = myCamera.getTop();
  float camBottom = myCamera.getBottom();
  float camLeft = myCamera.getLeft();
  float camRight = myCamera.getRight();

  Matrix temp;
  temp.zero();
  temp.setElement(0,0, (2*camNear)/(camRight - camLeft));
  temp.setElement(0,2, (camRight + camLeft)/(camRight - camLeft));
  temp.setElement(1,1, (2*camNear)/(camTop - camBottom));
  temp.setElement(1,2, (camTop + camBottom)/(camTop - camBottom));
  temp.setElement(2,2, -(camFar + camNear)/(camFar - camNear));
  temp.setElement(2,3, -(2*camNear*camFar)/(camFar - camNear));
  temp.setElement(3,2, -1);

  //temp = NP Matrix
  matrixStack.push(temp); 
  
  temp.zero();
  float* camOrient = myCamera.getOrientation();
  temp.rotation(camOrient[0], camOrient[1], camOrient[2], camOrient[3]);

  Vector axis;
  
  axis.xAxis();
  Vector u;
  u = temp * axis;

  axis.yAxis();
  Vector v;
  v = temp * axis;

  axis.zAxis();
  Vector n;
  n = temp * axis;

  float* camPosition = myCamera.getPosition();
  Vector r;

  r.setData(camPosition);
  
  temp.identity();

  temp.setElement(0,0, u.getElement(0));
  temp.setElement(0,1, u.getElement(1));
  temp.setElement(0,2, u.getElement(2));
  temp.setElement(0,3, -(r.dot(&u)));

  temp.setElement(1,0, v.getElement(0));
  temp.setElement(1,1, v.getElement(1));
  temp.setElement(1,2, v.getElement(2));
  temp.setElement(1,3, -(r.dot(&v)));
  
  temp.setElement(2,0, n.getElement(0));
  temp.setElement(2,1, n.getElement(1));
  temp.setElement(2,2, n.getElement(2));
  temp.setElement(2,3, -(r.dot(&n)));
  
  matrixStack.push(temp); // camera matrix

  // transformation matricies
  for(int k = 0; k < theModelToRender->getNumTransformations(); k++)
    {
      temp = theModelToRender->getTransformation(k);
      matrixStack.push(temp);
    }

  Vector A;
  Vector B;
  Vector C;
  
  for(int i = 0; i < theModelToRender->getNumFaces(); i++)
    {
      A = theModelToRender->getPoint((theModelToRender->getFace(i))[0]);
      B = theModelToRender->getPoint((theModelToRender->getFace(i))[1]);
      C = theModelToRender->getPoint((theModelToRender->getFace(i))[2]);

      A = matrixStack.getTop() * A;
      B = matrixStack.getTop() * B;
      C = matrixStack.getTop() * C;

      A.homogenize();
      B.homogenize();
      C.homogenize();

      framebuffer->drawLine(A.getElement(0), A.getElement(1), 
			    B.getElement(0), B.getElement(1));
      framebuffer->drawLine(B.getElement(0), B.getElement(1), 
			    C.getElement(0), C.getElement(1));
      framebuffer->drawLine(C.getElement(0), C.getElement(1), 
			    A.getElement(0), A.getElement(1));
    }
}

//dump
//writes rendered framebuffer to file
void Renderer::dump()
{
  framebuffer->writeOut();
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
  bool flag = false;
  float x, y, z, theta, xy, xz, yx, yz, zx, zy;
  Matrix tempMatrix;

  getline(inFile, buffer, '\n'); //get first line of file
  while(!flag)
    {
      sscanf(buffer.c_str(), "%s", c_buffer);
      tok = tokenize(c_buffer);

      switch(tok)
	{
	case CAMERA:
	  while(strcmp(c_buffer, "}"))
	    {
	      getline(inFile, buffer, '\n');
	      sscanf(buffer.c_str(), "%s", c_buffer);
	      tok2 = tokenize(c_buffer);
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
		}//switch
	    }//while != }
	  break;
	  
	case SEPARATOR:
	  int p1, p2, p3, neg1;
	  getline(inFile, buffer, '\n');
	  while(strcmp(c_buffer, "}"))
	    {
	      sscanf(buffer.c_str(), "%s", c_buffer);
	      tok3 = tokenize(c_buffer);
	      
	      switch(tok3)
		{	
		case TRANSFORM:		
		  // get translation, rotation, scaleFactor, and shearing
		  getline(inFile, buffer, '\n');
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

			case SHEARING:
			  sscanf(buffer.c_str(), "%s %f %f %f %f %f %f", 
				 garbage, &xy, &xz, &yx, &yz, &zx, &zy);
			  theModelToRender->addShear(xy,xz,yx,yz,zx,zy);
			  break;

			default:
			  break;
			} //switch(tok4)

		      getline(inFile, buffer, '\n');
		      sscanf(buffer.c_str(), "%s", c_buffer);
		      
		    }// while TRANSFORM
		  theModelToRender->makeTransformation();
		  break;
		  
		case COORDINATE3:
		  getline(inFile, buffer, '\n'); //get first point
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
		  break;
		  
		case INDEXEDFACESET:
		  getline(inFile, buffer, '\n'); //get first face
		  sscanf(buffer.c_str(), "%s %d %d %d %d", garbage, 
			 &p1, &p2, &p3, &neg1);
		  theModelToRender->addFace(p1, p2, p3);

		  getline(inFile, buffer, '\n');
		  while(strcmp(c_buffer, "]")) //get rest of faces
		    {
		      sscanf(buffer.c_str(), "%d %d %d %d", 
			     &p1, &p2, &p3, &neg1);
		      theModelToRender->addFace(p1, p2, p3);

		      getline(inFile, buffer, '\n');
		      sscanf(buffer.c_str(), "%s", c_buffer);
		    }
		  break;
		  
		default:
		  break;
		} // switch (tok3)
	      
	      getline(inFile, buffer, '\n');
	      getline(inFile, buffer, '\n');
	      sscanf(buffer.c_str(), "%s", c_buffer);
	      
	    } // while SEPARATOR
	  break;
	  
	case COMMENT:
	  break;
	  
	default:
	  break;
	}

      getline(inFile, buffer, '\n');
      if(inFile.eof())
	{
	  flag = true;
	}
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

  if(!strcmp(token, "shearing"))
    return SHEARING;
  
  return -1; //no related keyword
}


