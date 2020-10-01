// matrix.cc
// class implementation for Matrix
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "matrix.h"
#include <iostream>
using namespace std;
#include <math.h>

//default constructor
//sets all elements to zero
Matrix::Matrix()
{
  zero();
}

//copy constructor
Matrix::Matrix(Matrix& rhs)
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      this->setElement(i, j, rhs.data[i][j]);
}

//destructor required because of copy constructor
Matrix::~Matrix()
{
}

// getData
// returns a pointer to the data array
float** Matrix::getData()
{
  return (float**)(data);
}

// getElement
// returns a copy of data[row][column]
float Matrix::getElement(int row, int column)
{
  // check for out of bounds
  if(row > 3 || column > 3 || column < 0 || row < 0)
    exit(-1);
  
  else
    return data[row][column];
}

// setData
// sets entire data array at once
void Matrix::setData(float arr[][4])
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      data[i][j] = arr[i][j];
}

// setElement
// data[row][column] = value
void Matrix::setElement(int row, int column, float value)
{
  // bounds check
  if(row > 3 || column > 3 || row < 0 || column < 0)
    return;
  
  else
    data[row][column] = value;
}

//Matrix * Matrix operation
Matrix Matrix::operator*(Matrix& rhs)
{
  Matrix result;
  result.zero();
  
  float element = 0;
  
  for(int row = 0; row < 4; row++) // for each row in result
    {
      for(int col = 0; col < 4; col++) // for each column of each row in result
	{
	  element = 0;
	  for(int i = 0; i < 4; i++)
	    {
	      element += data[row][i] * rhs.getElement(i, col);
	    }
	  result.setElement(row, col, element);
	}
    }
  
  return result;
}

//Matrix * Vector operation
Vector Matrix::operator*(Vector& rhs)
{
  Vector result;
  result.zero();

  float element = 0;
  
  for(int row = 0; row < 4; row++) // for each row in matrix
    {
      element = 0;
      for(int i = 0; i < 4; i++) // for each column
	{
	  element += data[row][i] * rhs.getElement(i);
	}
      result.setElement(row, element);
    }
  return result;
}

//Matrix * scalar operation
// multiply each element by rhs
Matrix Matrix::operator*(float& rhs)
{
  Matrix result;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      result.setElement(i,j, result.getElement(i,j) * rhs);
  
  return result;
}

//creates a translation matrix out of *this
void Matrix::translation(float tx, float ty, float tz)
{
  this->identity();
  this->setElement(0,3,tx);
  this->setElement(1,3,ty);
  this->setElement(2,3,tz);
}

//creates a rotation about x,y,z for theta degrees and assigns to *this
void Matrix::rotation(float x, float y, float z, float theta)
{
  // create rotation matrix for rotating theta degrees about vector (x,y,z)
  // IMPORTANT: COS AND SIN functions don't take degrees, they need radians
  theta *= 3.14/180;
  
  int xi = (int)x;
  int yi = (int)y;
  int zi = (int)z;

  if(xi == 0 && yi == 0) // rotate about z-axis
    {
      this->identity();
      data[0][0] = cos(theta);
      data[0][1] = -sin(theta);
      data[1][0] = sin(theta);
      data[1][1] = cos(theta);
    }

  else if(xi == 0 && zi == 0) // rotate about y-axis
    {
      this->identity();
      data[0][0] = cos(theta);
      data[0][2] = sin(theta);
      data[2][0] = -sin(theta);
      data[2][2] = cos(theta);
    }

  else if(yi == 0 && zi == 0) // rotate about x-axis
    {
      this->identity();
      data[1][1] = cos(theta);
      data[1][2] = -sin(theta);
      data[2][1] = sin(theta);
      data[2][2] = cos(theta);
    }

  else // arbitrary axis
    {
      //first, rotate into xz
      //then rotate onto z
      //rotate theta degrees about z
      //reverse rotation in xz plane
      //reverse rotation from xz plane

      float mag = sqrt(pow(x,2) + pow(y,2) + pow(z,2));
      float a = x/mag;
      float b = y/mag;
      float c = z/mag;
      float d = sqrt(pow(b,2) + pow(c,2));
      
      // inverse alpha rotation matrix
      this->identity();
      this->data[1][1] = c/d;
      this->data[1][2] = b/d;
      this->data[2][1] = -b/d;
      this->data[2][2] = c/d;
      
      // inverse beta rotation matrix
      Matrix invBeta;
      invBeta.identity();
      invBeta.setElement(0,0,d);
      invBeta.setElement(0,2,a);
      invBeta.setElement(2,0,-a);
      invBeta.setElement(2,2,d);
      
      // theta rotation matrix
      Matrix thetaM;
      thetaM.identity();
      thetaM.setElement(0,0,cos(theta));
      thetaM.setElement(0,1,-sin(theta));
      thetaM.setElement(1,0,sin(theta));
      thetaM.setElement(1,1,cos(theta));
      
      // beta rotation matrix
      Matrix beta;
      beta.identity();
      beta.setElement(0,0,d);
      beta.setElement(0,2,-a);
      beta.setElement(2,0,a);
      beta.setElement(2,2,d);
      
      // alpha rotation matrix
      Matrix alpha;
      alpha.identity();
      alpha.setElement(1,1,c/d);
      alpha.setElement(1,2,-b/d);
      alpha.setElement(2,1,b/d);
      alpha.setElement(2,2,c/d);
      
      *this = (*this) * invBeta;
      *this = (*this) * thetaM;
      *this = (*this) * beta;
      *this = (*this) * alpha;
    }
}

