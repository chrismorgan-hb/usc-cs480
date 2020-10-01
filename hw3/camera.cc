// camera.cc
// class implementation for Camera
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "camera.h"

//default constructor
//initializes all camera parameters to defaults
Camera::Camera()
{
  setPosition(0,1,1);
  setOrientation(0,0,1,0);
  near = 1;
  far = 10;
  left = -1;
  right = 1;
  top = 1;
  bottom = -1;
}

float* Camera::getPosition()
{
  return position;
}

float* Camera::getOrientation()
{
  return orientation;
}

float Camera::getNear()
{
  return near;
}

float Camera::getFar()
{
  return far;
}

float Camera::getLeft()
{
  return left;
}

float Camera::getRight()
{
  return right;
}

float Camera::getTop()
{
  return top;
}

float Camera::getBottom()
{
  return bottom;
}

void Camera::setPosition(float x, float y, float z)
{
  position[0] = x;
  position[1] = y;
  position[2] = z;
}

void Camera::setOrientation(float x, float y, float z, float theta)
{
  orientation[0] = x;
  orientation[1] = y;
  orientation[2] = z;
  orientation[3] = theta;
}

void Camera::setNear(float n)
{
  near = n;
}

void Camera::setFar(float f)
{
  far = f;
}

void Camera::setLeft(float l)
{
  left = l;
}

void Camera::setRight(float r)
{
  right = r;
}

void Camera::setTop(float t)
{
  top = t;
}

void Camera::setBottom(float b)
{
  bottom = b;
}
