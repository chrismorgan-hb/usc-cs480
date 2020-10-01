// matrix.h
// class definition for Matrix
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

class Vector;

// only use 4x4 matricies
class Matrix
{
 protected:
  float data[4][4];
  float oned[16]; //1d representation cuz OpenGL hates me

 public:
  Matrix();
  Matrix(Matrix& rhs);
  ~Matrix();
  
  float** getData();
  float getElement(int row, int column);
  void setData(float[][4]);
  void setElement(int row, int column, float value);
  
  Matrix operator* (Matrix& rhs);
  Vector operator* (Vector& rhs);
  Matrix operator* (float& rhs);
  
  void translation(float tx, float ty, float tz);
  void rotation(float x, float y, float z, float theta);
  void scale(float sx, float sy, float sz);
  
  void identity(); // sets matrix to the identity matrix
  void zero(); // sets all elements to zero
  float invert(); // inverts the matrix, returns determinant
  void transpose(); // transposes the matrix
  float* transpose1d(); // transposes the matrix and puts it in a 1d array to
                        // play nice w/ OpenGL
  
  void print();
};
#endif
