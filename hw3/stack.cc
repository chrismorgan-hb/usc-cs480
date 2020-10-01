// stack.cc
// class implementation for Stack
// Author: Christopher Morgan
// CSCI 480 Sp2004

#include "stack.h"
#include "matrix.h"
#include "globals.h"

//constructor
//bottom of stack will always be identity
Stack::Stack()
{
  stk[0].identity();
  top = 0;
}

//push
// multiplies top of stack w/ matrix m
// puts result on top of stack
bool Stack::push(Matrix m)
{
  stk[top+1] = stk[top] * m;
  top++;
  
  if(top > STACKSIZE)
    return false;

  return true;
}

//pop
//moves one slot down the stack
bool Stack::pop()
{
  top--;
  if(top < 0)
    top = 0;
  return true;
}

//getTop
//returns the matrix at the top of the stack
Matrix Stack::getTop()
{
  return stk[top];
}
