// vector.h
// class definition for Vector
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef VECTOR_H
#define VECTOR_H

#include "globals.h"
#include "matrix.h"

// only use 4x1 vectors
class Vector
{
 protected:
  float data[4];
 public:
  Vector();
  Vector(float a, float b, float c, float d);
  Vector(Vector& v);
  ~Vector();
  
  float* getData();
  void setData(float* arr);
  
  float getElement(int index);
  void setElement(int index, float value);
  
  void zero();
  
  float dot(Vector* rhs);
  Vector cross(Vector* rhs);
  void norm();
  
  Vector operator-(Vector& rhs);
  Vector operator+(Vector& rhs);
  Vector operator*(float& rhs);
  
  void xAxis();
  void yAxis();
  void zAxis();

  void homogenize();
  
  void print();
};
#endif
