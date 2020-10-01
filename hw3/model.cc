// model.cc
// class implementation for Model
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "model.h"
#include <iostream>
using namespace std;
#include "GL/glut.h"

//default constructor
//initializes points array to all zeros
//initializes faces array to all zeros
//resets numOfPoints and numOfFaces
//transformation matricies become identities
Model::Model()
{
  Vector v;
  v.zero();
  
  for(int i = 0; i < MAXPOINTS; i++)
    for(int j = 0; j < 3; j++)
      {
	points[i][j] = 0;
	cameraSpacePoints[i] = v;
	vertexNormals[i] = v;
      }

  numOfPoints = 0;
  numOfVertexNormals = 0;

  for(int i = 0; i < MAXFACES; i++)
    for(int j = 0; j < 3; j++)
      {
	faces[i][j] = 0;
	faceNormals[i] = v;
      }

  numOfFaces = 0;
  numOfFaceNormals = 0;

  numTransformations = 0;

  myTranslations.identity();
  myRotations.identity();
  myScales.identity();

  for(int i = 0; i < MAXTRANSFORMATIONS; i++)
    myTransformations[i].identity();
  
  myTotalRotations.identity();
}

//addPoint
//adds the point x, y, z to points array
void Model::addPoint(float x, float y, float z)
{
  // bounds check
  if(numOfPoints < MAXPOINTS)
    {
      points[numOfPoints][0] = x;
      points[numOfPoints][1] = y;
      points[numOfPoints][2] = z;
      
      numOfPoints++;
    }
}

//addFace
//adds the face represented by the points in points[p1-p3] to faces array
void Model::addFace(int p1, int p2, int p3)
{
  // bounds check
  if(numOfFaces < MAXFACES)
    {
      faces[numOfFaces][0] = p1;
      faces[numOfFaces][1] = p2;
      faces[numOfFaces][2] = p3;

      numOfFaces++;
    }
}

void Model::addTranslation(float tx, float ty, float tz)
{
  myTranslations.translation(tx, ty, tz);
}

void Model::addRotation(float x, float y, float z, float theta)
{
  myRotations.rotation(x, y, z, theta);
  myTotalRotations.rotation(x, y, z, theta);
}

void Model::addScale(float sx, float sy, float sz)
{
  myScales.scale(sx, sy, sz);
}

void Model::addVertexNormal(Vector v)
{
  // bounds check
  if(numOfVertexNormals < MAXPOINTS)
    {
      vertexNormals[numOfVertexNormals] = v;
      numOfVertexNormals++;
    }
}

void Model::addFaceNormal(Vector v)
{
  if(numOfFaceNormals < MAXFACES)
    {
      faceNormals[numOfFaceNormals] = v;
      numOfFaceNormals++;
    }
}

Matrix Model::getTranslations()
{
  return myTranslations;
}

Matrix Model::getRotations()
{
  return myRotations;
}

Matrix Model::getTotalRotations()
{
  return myTotalRotations;
}

Matrix Model::getScales()
{
  return myScales;
}

int Model::getNumPoints()
{
  return numOfPoints;
}

int Model::getNumFaces()
{
  return numOfFaces;
}

int Model::getNumVertexNormals()
{
  return numOfVertexNormals;
}

int Model::getNumFaceNormals()
{
  return numOfFaceNormals;
}

int* Model::getFace(int i)
{
  return faces[i];
}

//getPoint
//returns a vector representation of points[i], with w = 1
Vector Model::getPoint(int i)
{
  Vector v;

  for(int j = 0; j < 3; j++)
    v.setElement(j,points[i][j]);

  v.setElement(3,1);

  return v;
}

Vector Model::getCameraSpacePoint(int i)
{
  Vector v;
  v.zero();

  // bounds check
  if(i < numOfPoints)
    return cameraSpacePoints[i];

  else
    return v;
}

Vector Model::getWindowSpacePoint(int i)
{
  Vector v;
  v.zero();

  // bounds check
  if(i < numOfPoints)
    return windowSpacePoints[i];

  else
    return v;
}

Vector Model::getVertexNormal(int i)
{
  Vector v;
  v.zero();
  
  // bounds check
  if(i < numOfVertexNormals)
    return vertexNormals[i];

  else
    return v;
}

Vector Model::getFaceNormal(int i)
{
  Vector v;
  v.zero();

  // bounds check
  if(i < numOfFaceNormals)
    return faceNormals[i];

  else
    return v;
}

Vector Model::getCenterPoint(int i)
{
  Vector v;
  v.zero();

  // bounds check
  if(i < numOfFaces)
    return centerPoints[i];

  else
    return v;
}

RGBColor Model::getVertexColor(int i)
{
  RGBColor black(0,0,0);

  // bounds check
  if(i < numOfPoints)
    return vertexColors[i];

  else
    return black;
}

RGBColor Model::getFaceColor(int i)
{
  RGBColor black(0,0,0);
  
  // bounds check
  if(i < numOfFaces)
    return faceColors[i];

  else
    return black;
}

void Model::setFaceNormal(int i, Vector v)
{
  // bounds check
  if(i < numOfFaceNormals)
    faceNormals[i] = v;
}

void Model::setVertexNormal(int i, Vector v)
{
  // bounds check
  if(i < numOfPoints)
    vertexNormals[i] = v;
}

void Model::setCameraSpacePoint(int i, Vector v)
{
  // bounds check
  if(i < numOfPoints)
    cameraSpacePoints[i] = v;
}

void Model::setWindowSpacePoint(int i, Vector v)
{
  // bounds check
  if(i < numOfPoints)
    windowSpacePoints[i] = v;
}

void Model::setCenterPoint(int i, Vector v)
{
  // bounds check
  if(i < numOfFaces)
    centerPoints[i] = v;
}

void Model::setVertexColor(int i, RGBColor c)
{
  // bounds check
  if(i < numOfPoints)
    vertexColors[i] = c;
}

void Model::setFaceColor(int i, RGBColor c)
{
  // bounds check
  if(i < numOfFaces)
    faceColors[i] = c;
}

//makeTransformation
//uses 3 types of transformations to make a single transformation matrix
//that can be pushed onto the stack
void Model::makeTransformation()
{
  Matrix temp;
  temp.identity();

  temp = temp * myTranslations;
  temp = temp * myRotations;
  temp = temp * myScales;
  
  myTransformations[numTransformations] = temp;

  numTransformations++;

  //reset all transformation matricies for the next transformation
  myTranslations.identity();
  myRotations.identity();
  myScales.identity();
}

int Model::getNumTransformations()
{
  return numTransformations;
}

Matrix Model::getTransformation(int i)
{
  Matrix temp;
  temp.identity();

  // bounds check
  if(i < numTransformations)
    return myTransformations[i];

  else
    return temp;
}

void Model::setAmbient(Vector c)
{
  myAmbient = c;
}

void Model::setDiffuse(Vector c)
{
  myDiffuse = c;
}

void Model::setSpecular(Vector c)
{
  mySpecular = c;
}

void Model::setShininess(float f)
{
  myShininess = f;
}

Vector Model::getAmbient()
{
  return myAmbient;
}

Vector Model::getDiffuse()
{
  return myDiffuse;
}

Vector Model::getSpecular()
{
  return mySpecular;
}

float Model::getShininess()
{
  return myShininess;
}


