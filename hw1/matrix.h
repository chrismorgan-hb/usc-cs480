#ifndef MATRIX_H
#define MATRIX_H
// matrix.h
// class definition for Matrix
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "globals.h"
#include "vector.h"

class Vector;

// only use 4x4 matricies
class Matrix
{
 protected:
  float data[4][4];
 public:
  Matrix();
  Matrix(Matrix& rhs); // copy constructor, if needed
  ~Matrix();
  
  float** getData();
  float getElement(int row, int column);
  void setData(float[][4]);
  void setElement(int row, int column, float value);
  
  Matrix operator* (Matrix& rhs);
  Vector operator* (Vector& rhs);
  
  void translation(float tx, float ty, float tz);
  void rotation(float x, float y, float z, float theta);
  void scale(float sx, float sy, float sz);
  void shear(float sxy, float sxz, float syx, float syz, float szx, float szy);
  
  void identity(); // sets matrix to the identity matrix
  void zero(); // sets all elements to zero
  
  void print();
};
#endif
