// renderer.h
// class definition for Renderer
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef RENDERER_H
#define RENDERER_H

#include "framebuffer.h"
#include "stack.h"
#include "model.h"
#include "camera.h"
#include "light.h"

// overall class
// merges other classes to allow for a simple interface
class Renderer
{
 protected:
  Framebuffer* framebuffer;
  Stack matrixStack; // converts from object space to window space
  Stack objToCamera; // converts from object space to camera space
  Stack worldToCamera; // converts from world space to camera space
  Model* modelsToRender[MAXMODELS];
  Camera myCamera;
  Light myLights[MAXLIGHTS];
  unsigned int numLights;
  int shadingType;
  int displayType;
  int numModels;
  char* texFile;
  int texWidth;
  int texHeight;
  unsigned char* texArr;

 public:
  Renderer(char* t);
  ~Renderer();
  void allocate(int w, int h);
  void render();
  void parse(char* filename);
  void scanline(int face);
  
  int getNumModels();
  Model* getModelToRender(int i);
  int addModel();

  void setDisplayType(int d);
  int getDisplayType();
  Camera* getCamera();
  void setWidth(int w);
  void setHeight(int h);
  void parseTex();
  int getTexWidth();
  int getTexHeight();
  unsigned char* getTexArr();
  void setTexFile(char* t);
};

#endif
