#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "bullet.h"
#include "misc.h"
#include "circle.h"

Bullet::Bullet (Point position, Color color, float angle){
  this->position = position;
  this->color = color;
  this->angle = angle;
}

void Bullet::draw(){
  glPushMatrix();

  glTranslatef(this->position.x,this->position.y,this->position.z);
  glRotatef(angle,0,0,1.0);


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
  this->position.x += -speed*sin(M_PI*this->angle/180.0);
  this->position.y += speed*cos(M_PI*this->angle/180.0);
  this->position.z = 20; // TODO: MUDAR ISSO DEPOIS!!!!
}

Point Bullet::get_position (){
  return this->position;
}
