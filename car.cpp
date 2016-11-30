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

extern float PLAYER_SPEED;
extern float ANGLE_SPEED;

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

float Car::get_size(){
  return this->size;
}

float Car::get_height(){
  return this->size*(BODY_HEIGHT);
}

void Car::draw (){

  glPushMatrix();

  glTranslatef(this->position.x,this->position.y,this->position.z);
  glRotatef(this->carAngle,0,0,1.0);

  GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
  GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
  GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1};
  GLfloat mat_shininess[] = { 100.0 };
  glColor3f(0,0,1);

  glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
  glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);


  Color color;
  float L = this->size;

  // Main body properties

  float bHeight = BODY_HEIGHT*L;
  float bWidth = BODY_WIDTH*L;
  float bLength = BODY_LENGTH*L;
  glColor3f(0,0,1);
  glPushMatrix();
  glScalef(BODY_WIDTH,BODY_LENGTH,BODY_HEIGHT);
  glutSolidCube(L);
  glPopMatrix();
  // ============


  // Wheel axes
  color = WHITE;
  float waWidth = WHEEL_AXIS_WIDTH*L;
  float waHeight = WHEEL_AXIS_HEIGHT*L;
/*  drawRect(waWidth,waHeight,color,bWidth/2,0.7*bHeight/2);
  drawRect(waWidth,waHeight,color,-(bWidth/2+waWidth),0.7*bHeight/2);
  drawRect(waWidth,waHeight,color,-(bWidth/2+waWidth),-(0.7*bHeight/2 + waHeight));
  drawRect(waWidth,waHeight,color,bWidth/2,-(0.7*bHeight/2 + waHeight));
  // =============
*/
  // Wheels
  color = GRAY;
  float wWidth = WHEEL_WIDTH*L;
  float wRadius = WHEEL_RADIUS*L;
  float axisMidY = 0.7*bHeight/2 + waHeight/2;

  //Front Wheels
  glPushMatrix();
  glTranslatef(bWidth/2+waWidth+wWidth/2,axisMidY+wRadius/2,0);
  glRotatef(this->wheelAngle,0,0,1.0);
  //drawRect(wWidth,wHeight,color,-wWidth/2,-wHeight/2);
  solidCylinder(wRadius);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-(bWidth/2+waWidth+wWidth/2),axisMidY+wRadius/2,0);
  glRotatef(this->wheelAngle,0,0,1.0);
  //drawRect(wWidth,wHeight,color,-wWidth/2,-wHeight/2);
  solidCylinder(wRadius);
  glPopMatrix();
/*
  glPushMatrix();
  glTranslatef(-(bWidth/2+waWidth+wWidth/2),axisMidY,0);
  glRotatef(this->wheelAngle,0,0,1.0);
  drawRect(wWidth,wHeight,color,-wWidth/2,-wHeight/2);
  glPopMatrix();
*/
  //Back wheels
  glPushMatrix();
  glTranslatef(-(bWidth/2+waWidth+wWidth/2),-(axisMidY + wRadius/2),0);
  solidCylinder(wRadius);
  glPopMatrix();
  //drawRect(wWidth,wHeight,color,-(bWidth/2+waWidth+wWidth),-(axisMidY + wHeight/2));

  glPushMatrix();
  glTranslatef((bWidth/2+waWidth+wWidth/2),-(axisMidY + wRadius/2),0);
  solidCylinder(wRadius);
  glPopMatrix();
//  drawRect(wWidth,wHeight,color,(bWidth/2+waWidth),-(axisMidY + wHeight/2));
  // ==============
/*
  // Canon (need to draw canon first to be behind the body)
  glPushMatrix();
  glTranslatef(0,bHeight/2,0);
  glRotatef(this->canonAngle,0,0,1.0);
  color = YELLOW;
  float cWidth = CANON_WIDTH*L;
  float cHeight = CANON_HEIGHT*L;
  drawRect(cWidth,cHeight,color,-cWidth/2,0);
  glPopMatrix();
  // =============

  // Windshield
  color = BLACK;
  float wsWidth = WINDSHIELD_WIDTH*L;
  float wsHeight = WINDSHIELD_HEIGHT*L;
  drawRect(wsWidth,wsHeight,color,-bWidth/2,bHeight/8);
  // =============

  // Exhaust Pipes
  color = YELLOW;
  float epWidth = EXHAUST_PIPE_WIDTH*L;
  float epHeight = EXHAUST_PIPE_HEIGHT*L;
  drawRect(epWidth,epHeight,color,-0.45*bWidth,-bHeight/2-epHeight);
  drawRect(epWidth,epHeight,color,0.45*bWidth-epWidth,-bHeight/2-epHeight);
  // =============

  // Draw smoke if car is moving
  static int n = 1; //represents the number of circles used to draw the smoke
  Color smokeColors[] = {LIGHT_GRAY,GRAY,DARK_GRAY};
  float radius = epWidth;
  float dy = 0;

  if(this->moving){
    int i;
    for(i = 0 ; i < n ; i++ ){
      Point centerLeft = {-0.45*bWidth+epWidth/2,-bHeight/2-epHeight-radius - dy};
      drawCircle(radius,centerLeft,smokeColors[i]);
      Point centerRight = {0.45*bWidth-epWidth/2,-bHeight/2-epHeight-radius - dy};
      drawCircle(radius,centerRight,smokeColors[i]);

      dy += radius*1.5;
      radius *= 1.2;
    }

    static GLdouble lastDrawTime = 0;
    GLdouble now = glutGet(GLUT_ELAPSED_TIME);;

    if(now - lastDrawTime > 150){
      n = (++n)%4; // Make n vary only between 0 and 3
      lastDrawTime = now;
    }

  }*/

  glPopMatrix();
}

// This function returns the coordinates of a bullet leaving the canon
Point Car::getBulletInitPos (){

  Point pCanon = {0,0};

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

	if(dy == 0.0 && dy == 0.0)
	 this->moving = false;
	else
	 this->moving = true;

  Point p = {dx,dy};

  return p;
}
