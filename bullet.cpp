#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "bullet.h"
#include "misc.h"
#include "circle.h"

Bullet::Bullet (Point position, Color color, float angleXY, float angleZ){
  this->position = position;
  this->color = color;
  this->angleXY = angleXY;
  this->angleZ = angleZ;
}

void Bullet::draw(){
  glPushMatrix();

  glTranslatef(this->position.x,this->position.y,this->position.z);
  glRotatef(angleXY,0,0,1.0);


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

  glutSolidSphere(BULLET_RADIUS,10,10);

  //drawCircle(BULLET_RADIUS,this->color);
  glPopMatrix();
}

void Bullet::update(float speed){

  float x = this->position.x;
  float newX = x - speed*sin(M_PI*this->angleXY/180.0);
  float y = this->position.y;
  float newY = y + speed*cos(M_PI*this->angleXY/180.0);
  float z = this->position.z;
  float newZ = z + speed*sin(M_PI*this->angleZ/180.0);

  float r = sqrt((newX-x)*(newX-x) + (newY-y)*(newY-y));
  this->position.x = newX;
  this->position.y = newY;
  this->position.z = newZ;
}

Point Bullet::get_position (){
  return this->position;
}
