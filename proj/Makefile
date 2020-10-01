#Makefile for proj

CXX = g++
CXXFLAGS = -lGL -lGLU -lglut -lXmu -lX11 -I/usr/usc/glut/3.5/include/ -L/usr/usc/glut/3.5/lib/ -g -Wall
SSOBJS = SolarSystem.o planet.o matrix.o vector.o moon.o
SSCC = SolarSystem.cc planet.cc matrix.cc vector.cc moon.cc
SSH = planet.h globals.h matrix.h vector.h moon.h
SSOTHER = Makefile README planets.dat moons.dat 

all: $(SSOBJS)
	$(CXX) -o SolarSystem $(SSOBJS) $(CXXFLAGS)

SolarSystem.o: SolarSystem.cc planet.o globals.h moon.o matrix.o vector.o
	$(CXX) $(CXXFLAGS) -c SolarSystem.cc

planet.o: planet.cc planet.h moon.o
	$(CXX) $(CXXFLAGS) -c planet.cc

matrix.o: matrix.cc matrix.h vector.o
	$(CXX) $(CXXFLAGS) -c matrix.cc

vector.o: vector.cc vector.h
	$(CXX) $(CXXFLAGS) -c vector.cc

moon.o: moon.cc moon.h
	$(CXX) $(CXXFLAGS) -c moon.cc

submit:
	submit -user csci480 -tag PROJECT $(SSCC) $(SSH) $(SSOTHER)