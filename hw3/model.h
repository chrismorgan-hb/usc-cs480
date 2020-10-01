// model.h
// class definition for Model
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef MODEL_H
#define MODEL_H

#include "globals.h"
#include "matrix.h"
#include "vector.h"
#include "rgbcolor.h"

// abstraction of a object
// includes points, faces, normals, and colors
// also takes care of all types of transformations
class Model
{
 protected:
  float points[MAXPOINTS][3]; // object space
  Vector cameraSpacePoints[MAXPOINTS]; // camera space
  Vector windowSpacePoints[MAXPOINTS]; // window space
  int numOfPoints;
  int faces[MAXFACES][3];
  int numOfFaces;
  int numTransformations;
  Vector vertexNormals[MAXPOINTS]; // all spaces
  int numOfVertexNormals;
  RGBColor vertexColors[MAXPOINTS];
  Vector centerPoints[MAXFACES]; // camera space & window space
  Vector faceNormals[MAXFACES]; // camera space & window space
  RGBColor faceColors[MAXFACES];
  int numOfFaceNormals;

  Matrix myTranslations;
  Matrix myRotations;
  Matrix myScales;

  Matrix myTransformations[MAXTRANSFORMATIONS];
  Matrix myTotalRotations;

  Vector myAmbient;
  Vector myDiffuse;
  Vector mySpecular;
  float myShininess;
  
 public:
  Model();
  void addPoint(float x, float y, float z);
  void addFace(int p1, int p2, int p3);
  void addTranslation(float x, float y, float z);
  void addRotation(float x, float y, float z, float theta);
  void addScale(float x, float y, float z);
    
  void addVertexNormal(Vector v);
  void addFaceNormal(Vector v);
  
  void makeTransformation();

  Matrix getTranslations();
  Matrix getRotations();
  Matrix getScales();
  
  Matrix getTransformation(int i);
  Matrix getTotalRotations();

  int getNumPoints();
  int getNumFaces();
  int getNumTransformations();
  int getNumVertexNormals();
  int getNumFaceNormals();

  int* getFace(int i);
  Vector getPoint(int i);
  Vector getCameraSpacePoint(int i);
  Vector getWindowSpacePoint(int i);
  Vector getVertexNormal(int i);
  Vector getFaceNormal(int i);
  Vector getCenterPoint(int i);
  RGBColor getVertexColor(int i);
  RGBColor getFaceColor(int i);

  void setFaceNormal(int i, Vector v);
  void setVertexNormal(int i, Vector v);
  void setCameraSpacePoint(int i, Vector v);
  void setWindowSpacePoint(int i, Vector v);
  void setCenterPoint(int i, Vector v);
  void setVertexColor(int i, RGBColor c);
  void setFaceColor(int i, RGBColor c);

  void setAmbient(Vector c);
  void setDiffuse(Vector c);
  void setSpecular(Vector c);
  void setShininess(float f);
  
  Vector getAmbient();
  Vector getDiffuse();
  Vector getSpecular();
  float getShininess();
};

#endif
