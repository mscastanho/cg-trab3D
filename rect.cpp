#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "rect.h"
#include "misc.h"

using namespace std;

Rect::Rect(float x, float y, float width, float height, Color c){
	// Assuming x and y correspond to the lower-left vertex of the rectangle

	Point v = {x,y,0.1};

	this->vertex = v;
	this->width = width;
	this->height = height;
	this->color = c;
}

void Rect::draw (GLuint texture){

/*	glColor3f(this->color.r,this->color.g,this->color.b);

	glBegin(GL_QUADS);
		glVertex2f(0,0);
		glVertex2f(width,0);
		glVertex2f(width,height);
		glVertex2f(0,height);
	glEnd();
*/

	drawRect3D (this->width,this->height,vertex,texture);


}

void Rect::draw (Point pos){
	glPushMatrix();

	glColor3f(this->color.r,this->color.g,this->color.b);

	//glTranslatef(pos.x,pos.y,0);
	// Needed to invert y axis because of SVG and OpenGL's different y-axis
	// orientation

	glBegin(GL_QUADS);
		glVertex3f(0,0,1.0f);
		glVertex3f(width,0,1.0f);
		glVertex3f(width,height,1.0f);
		glVertex3f(0,height,1.0f);
	glEnd();

	glPopMatrix();
}

void drawRect (float width, float height, Color c){

	glColor3f(c.r,c.g,c.b);

	glBegin(GL_QUADS);
		glVertex3f(0,0,0);
		glVertex3f(width,0,0);
		glVertex3f(width,height,0);
		glVertex3f(0,height,0);
	glEnd();

}

void drawRect (float width, float height, Color c, float xTrans, float yTrans){
	glPushMatrix();

	glColor3f(c.r,c.g,c.b);

	glTranslatef(xTrans,yTrans,1);
	// Needed to invert y axis because of SVG and OpenGL's different y-axis
	// orientation

	glBegin(GL_QUADS);
		glVertex2f(0,0);
		glVertex2f(width,0);
		glVertex2f(width,height);
		glVertex2f(0,height);
	glEnd();

	glPopMatrix();
}

void drawRect3D (float width, float height, Point lowerLeftCorner, GLuint texture){
	glPushMatrix();


	glTranslatef(lowerLeftCorner.x,lowerLeftCorner.y,lowerLeftCorner.z);

	glBindTexture (GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
		glNormal3f(0,0,1);
		glTexCoord2f(0,0);
		glVertex2f(0,0);

		glNormal3f(0,0,1);
		glTexCoord2f(width/10,0);
		glVertex2f(width,0);

		glNormal3f(0,0,1);
		glTexCoord2f(width/10,height/10);
		glVertex2f(width,height);

		glNormal3f(0,0,1);
		glTexCoord2f(0,height/10);
		glVertex2f(0,height);
	glEnd();

	glPopMatrix();
}

// getter for the vertices
Point Rect::get_vertex() const{
	return this->vertex;
}

void Rect::set_vertex(Point v1){
	this->vertex = v1;
}

void Rect::print() const{
	float r = this->color.r;
	float g = this->color.g;
	float b = this->color.b;

	cout << "< Rect >" << endl;
	printf("Vertex: (%f,%f)\n",this->vertex.x,this->vertex.y);
	printf("Width: %f\n",this->width);
	printf("Height: %f\n",this->height);
	printf("Color: (%f,%f,%f)\n",r,g,b);

}

ostream& operator<<(ostream& output, const Rect& c){
	output << "Sou um Rectrilatero." << endl;
	return output;
}
