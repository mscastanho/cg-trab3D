#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <cmath>
#include "circle.h"
#include "rect.h"
#include "misc.h"
#include "car.h"
#include "bullet.h"
#include "object.h"

extern float PLAYER_SPEED;
extern float ANGLE_SPEED;
extern Object* wheelOBJ;
extern Object* carOBJ;
extern Object* lamborghiniOBJ;
extern Object* lambWheelOBJ;
extern Object* cannonOBJ;

Car::Car(Point pos, float radius, Color c, float cAng, float cnAng, float cnAngZ, float wAng){
  this->position = pos;
  this->bodyColor = c;
  this->moving = false;
  this->radius = radius;
  this->trajRadius = 0;
  this->size = radius/RADIUS_SIZE_RATIO;
  this->carAngle = cAng;
  this->canonAngle = cnAng;
  this->canonAngleZ = cnAngZ;
  this->wheelAngle = wAng;
  this->wheelSpinAngle = 0;
}

void Car::setMoving(bool status){
  this->moving = status;
}

Point Car::get_position(){
	return this->position;
}

void Car::set_position(Point pos){
	this->position = pos;
}

float Car::get_trajRadius(){
  return this->trajRadius;
}

void Car::set_trajRadius(float r){
  this->trajRadius = r;
}

float Car::get_wAngle(){
	return this->wheelAngle;
}

void Car::inc_wAngle(float da){
	this->wheelAngle += da;
}

float Car::get_cAngle(){
	return this->carAngle;
}

void Car::set_cAngle(float angle){
	this->carAngle = angle;
}

void Car::inc_cAngle(float da){
	this->carAngle += da;
}

float Car::get_cnAngle(){
	return this->canonAngle;
}

void Car::inc_cnAngle(float da){

  if(this->canonAngle + da > -45 && this->canonAngle + da < 45)
	   this->canonAngle += da;
}

float Car::get_cnAngleZ(){
	return this->canonAngleZ;
}

void Car::inc_cnAngleZ(float da){

  if(this->canonAngleZ + da >= 0 && this->canonAngleZ + da <= 45)
	   this->canonAngleZ += da;
}

void Car::inc_position(float dx, float dy){
	this->position.x += dx;
	this->position.y += dy;
}

void Car::inc_spinAngle(float da){
  this->wheelSpinAngle -= da;
}

float Car::get_size(){
  return this->size;
}

float Car::get_height(){
  return this->size*(BODY_HEIGHT);
}

