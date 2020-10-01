// globals.h
// global defines and constants
// Author: Christopher Morgan
// CSCI 480 Sp2004

#ifndef GLOBALS_H
#define GLOBALS_H

#include "matrix.h"

#define OCT1		   1
#define OCT2		   2
#define OCT3		   3
#define OCT4		   4
#define OCT5		   5 
#define OCT6		   6
#define OCT7		   7
#define OCT8		   8
#define HORIZONTAL	   9
#define VERTICAL	   10

#define STACKSIZE          20

#define MAXPOINTS          1000
#define MAXFACES           500
#define MAXTRANSFORMATIONS 20
#define MAXLIGHTS          8
#define MAXEDGELENGTH      1000
#define MAXMODELS          5

#define CAMERA             11
#define POSITION           12
#define ORIENTATION        13
#define NEAR               14
#define FAR                15
#define LEFT               16
#define RIGHT              17
#define TOP                18
#define BOTTOM             19

#define SEPARATOR          20
#define TRANSFORM          21
#define TRANSLATION        22
#define ROTATION           23
#define SCALEFACTOR        24
#define SHEARING           25
#define MATERIAL           26
#define AMBIENTCOLOR       27
#define DIFFUSECOLOR       28
#define SPECULARCOLOR      29
#define SHININESS          30

#define COORDINATE3        35
#define POINT              36
#define NORMAL             37
#define VECTOR             38

#define INDEXEDFACESET     40
#define COORDINDEX         41

#define POINTLIGHT         50
#define LOCATION           51
#define COLOR              52

#define COMMENT            60  

#define WIREFRAME          100
#define SHADED             101
#define TEXTURE            102
#define SHADED_TEXTURE     103

#define ZOOM               110
#define ROTATE             111
#define TRANSLATE          112

#endif
