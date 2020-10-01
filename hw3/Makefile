#Makefile for HW3

CXX = g++
CXXFLAGS = -lGL -lGLU -lglut -lXmu -lX11 -I/usr/usc/glut/3.5/include/ -L/usr/usc/glut/3.5/lib/ -g -Wall
OGLOBJS = OglRenderer.o renderer.o model.o camera.o framebuffer.o light.o matrix.o rgbcolor.o stack.o utility.o vector.o
OGLCC = OglRenderer.cc framebuffer.cc matrix.cc vector.cc utility.cc stack.cc model.cc camera.cc renderer.cc light.cc rgbcolor.cc
OGLH = framebuffer.h matrix.h vector.h utility.h stack.h globals.h model.h camera.h renderer.h light.h rgbcolor.h
OGLOTHER = Makefile README 

all: $(OGLOBJS)
	$(CXX) -o OglRenderer $(OGLOBJS) $(CXXFLAGS)

OglRenderer.o: OglRenderer.cc 
	$(CXX) $(CXXFLAGS) -c OglRenderer.cc

framebuffer.o: framebuffer.cc framebuffer.h utility.o rgbcolor.o globals.h
	$(CXX) $(CXXFLAGS) -c framebuffer.cc

matrix.o: matrix.cc matrix.h
	$(CXX) $(CXXFLAGS) -c matrix.cc

vector.o: vector.cc vector.h matrix.o
	$(CXX) $(CXXFLAGS) -c vector.cc

utility.o: utility.cc utility.h matrix.o vector.o rgbcolor.o globals.h
	$(CXX) $(CXXFLAGS) -c utility.cc

stack.o: stack.cc stack.h globals.h matrix.o
	$(CXX) $(CXXFLAGS) -c stack.cc

model.o: model.cc model.h globals.h matrix.o vector.o rgbcolor.o
	$(CXX) $(CXXFLAGS) -c model.cc

camera.o: camera.cc camera.h
	$(CXX) $(CXXFLAGS) -c camera.cc

light.o: light.cc light.h vector.o rgbcolor.o
	$(CXX) $(CXXFLAGS) -c light.cc

renderer.o: renderer.cc renderer.h framebuffer.o stack.o model.o camera.o light.o
	$(CXX) $(CXXFLAGS) -c renderer.cc

rgbcolor.o: rgbcolor.cc rgbcolor.h
	$(CXX) $(CXXFLAGS) -c rgbcolor.cc

submit:
	submit -user csci480 -tag HW3 $(OGLCC) $(OGLH) $(OGLOTHER)