void Car::draw (){

  glPushMatrix();

  glTranslatef(this->position.x,this->position.y,0.5);
  glRotatef(this->carAngle,0,0,1.0);

  Color color;
  float L = this->size;

  // Main body properties

  float bHeight = BODY_HEIGHT*L;
  float bWidth = BODY_WIDTH*L;
  float bLength = BODY_LENGTH*L;
  glColor3f(0,0,1);
  glPushMatrix();
  //glScalef(BODY_WIDTH,BODY_LENGTH,BODY_HEIGHT);
  //glutSolidCube(L);
  float scaleRatio = this->radius/100;
  glScalef(scaleRatio,scaleRatio,scaleRatio);
  drawObject(carOBJ);
  //drawObject(lamborghiniOBJ);
  glPopMatrix();
  // ============


  // Wheel axes
  color = WHITE;
  float waWidth = WHEEL_AXIS_WIDTH*L;
  float waHeight = WHEEL_AXIS_HEIGHT*L;


  // Wheels
  color = GRAY;
  float wWidth = WHEEL_WIDTH*L;
  float wRadius = WHEEL_RADIUS*L;
  float axisMidY = 0.7*bHeight/2 + waHeight/2;

  //Front Wheels
  glPushMatrix();
    glTranslatef(-34.14*scaleRatio,17.5*scaleRatio,7.5*scaleRatio);
    glRotatef(this->wheelAngle,0,0,1.0);
    glRotatef(this->wheelSpinAngle,1.0,0,0);
    glScalef(0.75,0.75,0.75);
    drawObject(wheelOBJ);
  glPopMatrix();

  glPushMatrix();
    glTranslatef(34.14*scaleRatio,17.5*scaleRatio,7.5*scaleRatio);
    glRotatef(this->wheelAngle,0,0,1.0);
    glRotatef(this->wheelSpinAngle,1.0,0,0);
    glScalef(0.750,0.75,0.75);
    drawObject(wheelOBJ);
  glPopMatrix();

  //Back wheels
  glPushMatrix();
  glTranslatef(33.856*scaleRatio,-37.738*scaleRatio,10*scaleRatio);
  glRotatef(this->wheelAngle,0,0,1.0);
  glRotatef(this->wheelSpinAngle,1.0,0,0);
  drawObject(wheelOBJ);
  glPopMatrix();
  //drawRect(wWidth,wHeight,color,-(bWidth/2+waWidth+wWidth),-(axisMidY + wHeight/2));

  glPushMatrix();
    glTranslatef(-33.856*scaleRatio,-37.738*scaleRatio,10*scaleRatio);
    glRotatef(this->wheelAngle,0,0,1.0);
    glRotatef(this->wheelSpinAngle,1.0,0,0);
    drawObject(wheelOBJ);
  glPopMatrix();

  // Draw cannon
  glPushMatrix();
    glTranslatef(0,17.5*scaleRatio,20*scaleRatio);
    glRotatef(this->canonAngle,0,0,1.0);
    glRotatef(this->canonAngleZ,1.0,0,0);
    glScalef(0.5*scaleRatio,1.2*scaleRatio,0.5*scaleRatio);
    drawObject(cannonOBJ);
  glPopMatrix();

  glPopMatrix();
}

// This function returns the coordinates of a bullet leaving the canon
Point Car::getBulletInitPos (){

  Point pCanon = {0,0,0};

  Point pCanonAux = {0,0,0};

  pCanon.z = 10 + 10*sin((canonAngleZ*M_PI)/180); //MUDAR

  // Get new X and Y position with cannon Z angle
  float XYAngleRotated = canonAngle + carAngle;
  float xRotated = - cos((canonAngleZ)*M_PI/180) * cos((XYAngleRotated - 90)*M_PI/180);
  float yRotated = - cos((canonAngleZ)*M_PI/180) * sin((XYAngleRotated - 90)*M_PI/180);
  float zRotated = 1 + sin((canonAngleZ)*M_PI/180);

  // The difference between new and old position is calculated
  pCanonAux.x = CANON_HEIGHT*this->size*xRotated - CANON_HEIGHT*this->size*sin((180+XYAngleRotated)*M_PI/180);
  pCanonAux.y = CANON_HEIGHT*this->size*yRotated - CANON_HEIGHT*this->size*sin((90+XYAngleRotated)*M_PI/180);

  // Translate from the canon's end position
  Point canonEndPos = {0,CANON_HEIGHT*this->size};
  pCanon = translateFrom(pCanon,canonEndPos);

  // Rotate by the canon's rotation
  pCanon = rotateBy(pCanon,this->canonAngle);

  // Translate from canon position
  Point canonPos = {0,BODY_HEIGHT*this->size/2};
  pCanon = translateFrom(pCanon,canonPos);

  // Rotate by the car's rotation
  pCanon = rotateBy(pCanon,this->carAngle);

  // First translate from car position
  pCanon = translateFrom(pCanon,this->position);

  // Adds the difference between X and Y position
  pCanon.x += pCanonAux.x;
  pCanon.y += pCanonAux.y;

  return pCanon;
}

