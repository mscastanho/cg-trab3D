#ifndef INPUT_H_
#define INPUT_H_

#include <iostream>
#include <stdio.h>
#include <map>
#include <GL/glut.h>
#include "misc.h"

using namespace std;

string parseConfigFile (string filePath);

void parseSVGFile (string filePath);

GLuint png_texture_load(const char * file_name, int * width, int * height);

#endif /* INPUT_H_ */
