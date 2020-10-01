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

class Renderer
{
 protected:
  Framebuffer* framebuffer;
  Stack matrixStack;
  Model* theModelToRender;
  Camera myCamera;

 public:
  Renderer();
  ~Renderer();
  void allocate(int w, int h);
  void render();
  void dump();
  void parse(char* filename);
};

#endif
