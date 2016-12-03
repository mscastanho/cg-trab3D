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
  float y = this->position.y;

  float r = sqrt(x*x + y*y);
  this->position.x += -speed*sin(M_PI*this->angleXY/180.0);
  this->position.y += speed*cos(M_PI*this->angleXY/180.0);

  //this->position.z += speed*r/tan(M_PI*this->angleZ/180.0);
}

Point Bullet::get_position (){
  return this->position;
}
