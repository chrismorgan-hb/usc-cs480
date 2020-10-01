#Makefile for hw1
#CSCI 480 Sp2004
#Author: Christopher Morgan

CXX = g++
CXXFLAGS = -g -Wall
WOBJS = framebuffer.o matrix.o vector.o wireframe.o utility.o stack.o model.o camera.o renderer.o
WCC = framebuffer.cc matrix.cc vector.cc wireframe.cc utility.cc stack.cc model.cc camera.cc renderer.cc
WH = framebuffer.h matrix.h vector.h utility.h stack.h globals.h model.h camera.h renderer.h
OTHER = Makefile README

all: $(WOBJS)
	$(CXX) $(CXXFLAGS) -o wireframe $(WOBJS)

wireframe.o: wireframe.cc utility.o globals.h
	$(CXX) $(CXXFLAGS) -c wireframe.cc

framebuffer.o: framebuffer.cc framebuffer.h utility.o
	$(CXX) $(CXXFLAGS) -c framebuffer.cc

matrix.o: matrix.cc matrix.h
	$(CXX) $(CXXFLAGS) -c matrix.cc

vector.o: vector.cc vector.h matrix.o
	$(CXX) $(CXXFLAGS) -c vector.cc

utility.o: utility.cc utility.h
	$(CXX) $(CXXFLAGS) -c utility.cc

stack.o: stack.cc stack.h globals.h matrix.o
	$(CXX) $(CXXFLAGS) -c stack.cc

model.o: model.cc model.h globals.h
	$(CXX) $(CXXFLAGS) -c model.cc

camera.o: camera.cc camera.h
	$(CXX) $(CXXFLAGS) -c camera.cc

renderer.o: renderer.cc renderer.h framebuffer.o stack.o model.o camera.o
	$(CXX) $(CXXFLAGS) -c renderer.cc

submit:
	submit -user csci480 -tag HW1 $(WCC) $(WH) $(OTHER)
