// model.h
// class definition for Model
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef MODEL_H
#define MODEL_H

#include "globals.h"
#include "matrix.h"
#include "vector.h"

class Model
{
 protected:
  float points[MAXPOINTS][3];
  int numOfPoints;
  int faces[MAXFACES][3];
  int numOfFaces;
  int numTransformations;

  Matrix myTranslations;
  Matrix myRotations;
  Matrix myScales;
  Matrix myShears;
  Matrix myTransformations[MAXTRANSFORMATIONS];
  
 public:
  Model();
  void addPoint(float x, float y, float z);
  void addFace(int p1, int p2, int p3);
  void addTranslation(float x, float y, float z);
  void addRotation(float x, float y, float z, float theta);
  void addScale(float x, float y, float z);
  void addShear(float sxy, float sxz, float syx, float syz, float szx, float szy);
  
  void makeTransformation();

  Matrix getTranslations();
  Matrix getRotations();
  Matrix getScales();
  Matrix getShears();
  Matrix getTransformation(int i);

  int getNumPoints();
  int getNumFaces();
  int getNumTransformations();

  int* getFace(int i);
  Vector getPoint(int i);
};

#endif
