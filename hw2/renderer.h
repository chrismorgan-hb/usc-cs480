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
  Model* theModelToRender;
  Camera myCamera;
  Light myLights[MAXLIGHTS];
  unsigned int numLights;
  int shadingType;

 public:
  Renderer(int s);
  ~Renderer();
  void allocate(int w, int h);
  void render();
  void dump();
  void parse(char* filename);
  void scanline(int face);
  
  void scanFlat(int face);
  void scanGouraud(int face);
  void scanPhong(int face);
};

#endif
