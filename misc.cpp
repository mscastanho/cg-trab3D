#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "misc.h"

void printPoint(Point p){
  printf("x: %f, y: %f, z: %f\n",p.x,p.y,p.z);
}

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

void RasterChars(GLfloat x, GLfloat y, GLfloat z, const char * text, double r, double g, double b, void* font)
{
    //Push to recover original attributes
    glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //Draw text in the x, y, z position
        glColor3f(r,g,b);
        glRasterPos3f(x, y, z);
        const char* tmpStr;
        tmpStr = text;
        while( *tmpStr ){
            glutBitmapCharacter(font, *tmpStr);
            tmpStr++;
        }
    glPopAttrib();
}

void PrintText(GLfloat x, GLfloat y, const char * text, double r, double g, double b, void* font)
{
    //Draw text considering a 2D space (disable all 3d features)
    glMatrixMode (GL_PROJECTION);
    //Push to recover original PROJECTION MATRIX
    glPushMatrix();
        glLoadIdentity ();
        glOrtho (0, 1, 0, 1, -1, 1);
        RasterChars(x, y, 0, text, r, g, b, font);
    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
}
