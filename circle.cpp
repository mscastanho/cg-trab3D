#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <cmath>
#include "circle.h"
#include "misc.h"


using namespace std;

Circle::Circle(Point center, float r, Color c){
	this->center = center;
	this->radius = r;
	this->color = c;
}

void Circle::draw (){
	glPushMatrix();

	glColor3f(this->color.r,this->color.g,this->color.b);


	glTranslatef(this->center.x,this->center.y,this->center.z);
	// Needed to invert y axis because of SVG and OpenGL's different y-axis
	// orientation

	int i;
	float x,y;
	glBegin(GL_POLYGON);
	// Let`s use 36 vertices to draw the circle as a polygon
	for(i = 0 ; i < 360 ; i += 4 ){
		x = radius*cos(M_PI*i/180.0);
		y = radius*sin(M_PI*i/180.0);
		glVertex3f(x,y,1.0f);
	}
	glEnd();

	glPopMatrix();
}

void Circle::draw (float deltaX, float deltaY){
	glPushMatrix();

	glColor3f(this->color.r,this->color.g,this->color.b);


	glTranslatef(this->center.x + deltaX,-this->center.y + deltaY,1);
	// Needed to invert y axis because of SVG and OpenGL's different y-axis
	// orientation

	int i;
	float x,y;
	glBegin(GL_POLYGON);
	// Let`s use 36 vertices to draw the circle as a polygon
	for(i = 0 ; i < 360 ; i += 4 ){
		x = radius*cos(M_PI*i/180.0);
		y = radius*sin(M_PI*i/180.0);
		glVertex3f(x,y,-1.0f);
	}
	glEnd();

	glPopMatrix();
}

Point Circle::get_center() const{
	return this->center;
}

void Circle::set_center(Point c){
	this->center = c;
}

float Circle::get_radius() const{
	return this->radius;
}

void Circle::set_radius(float r){
	this->radius = r;
}

Color Circle::get_color() const{
	return this->color;
}

void Circle::set_color(Color c){
	this->color = c;
}

void Circle::print() const{

	float r = this->color.r;
	float g = this->color.g;
	float b = this->color.b;

	cout << "< Circle >" << endl;
	printf("center: (%f,%f)\n",this->center.x,this->center.y);
	printf("radius: %f\n",this->radius);
	printf("Color: (%f,%f,%f)\n",r,g,b);
}

ostream& operator<<(ostream& output, const Circle& c){
	output << "Circle: c=(" << c.center.x << "," << c.center.y << ") r=" << c.radius << endl;
	return output;
}

//Draws a circle centered in the origin
void drawCircle (float radius, Color color){

	glColor3f(color.r,color.g,color.b);

	// Needed to invert y axis because of SVG and OpenGL's different y-axis
	// orientation

	int i;
	float x,y;
	glBegin(GL_POLYGON);
	// Let`s use 36 vertices to draw the circle as a polygon
	for(i = 0 ; i < 360 ; i += 4 ){
		x = radius*cos(M_PI*i/180.0);
		y = radius*sin(M_PI*i/180.0);
		glVertex3f(x,y,1.0f);
	}
	glEnd();

}

//Draws a circle centered at center
void drawCircle (float radius, Point center, Color color){

	glPushMatrix();

	glColor3f(color.r,color.g,color.b);
	glTranslatef(center.x,center.y,0);

	// Needed to invert y axis because of SVG and OpenGL's different y-axis
	// orientation

	int i;
	float x,y;
	glBegin(GL_POLYGON);
	// Let`s use 36 vertices to draw the circle as a polygon
	for(i = 0 ; i < 360 ; i += 4 ){
		x = radius*cos(M_PI*i/180.0);
		y = radius*sin(M_PI*i/180.0);
		glVertex3f(x,y,1.0);
	}
	glEnd();

	glPopMatrix();
}

//Draws a plane circle in the XY plane
void drawCircle3D(float radius, Point center, GLuint texture){
	glPushMatrix();
	glTranslatef(center.x,center.y,center.z);

	GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
	GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
	GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1};
	GLfloat mat_shininess[] = { 100.0 };
	glColor3f(1,1,1);

	glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	int i;
	float x,y;
	glBindTexture (GL_TEXTURE_2D, texture);
	glBegin(GL_POLYGON);
	// Let`s use 36 vertices to draw the circle as a polygon
	for(i = 0 ; i < 360 ; i += 4 ){
		glNormal3f(0,0,1); //always pointing up (z-axis)

		float cossine = cos(M_PI*i/180.0);
		float sine = sin(M_PI*i/180.0);

		glTexCoord2f (cossine,sine);

		x = radius*cossine;
		y = radius*sine;

		glVertex3f(x,y,0);
	}
	glEnd();

	glPopMatrix();
}

