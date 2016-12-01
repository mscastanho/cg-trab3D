#ifndef CAR_H_
#define CAR_H_

#include <GL/glut.h>
#include "misc.h"
#include "bullet.h"
#include <iostream>
#include <stdio.h>

using namespace std;

const float RADIUS_SIZE_RATIO = 0.65;
const float BODY_HEIGHT = 0.5;
const float BODY_WIDTH = 0.5;
const float BODY_LENGTH = 1;
const float CANON_HEIGHT = 0.2;
const float CANON_WIDTH = 0.1;
const float WHEEL_AXIS_WIDTH = 0.075;
const float WHEEL_AXIS_HEIGHT = WHEEL_AXIS_WIDTH;
const float WHEEL_WIDTH = 0.15;
const float WHEEL_RADIUS = 0.2;
const float WINDSHIELD_WIDTH = BODY_WIDTH;
const float WINDSHIELD_HEIGHT = 0.2;
const float EXHAUST_PIPE_WIDTH = 0.075;
const float EXHAUST_PIPE_HEIGHT = EXHAUST_PIPE_WIDTH;

class Car {

	Point position;
	float size;
  Color bodyColor;
  Color styleColor;
	bool moving;
	float radius;
	float trajRadius; //trajectory radius for enemies` movement
	float carAngle;
	float canonAngle;
	float canonAngleZ;
	float wheelAngle;
	float wheelSpinAngle;

  public:
  Car(Point pos, float radius, Color c, float cAng, float cnAng, float cnAngZ, float wAng);

	void setMoving(bool status);
	Point get_position();
	void set_position(Point pos);
	void inc_position(float dx, float dy);
	float get_trajRadius();
	void set_trajRadius(float r);
	float get_wAngle();
	void inc_wAngle(float da);
	float get_cAngle();
	void set_cAngle(float angle);
	void inc_cAngle(float da);
	float get_cnAngle();
	void inc_cnAngle(float da);
	float get_cnAngleZ();
	void inc_cnAngleZ(float da);
	void inc_spinAngle(float da);
	float get_size();
	float get_height();

	void draw ();

	Point getBulletInitPos ();

	bool outsideOf(Car* c) const;
	bool outsideOf(Circle* c) const;
	bool outsideOf(Bullet* b) const;
	bool insideOf(Circle* c) const;

	Point update(bool w, bool s, bool a, bool d, GLdouble timeDiff);

};

#endif /* CAR_H_ */