//create scale matrix from x,y,z and assign to *this
void Matrix::scale(float x, float y, float z)
{
  this->identity();
  this->setElement(0,0,x);
  this->setElement(1,1,y);
  this->setElement(2,2,z);
}

//*this = identity matrix
void Matrix::identity()
{
  this->zero();
  for(int i = 0, j = 0; i < 4; i++, j++)
    data[i][j] = 1;
}

//*this = Matrix[0]
void Matrix::zero()
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      data[i][j] = 0;
}

//*this = (*this)^-1
float Matrix::invert()
{
  Matrix cof;
  float det = 0;

  cof.setElement(0,0, data[1][1]*data[2][2]*data[3][3] + data[1][2]*data[2][3]*data[3][1] +
		      data[1][3]*data[2][1]*data[3][2] - data[1][1]*data[2][3]*data[3][2] -
		      data[1][3]*data[2][2]*data[3][1] - data[1][2]*data[2][1]*data[3][3]);

  cof.setElement(1,0, -(data[0][1]*data[2][2]*data[3][3] + data[0][2]*data[2][3]*data[3][1] +
			data[0][3]*data[2][1]*data[3][2] - data[0][1]*data[2][3]*data[3][2] -
			data[0][3]*data[2][2]*data[3][1] - data[0][2]*data[2][1]*data[3][3]));

  cof.setElement(2,0, data[0][1]*data[1][2]*data[3][3] + data[0][2]*data[1][3]*data[3][1] +
		      data[0][3]*data[1][1]*data[3][2] - data[0][1]*data[1][3]*data[3][2] -
		      data[0][3]*data[1][2]*data[3][1] - data[0][2]*data[1][1]*data[3][3]);

  cof.setElement(3,0, -(data[0][1]*data[1][2]*data[2][3] + data[0][2]*data[1][3]*data[2][1] +
			data[0][3]*data[1][1]*data[2][2] - data[0][1]*data[1][3]*data[2][2] -
			data[0][3]*data[1][2]*data[2][1] - data[0][2]*data[1][1]*data[2][3]));

  cof.setElement(0,1, -(data[1][0]*data[2][2]*data[3][3] + data[1][2]*data[2][3]*data[3][0] +
			data[1][3]*data[2][0]*data[3][2] - data[1][0]*data[2][3]*data[3][2] -
			data[1][3]*data[2][2]*data[3][0] - data[1][2]*data[2][0]*data[3][3]));

  cof.setElement(1,1, data[0][0]*data[2][2]*data[3][3] + data[0][2]*data[2][3]*data[3][0] +
		      data[0][3]*data[2][0]*data[3][2] - data[0][0]*data[2][3]*data[3][2] -
		      data[0][3]*data[2][2]*data[3][0] - data[0][2]*data[2][0]*data[3][3]);

  cof.setElement(2,1, -(data[0][0]*data[1][2]*data[3][3] + data[0][2]*data[1][3]*data[3][0] +
		        data[0][3]*data[1][0]*data[3][2] - data[0][0]*data[1][3]*data[3][2] -
			data[0][3]*data[1][2]*data[3][0] - data[0][2]*data[1][0]*data[3][3]));
  
  cof.setElement(3,1, data[0][0]*data[1][2]*data[2][3] + data[0][2]*data[1][3]*data[2][0] +
		      data[0][3]*data[1][0]*data[2][2] - data[0][0]*data[1][3]*data[2][2] -
		      data[0][3]*data[1][2]*data[2][0] - data[0][2]*data[1][0]*data[2][3]);

  cof.setElement(0,2, data[1][0]*data[2][1]*data[3][3] + data[1][1]*data[2][3]*data[3][0] +
		      data[1][3]*data[2][0]*data[3][1] - data[1][0]*data[2][3]*data[3][1] -
		      data[1][3]*data[2][1]*data[3][0] - data[1][1]*data[2][0]*data[3][3]);

  cof.setElement(1,2, -(data[0][0]*data[2][1]*data[3][3] + data[0][1]*data[2][3]*data[3][0] +
			data[0][3]*data[2][0]*data[3][1] - data[0][0]*data[2][3]*data[3][1] -
			data[0][3]*data[2][1]*data[3][0] - data[0][1]*data[2][0]*data[3][3]));

  cof.setElement(2,2, data[0][0]*data[1][1]*data[3][3] + data[0][1]*data[1][3]*data[3][0] +
		      data[0][3]*data[1][0]*data[3][1] - data[0][0]*data[1][3]*data[3][1] -
		      data[0][3]*data[1][1]*data[3][0] - data[0][1]*data[1][0]*data[3][3]);

  cof.setElement(3,2, -(data[0][0]*data[1][1]*data[2][3] + data[0][1]*data[1][3]*data[2][0] +
			data[0][3]*data[1][0]*data[2][1] - data[0][0]*data[1][3]*data[2][1] -
			data[0][3]*data[1][1]*data[2][0] - data[0][1]*data[1][0]*data[2][3]));

  cof.setElement(0,3, -(data[1][0]*data[2][1]*data[3][2] + data[1][1]*data[2][2]*data[3][0] +
			data[1][2]*data[2][0]*data[3][1] - data[1][0]*data[2][2]*data[3][1] -
			data[1][2]*data[2][1]*data[3][0] - data[1][1]*data[2][0]*data[3][2]));

  cof.setElement(1,3, data[0][0]*data[2][1]*data[3][2] + data[0][1]*data[2][2]*data[3][0] +
		      data[0][2]*data[2][0]*data[3][1] - data[0][0]*data[2][2]*data[3][1] -
		      data[0][2]*data[2][1]*data[3][0] - data[0][1]*data[2][0]*data[3][2]);

  cof.setElement(2,3, -(data[0][0]*data[1][1]*data[3][2] + data[0][1]*data[1][2]*data[3][0] +
			data[0][2]*data[1][0]*data[3][1] - data[0][0]*data[1][2]*data[3][1] -
			data[0][2]*data[1][1]*data[3][0] - data[0][1]*data[1][0]*data[3][2]));
  
  cof.setElement(3,3, data[0][0]*data[1][1]*data[2][2] + data[0][1]*data[1][2]*data[2][0] +
		      data[0][2]*data[1][0]*data[2][1] - data[0][0]*data[1][2]*data[2][1] -
		      data[0][2]*data[1][1]*data[2][0] - data[0][1]*data[1][0]*data[2][2]);

  for(int i = 0; i < 4; i++)
    det += (cof.getElement(i,0)) * (this->getElement(i,0));

  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      this->setElement(j,i, (1/det) * cof.getElement(i,j));
  return det;
}

// transpose
// data[i][j] = data[j][i]
void Matrix::transpose()
{
  float temp[4][4];

  for(int k = 0; k < 4; k++)
    for(int m = 0; m < 4; m++)
      temp[k][m] = data[k][m];
  
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      data[i][j] = temp[j][i];
}

// transpose1d
// transposes matrix, and then copies to 1d array row by row for OpenGL's pleasure
float* Matrix::transpose1d()
{
  float temp[4][4];
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      temp[i][j] = data[i][j];

  int counter = 0;
  for(int k = 0; k < 4; k++)
    for(int m = 0; m < 4; m++)
      {
	oned[counter] = temp[m][k];
	counter++;
      }
  return oned;
}

//prints tab and endline separated matrix representation for debugging
void Matrix::print()
{
  for(int i = 0; i < 4; i++)
    {
      for(int j = 0; j < 4; j++)
	  cout << data[i][j] << "\t";
      cout << endl;
    }
  cout << endl;
}