void drawArenaCeiling(float radius, Point center, GLuint texture){
	glPushMatrix();
	glTranslatef(center.x,center.y,center.z);

	GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
	GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
	GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1};
	GLfloat mat_shininess[] = { 100.0 };
	glColor3f(0,0,0);

	glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	int i;
	float x,y;
	//glBindTexture (GL_TEXTURE_2D, texture);
	glBegin(GL_POLYGON);
	// Let`s use 36 vertices to draw the circle as a polygon
	for(i = 0 ; i < 360 ; i += 4 ){
		glNormal3f(0,0,-1); //always pointing up (z-axis)

		float cossine = cos(M_PI*i/180.0);
		float sine = sin(M_PI*i/180.0);

		glTexCoord2f (cossine,sine);

		x = radius*cossine;
		y = radius*sine;

		glVertex3f(x,y,0);
	}
	glEnd();

	glPopMatrix();
}

void drawCurb3D(float radius, float thickness, float height, Point center, int stacks, int slices, GLuint texture){

	glPushMatrix();
	glTranslatef(center.x,center.y,center.z);

	int i;
	glBindTexture (GL_TEXTURE_2D, texture);
	float angleStep = 360.0/slices;
	float verticalStep = height/stacks;
	float previousX = radius;
	float previousY = 0;
	float previousXin = radius-thickness;
	float previousYin = 0;
	for(i = angleStep ; i < 360 + angleStep ; i += angleStep ){


		float x = radius*cos(M_PI*i/180.0);
		float y = radius*sin(M_PI*i/180.0);

		//Draw outside faces
		/*	v2._____.v3
			   	|			|
			  	._____.
				v1      v4
		*/
		glBegin(GL_QUADS);
			//v1
			glNormal3f(previousX,previousY,0);
			glTexCoord2f(0,0);
			glVertex3f(previousX,previousY,0);

			//v2
			glNormal3f(previousX,previousY,0);
			glTexCoord2f(0,1);
			glVertex3f(previousX,previousY,verticalStep);

			//v3
			glNormal3f(x,y,0);
			glTexCoord2f(1,1);
			glVertex3f(x,y,verticalStep);

			//v4
			glNormal3f(x,y,0);
			glTexCoord2f(1,0);
			glVertex3f(x,y,0);
		glEnd();

		//Draw inside faces
		/*	v2._____.v3
			   	|			|
			  	._____.
				v1      v4
		*/

	/*	float inRadius = radius - thickness;
		float xin = inRadius*cos(M_PI*i/180.0);
		float yin = inRadius*sin(M_PI*i/180.0);
		glBegin(GL_TRIANGLE_STRIP);
			//v4
			glNormal3f(x,y,0);
			glTexCoord2f(1,0);
			glVertex3f(x,y,0);

			//v3
			glNormal3f(x,y,0);
			glTexCoord2f(1,1);
			glVertex3f(x,y,verticalStep);

			//v2
			glNormal3f(previousX,previousY,0);
			glTexCoord2f(0,1);
			glVertex3f(previousX,previousY,verticalStep);

			//v1
			glNormal3f(previousX,previouxY,0);
			glTexCoord2f(0,0);
			glVertex3f(previousX,previousY,0);


		glEnd();
*/
		previousX = x;
		previousY = y;

	}

	glPopMatrix();
}

void drawWallArena3D(float radius, float thickness, float height, Point center, int stacks, int slices, GLuint texture){

	glPushMatrix();
	glTranslatef(center.x,center.y,center.z);

	int i;
	glBindTexture (GL_TEXTURE_2D, texture);
	float angleStep = 360.0/slices;
	float verticalStep = height/stacks;
	float previousX = radius;
	float previousY = 0;
	for(i = angleStep ; i < 360 + angleStep ; i += angleStep ){


		float x = radius*cos(M_PI*i/180.0);
		float y = radius*sin(M_PI*i/180.0);

		//Draw outside faces
		/*	v2._____.v3
			   	|			|
			  	._____.
				v1      v4
		*/
		glBegin(GL_QUADS);
			//v1
			glNormal3f(-previousX,-previousY,0);
			glTexCoord2f(0,0);
			glVertex3f(previousX,previousY,0);

			//v2
			glNormal3f(-previousX,-previousY,0);
			glTexCoord2f(0,1);
			glVertex3f(previousX,previousY,verticalStep);

			//v3
			glNormal3f(-x,-y,0);
			glTexCoord2f(1,1);
			glVertex3f(x,y,verticalStep);

			//v4
			glNormal3f(-x,-y,0);
			glTexCoord2f(1,0);
			glVertex3f(x,y,0);
		glEnd();

		previousX = x;
		previousY = y;

	}

	glPopMatrix();
}
