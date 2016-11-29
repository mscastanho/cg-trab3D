#ifndef RECT_H_
#define RECT_H_

#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include "misc.h"

using namespace std;

// This class represents a generic Rectrilateral
class Rect {

  // The vertex corresponds to the lower-left one
	Point vertex;
	float width;
	float height;
  Color color;

	public:
	Rect(float x, float y, float width, float height, Color c);
	void draw (GLuint texture);
	void draw (Point pos);
	Point get_vertex() const;
	void set_vertex(Point v1);
	void print() const;
	friend ostream& operator<<(ostream& output, const Rect& c);
};

void drawRect (float width, float height, Color c);
void drawRect (float width, float height, Color c, float xTrans, float yTrans);
void drawRect3D (float width, float height, Point lowerLeftCorner, GLuint texture);

#endif /* RECT_H_ */
