#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "misc.h"

// Andle must be in degrees
Point rotateBy(Point p, float angle){
  Point rotated;
  float angleRad = M_PI*angle/180.0;
  float sine = sin(angleRad);
  float cossine = cos(angleRad);

  rotated.x = p.x*cossine - p.y*sine;
  rotated.y = p.x*sine + p.y*cossine;

  return rotated;
}

// This assumes that the coordinates of oldOrigin are expressed in terms
// of the new coordinate system
Point translateFrom(Point p, Point oldOrigin){
  Point translated = p;

  translated.x += oldOrigin.x;
  translated.y += oldOrigin.y;

  return translated;
}

void solidCylinder(float radius){
  glPushMatrix();

  glScalef(0.5,1,1);
  glutSolidSphere(radius,50,50);
  glPopMatrix();
}
