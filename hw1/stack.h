// stack.h
// class definition for Stack
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef STACK_H
#define STACK_H

#include "matrix.h"
#include "globals.h"

class Stack
{
 protected:
  Matrix stk[STACKSIZE];
  int top;

 public:
  Stack();
  bool push(Matrix m);
  bool pop();
  Matrix getTop();
};

#endif
