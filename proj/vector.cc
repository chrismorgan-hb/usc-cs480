// vector.cc
// class implementation for Vector
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "vector.h"
#include <iostream>
using namespace std;

//constructor
//*this = (0, 0, 0, 1)
Vector::Vector()
{
  zero();
  data[3] = 1;
}

//constructor
//*this = (a, b, c, d)
Vector::Vector(float a, float b, float c, float d)
{
  data[0] = a;
  data[1] = b;
  data[2] = c;
  data[3] = d;
}

//copy constructor
//performs deep copy of data array
Vector::Vector(Vector& rhs)
{
  for(int i = 0; i < 4; i++)
    data[i] = rhs.getElement(i);
}

//destructor, required for copy constructor
Vector::~Vector()
{
}

//getData
//returns a pointer to the data array
float* Vector::getData()
{
  return data;
}

//setData
//sets the whole data array at once
void Vector::setData(float* arr)
{
  for(int i = 0; i < 4; i++)
    data[i] = arr[i];
}

// getElement
// returns data[index]
float Vector::getElement(int index)
{
  //bounds check
  if(index < 0 || index > 3)
    exit(-1);
  
  else
    return data[index];
}

// setElement
// data[index] = value
void Vector::setElement(int index, float value)
{
  //bounds check
  if(index < 0 || index > 3)
    return;
  
  else
    data[index] = value;
}

// zero
// *this = (0,0,0,1)
void Vector::zero()
{
  for(int i = 0; i < 4; i++)
    data[i] = 0;

  data[3] = 1;
}

// dot
// returns *this . rhs
float Vector::dot(Vector* rhs)
{
  float product = 0;
  for(int i = 0; i < 3; i++)
    product += (data[i])*(rhs->getElement(i));
  return product;
}

// cross
// returns *this X rhs
Vector Vector::cross(Vector* rhs)
{
  Vector result;
  result.zero();
  float element = 0;
  
  // x-component
  element = (this->getElement(1) * (rhs->getElement(2))) 
             - (this->getElement(2) * (rhs->getElement(1)));
  result.setElement(0, element);

  // y-component
  element = -((this->getElement(0) * (rhs->getElement(2))) 
	     - (this->getElement(2) * (rhs->getElement(0))));
  result.setElement(1, element);

  // z-component
  element = (this->getElement(0) * (rhs->getElement(1))) 
             - (this->getElement(1) * (rhs->getElement(0)));
  result.setElement(2, element);

  return result;
}

//norm
//normalizes *this
void Vector::norm()
{
  float mag = sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
  data[0] /= mag;
  data[1] /= mag;
  data[2] /= mag;
}

// subtraction operator
// returns *this - rhs
Vector Vector::operator-(Vector& rhs)
{
  Vector result;
  result.zero();
  float element = 0;

  //x-components
  element = this->getElement(0) - rhs.getElement(0);
  result.setElement(0, element);

  //y-components
  element = this->getElement(1) - rhs.getElement(1);
  result.setElement(1, element);
  
  //z-components
  element = this->getElement(2) - rhs.getElement(2);
  result.setElement(2, element);

  return result;
}

// addition operator
// returns *this + rhs
Vector Vector::operator+(Vector& rhs)
{
  Vector result;
  result.zero();
  float element = 0;

  //x-components
  element = this->getElement(0) + rhs.getElement(0);
  result.setElement(0, element);

  //y-components
  element = this->getElement(1) + rhs.getElement(1);
  result.setElement(1, element);
  
  //z-components
  element = this->getElement(2) + rhs.getElement(2);
  result.setElement(2, element);

  return result;
}

// scalar multiplication operator
// returns *this * rhs
Vector Vector::operator*(float& rhs)
{
  Vector result;
  result.zero();
  float element = 0;

  //x-component
  element = this->getElement(0) * rhs;
  result.setElement(0, element);

  //y-component
  element = this->getElement(1) * rhs;
  result.setElement(1, element);
  
  //z-component
  element = this->getElement(2) * rhs;
  result.setElement(2, element);

  return result;
}

//xAxis
//sets *this to be unit vector in x direction
void Vector::xAxis()
{
  zero();
  data[0] = 1;
}

//yAxis
//sets *this to be unit vector in y direction
void Vector::yAxis()
{
  zero();
  data[1] = 1;
}

//zAxis
//sets *this to be unit vector in z direction
void Vector::zAxis()
{
  zero();
  data[2] = 1;
}

//homogenize
// x=x/w y=y/w z=z/w
void Vector::homogenize()
{
  data[0] /= data[3];
  data[1] /= data[3];
  data[2] /= data[3];
  data[3] /= data[3];
}

//print
//prints space separated vector data
//useful for debugging
void Vector::print()
{
  for(int i = 0; i < 4; i++)
    cout << data[i] << "  ";

  cout << endl;
}
