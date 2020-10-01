// camera.h
// class definition for Camera
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
protected:
	float position[3];
	float orientation[4];
	float near;
	float far;
	float left;
	float right;
	float top;
	float bottom;

public:
	Camera();
	float* getPosition();
	float* getOrientation();
	float getNear();
	float getFar();
	float getLeft();
	float getRight();
	float getTop();
	float getBottom();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z, float theta);
	void setNear(float n);
	void setFar(float f);
	void setLeft(float l);
	void setRight(float r);
	void setTop(float t);
	void setBottom(float b);
};

#endif