// This function calculates if a collision has occurred between this and another
// circle. If one circle is inside another, they have collided.
bool Car::outsideOf(Car* c) const{
	float x1,x2,y1,y2,r1,r2;

	x1 = this->position.x;
	y1 = this->position.y;
	r1 = this->radius;

	x2 = c->position.x;
	y2 = c->position.y;
	r2 = c->radius;



	float dist = sqrt(pow(x1-x2,2) + pow(y1-y2,2));

	if(dist >= r1 + r2)
		return true;
	else
		return false;
}

bool Car::outsideOf(Circle* c) const{
	float x1,x2,y1,y2,r1,r2;

	x1 = this->position.x;
	y1 = this->position.y;
	r1 = this->radius;

  Point ccenter = c->get_center();
  float cradius = c->get_radius();

	x2 = ccenter.x;
	y2 = ccenter.y;
	r2 = cradius;

	float dist = sqrt(pow(x1-x2,2) + pow(y1-y2,2));

	if(dist >= r1 + r2)
		return true;
	else
		return false;
}

bool Car::outsideOf(Bullet* b) const{
	float x1,x2,y1,y2,r1,r2;

	x1 = this->position.x;
	y1 = this->position.y;
	r1 = this->radius;

  Point bcenter = b->get_position();
  float bradius = BULLET_RADIUS;

	x2 = bcenter.x;
	y2 = bcenter.y;
	r2 = bradius;

	float dist = sqrt(pow(x1-x2,2) + pow(y1-y2,2));

	if(dist >= r1 + r2)
		return true;
	else
		return false;
}

// This function calculates if a collision has occurred between this and another
// circle. In this case a collision only occurs if this circle is partially or
// completely outside of the circle c
bool Car::insideOf(Circle* c) const{
	float x1,x2,y1,y2,r1,r2;

	x1 = this->position.x;
	y1 = this->position.y;
	r1 = this->radius;

  Point ccenter = c->get_center();
  float cradius = c->get_radius();

	x2 = ccenter.x;
	y2 = ccenter.y;
	r2 = cradius;

	float dist = sqrt(pow(x1-x2,2) + pow(y1-y2,2));

	if(dist <= r2 - r1)
		return true;
	else
		return false;
}

//Parameters:
// w,s,a,d: tell if the corresponding keys are pressed in the keyboard
// timeDiff: time elapsed since last function call
// Returns a Point with x and y deltas that should be applied to the car
Point Car::update(bool w, bool s, bool a, bool d, GLdouble timeDiff) {

	float playerSpeed = PLAYER_SPEED*timeDiff;
	float dx=0,dy=0;
	float wheelAngle = this->wheelAngle;
	float playerAngle = this->carAngle;

	if( d && wheelAngle > -45+ANGLE_SPEED)
		this->wheelAngle -= ANGLE_SPEED;

	if( a && wheelAngle < 45-ANGLE_SPEED)
		this->wheelAngle += ANGLE_SPEED;

	if( w ){
		if(wheelAngle > 0){
      this->wheelAngle -= 1;
      this->carAngle += 1;
		}else if(wheelAngle < 0){
      this->wheelAngle += 1;
      this->carAngle -= 1;
		}

		dy = playerSpeed*cos(M_PI*playerAngle/180.0);
		dx = -playerSpeed*sin(M_PI*playerAngle/180.0);

	}

	if( s ){
		if(wheelAngle > 0){
      this->wheelAngle -= 1;
      this->carAngle -= 1;
		}else if(wheelAngle < 0){
      this->wheelAngle += 1;
      this->carAngle += 1;
		}

		dy = - playerSpeed*cos(M_PI*playerAngle/180.0);
		dx = +playerSpeed*sin(M_PI*playerAngle/180.0);

	}

  //float dt = sqrt(dx*dx + dy*dy);
  //float percentSpin =

	if(dy == 0.0 && dy == 0.0)
	 this->moving = false;
	else
	 this->moving = true;

  if(this->moving)
    this->wheelSpinAngle -= 5;

  Point p = {dx,dy};

  return p;
}
