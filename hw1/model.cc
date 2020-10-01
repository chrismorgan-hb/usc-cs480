// model.cc
// class implementation for Model
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "model.h"

//default constructor
//initializes points array to all zeros
//initializes faces array to all zeros
//resets numOfPoints and numOfFaces
//transformation matricies become identities
Model::Model()
{
  for(int i = 0; i < MAXPOINTS; i++)
    for(int j = 0; j < 3; j++)
      points[i][j] = 0;

  numOfPoints = 0;
  
  for(int i = 0; i < MAXFACES; i++)
    for(int j = 0; j < 3; j++)
      faces[i][j] = 0;

  numOfFaces = 0;

  numTransformations = 0;

  myTranslations.identity();
  myRotations.identity();
  myScales.identity();
  myShears.identity();
  for(int i = 0; i < MAXTRANSFORMATIONS; i++)
    myTransformations[i].identity();
}

//adds the point x, y, z to points array
void Model::addPoint(float x, float y, float z)
{
  if(numOfPoints < MAXPOINTS)
    {
      points[numOfPoints][0] = x;
      points[numOfPoints][1] = y;
      points[numOfPoints][2] = z;
      
      numOfPoints++;
    }
}

//adds the face represented by the points in points[p1-p3] to faces array
void Model::addFace(int p1, int p2, int p3)
{
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
}

void Model::addScale(float sx, float sy, float sz)
{
  myScales.scale(sx, sy, sz);
}

void Model::addShear(float sxy, float sxz, float syx, float syz, float szx, float szy)
{
  myShears.shear(sxy, sxz, syx, syz, szx, szy);
}

Matrix Model::getTranslations()
{
  return myTranslations;
}

Matrix Model::getRotations()
{
  return myRotations;
}

Matrix Model::getScales()
{
  return myScales;
}

Matrix Model::getShears()
{
  return myShears;
}

int Model::getNumPoints()
{
  return numOfPoints;
}

int Model::getNumFaces()
{
  return numOfFaces;
}

int* Model::getFace(int i)
{
  return faces[i];
}

//returns a vector representation of points[i], with w = 1
Vector Model::getPoint(int i)
{
  Vector v;
  for(int j = 0; j < 3; j++)
    v.setElement(j,points[i][j]);
  v.setElement(3,1);

  return v;
}

//makeTransformation
//uses 4 types of transformations to make a single transformation matrix
//that can be pushed onto the stack
void Model::makeTransformation()
{
  Matrix temp;
  temp.identity();

  temp = myTranslations * temp;
  temp = myRotations * temp;
  temp = myScales * temp;
  temp = myShears * temp;

  myTransformations[numTransformations] = temp;

  numTransformations++;

  //reset all transformation matricies for the next transformation
  myTranslations.identity();
  myRotations.identity();
  myScales.identity();
  myShears.identity();
}

int Model::getNumTransformations()
{
  return numTransformations;
}

Matrix Model::getTransformation(int i)
{
  Matrix temp;
  temp.identity();

  if(i < numTransformations)
    return myTransformations[i];

  else
    return temp;
}



