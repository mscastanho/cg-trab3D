#ifndef MISC_H_
#define MISC_H_

typedef struct tColor {
	float r;
	float g;
	float b;
} Color;

const Color BLACK = {0,0,0};
const Color BLUE = {0,0,1};
const Color GREEN = {0,1,0};
const Color CYAN = {0,1,1};
const Color RED = {1,0,0};
const Color MAGENTA = {1,0,1};
const Color YELLOW = {1,1,0};
const Color WHITE = {1,1,1};
const Color GRAY = {0.5,0.5,0.5};
const Color DARK_GRAY = {0.2,0.2,0.2};
const Color LIGHT_GRAY = {0.9,0.9,0.9};

typedef struct tPoint {
	float x;
	float y;
	float z;
} Point;


void printPoint(Point p);

// Angle must be in degrees
Point rotateBy(Point p, float angle);

// This assumes that the coordinates of oldOrigin are expressed in terms
// of the new coordinate system
Point translateFrom(Point p, Point oldOrigin);

void solidCylinder(float radius);

#endif /* MISC_H_ */
