#Makefile for hw2
#CSCI 480 Sp2004
#Author: Christopher Morgan

CXX = g++
CXXFLAGS = -g -Wall

SHOBJS = framebuffer.o matrix.o vector.o shaded.o utility.o stack.o model.o camera.o light.o renderer.o rgbcolor.o polygonedge.o

SHCC = framebuffer.cc matrix.cc vector.cc shaded.cc utility.cc stack.cc model.cc camera.cc renderer.cc light.cc rgbcolor.cc polygonedge.cc

SHH = framebuffer.h matrix.h vector.h utility.h stack.h globals.h model.h camera.h renderer.h light.h rgbcolor.h polygonedge.h

OTHER = Makefile README

all: $(SHOBJS)
	$(CXX) $(CXXFLAGS) -o shaded $(SHOBJS)

shaded.o: shaded.cc utility.o globals.h renderer.o
	$(CXX) $(CXXFLAGS) -c shaded.cc

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

polygonedge.o: polygonedge.cc polygonedge.h vector.o rgbcolor.o globals.h
	$(CXX) $(CXXFLAGS) -c polygonedge.cc

renderer.o: renderer.cc renderer.h framebuffer.o stack.o model.o camera.o light.o polygonedge.o
	$(CXX) $(CXXFLAGS) -c renderer.cc

rgbcolor.o: rgbcolor.cc rgbcolor.h
	$(CXX) $(CXXFLAGS) -c rgbcolor.cc
submit:
	submit -user csci480 -tag HW2 $(SHCC) $(SHH) $(OTHER)
