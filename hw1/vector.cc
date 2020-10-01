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

//copy constructor
Vector::Vector(Vector& rhs)
{
  for(int i = 0; i < 4; i++)
    data[i] = rhs.getElement(i);
}

//destructor, required for copy constructor
Vector::~Vector()
{
}

float* Vector::getData()
{
  return data;
}

void Vector::setData(float* arr)
{
  for(int i = 0; i < 4; i++)
    data[i] = arr[i];
}

float Vector::getElement(int index)
{
  if(index < 0 || index > 3)
    exit(-1);
  
  else
    return data[index];
}

void Vector::setElement(int index, float value)
{
  if(index < 0 || index > 3)
    return;
  
  else
    data[index] = value;
}

void Vector::zero()
{
  for(int i = 0; i < 4; i++)
    data[i] = 0;

  data[3] = 1;
}

// returns this . rhs
float Vector::dot(Vector* rhs)
{
  float product = 0;
  for(int i = 0; i < 3; i++)
    product += (data[i])*(rhs->getElement(i));
  return product;
}

// returns this X rhs
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
