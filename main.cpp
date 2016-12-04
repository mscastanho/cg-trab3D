#include <GL/glut.h>
#include <map>
#include <vector>
#include <list>
#include <iterator>
#include "input.h"
#include "circle.h"
#include "rect.h"
#include "misc.h"
#include "car.h"
#include "bullet.h"
#include <cmath>
#include "imageloader.h"
#include "geometry.h"
#include "object.h"

using namespace std;

const int INIT_WINDOW_WIDTH = 500;
const int INIT_WINDOW_HEIGHT = 700;

vector<Car*> enemies;
Circle* arenaOut = NULL;
Circle* arenaIn = NULL;
Car* player = NULL;
Rect* startEnd = NULL;

GLfloat gx=0,gy=0;
GLfloat pgx=0,pgy=0;
int keyStatus[256];

// Deltas to adjust all the coordinates to the new coordinate system
float dx = 0;
float dy = 0;

// Window size
float windowHeight;
float windowWidth;

// Constants for player, enemies and bullets
float PLAYER_SIZE = 0;
float PLAYER_SPEED = 0;
float PLAYER_BULLET_SPEED = 0;
float ANGLE_SPEED = 1;
float ENEMY_SPEED = 0;
float ENEMY_SHOT_FREQ = 0;
float ENEMY_BULLET_SPEED = 0;

// Global to save last mouse position
float lastMouseX;
float lastMouseY;

// Global to save the mouse status
bool mousePressed;

// List of player bullets that have been shot
list<Bullet*> player_bullets;
list<Bullet*> enemy_bullets;

// Text variable
void * font = GLUT_BITMAP_9_BY_15;
void * playerWonFont = GLUT_BITMAP_TIMES_ROMAN_24;
string YOU_WON = "YOU WON!!!";
string YOU_LOST = "YOU LOST!!!";

// Says whether the game has started or not
bool gameStarted = false;
GLdouble timeGameStarted;

// Says if the game is over
bool gameOver = false;

// Says if the player has won or lost
bool playerWon;

// States if the player has already crossed the checkpoint
bool crossedCheckpoint = false;

// Seeds for random number generation
time_t seedMag;
time_t seedSign;

GLuint asphaltTX;
GLuint groundTX;
GLuint cementTX;
GLuint stonewallTX;
GLuint skyTX;
GLuint finishLineTX;

// Camera variables
int toggleCam = 1;
float distCam = 3;
float XYAngle = 0;
float XZAngle = 45;
bool rightButtonPressed = false;

// Lighting variables
bool noturnMode = false;

// Objects
Object* wheelOBJ;
Object* carOBJ;
Object* cannonOBJ;

// Use complex car model or not
bool useComplexModel = true;

// This function calculates the deltas to adjust all coordinates so the center
// of the arena is the new origin
void setNewOrigin(){

	// Adjust y axis orientation to match OpenGL`s
	Point aoc = arenaOut->get_center();
	aoc.y = windowHeight - aoc.y;
	arenaOut->set_center(aoc);

	Point aic = arenaIn->get_center();
	aic.y = windowHeight - aic.y;
	arenaIn->set_center(aic);

	// Delta by which all x and y must be sutracted to set the center of the
	// arena as the new origin
	dx = arenaOut->get_center().x;
	dy = arenaOut->get_center().y;

	// Adjust arena
	Point origin1 = {0,0,0};
	Point origin2 = {0,0,0.5};
	arenaOut->set_center(origin1);
	arenaIn->set_center(origin2);

	// Adjust player
	Point pc = player->get_position();
	pc.x -= dx;
	pc.y = (windowHeight - pc.y) - dy;
	player->set_position(pc);

	// Adjust startEnd rectangle
	Point se = startEnd->get_vertex();
	se.x -= dx;
	se.y = (windowHeight - se.y) - dy;
	startEnd->set_vertex(se);

	// Adjust enemies
	vector<Car*>::iterator it;

	for(it = enemies.begin();it != enemies.end(); it++){
		Point pos = (*it)->get_position();
		pos.x -= dx;
		pos.y = (windowHeight - pos.y) - dy;
		float trajRadius = sqrt(pos.x*pos.x + pos.y*pos.y);
		(*it)->set_trajRadius(trajRadius);
		(*it)->set_position(pos);
		float angle = atan2(pos.y,pos.x)*180/M_PI;
		(*it)->set_cAngle(angle);
	}

}

bool isOutsideWindow(Bullet* b){

	Point pos = b->get_position();

	//Check if bullet is out of the window
	bool cond1 = pos.x > windowWidth/2;
	bool cond2 = pos.x < -windowWidth/2;
	bool cond3 = pos.y > windowHeight/2;
	bool cond4 = pos.y < -windowHeight/2;

	return cond1 || cond2 || cond3 || cond4;
}

void controlEnemyShots(){
	static GLdouble previousTime = 0;
	GLdouble currentTime;
	GLdouble timeDifference;
	GLdouble shotPeriod = 1/ENEMY_SHOT_FREQ;
	vector<Car*>::iterator it;

	if(gameStarted){
		// Get time from the beginning of the game
		currentTime = glutGet(GLUT_ELAPSED_TIME);
		timeDifference = currentTime - previousTime;

		if (timeDifference > shotPeriod){
			previousTime = currentTime;

			for(it = enemies.begin() ; it != enemies.end() ; it++){

				Point canonPosition = (*it)->getBulletInitPos();
				float playerAngle = (*it)->get_cAngle();
				float canonAngle = (*it)->get_cnAngle();
				float canonAngleZ = (*it)->get_cnAngleZ();
				Bullet* b = new Bullet(canonPosition,RED,playerAngle+canonAngle,canonAngleZ);
				enemy_bullets.push_back(b);
			}
		}
	}
}

// This function returns true if a car has collided if it is outside of the
// arena. And return false otherwise.
bool collidedWithArena(Car* c){
	return !(c->insideOf(arenaOut) && c->outsideOf(arenaIn));
}


bool collidedWithEnemies(Car* c){

	vector<Car*>::iterator it;
	bool outsideEnemies = true;

	for(it = enemies.begin();it != enemies.end(); it++){
		if(*it != c) // check if we're not comparing an object to itself
			outsideEnemies = outsideEnemies && c->outsideOf( (*it) );
	}
	return !outsideEnemies;

}

// This function returns true if the enemy has collided with the player
// or if it is outside of the arena. And return false otherwise.
bool collidedWithPlayer(Car* enemy){

	bool outsidePlayer = enemy->outsideOf(player);

	return !outsidePlayer;

}

void updateEnemies(GLdouble timeDiff){
	vector<Car*>::iterator it = enemies.begin();
	Point oldPos, newPos;
	float enemySpeed = ENEMY_SPEED*timeDiff;
	float R; //trajectory radius
	float oldAngle, oldcAngle;

	for(it = enemies.begin() ; it != enemies.end() ; it++){
		// Update position
		oldPos = (*it)->get_position();
		oldAngle = (atan2(oldPos.y,oldPos.x));
		oldcAngle = (*it)->get_cAngle();

		R = (*it)->get_trajRadius();

		float dx = R*cos(oldAngle + enemySpeed/R) - R*cos(oldAngle);
		float dy = R*sin(oldAngle + enemySpeed/R) - R*sin(oldAngle);

		newPos.x = oldPos.x + dx;
		newPos.y = oldPos.y + dy;

		(*it)->inc_position(dx,dy);

		// Check collisions, undo update if necessary
		bool collisionPlayer = collidedWithPlayer(*it);
		bool collisionArena = collidedWithArena(*it);
		bool collisionEnemies = collidedWithEnemies(*it);

		if(collisionPlayer || collisionArena || collisionEnemies)
			(*it)->inc_position(-dx,-dy);
		else{
			(*it)->set_cAngle(oldcAngle+enemySpeed*180/(R*M_PI));
			(*it)->inc_spinAngle(-5*enemySpeed);
		}
		// Change canon angle randomly
		srand((unsigned int) seedMag);
		int ang = rand();
		srand((unsigned int) seedSign);
		int sign = rand();

		if(sign%2 == 0)
			(*it)->inc_cnAngle(ang%5);
		else
			(*it)->inc_cnAngle(-ang%5);
	}

}

void updateBullets(GLdouble timeDiff){

	list<Bullet*>::iterator it;
	list<int> toDelete;
	int i;
	float playerBulletSpeed = PLAYER_BULLET_SPEED*timeDiff;
	float enemyBulletSpeed = ENEMY_BULLET_SPEED*timeDiff;

	//Update bullets
	for(it = player_bullets.begin(), i = 0;it != player_bullets.end(); it++, i++)
		(*it)->update(playerBulletSpeed);

	for(it = enemy_bullets.begin(), i = 0;it != enemy_bullets.end(); it++, i++)
		(*it)->update(enemyBulletSpeed);

	player_bullets.remove_if(isOutsideWindow);
	enemy_bullets.remove_if(isOutsideWindow);
}

void displayBullets(){
	list<Bullet*>::iterator it;

	for(it = player_bullets.begin();it != player_bullets.end(); it++)
		(*it)->draw();

	for(it = enemy_bullets.begin();it != enemy_bullets.end(); it++)
		(*it)->draw();
}

bool checkCheckpoint (Point oldPos, Point newPos){
	float innerRadius = arenaIn->get_radius();
	float outerRadius = arenaOut->get_radius();

	if((oldPos.x > -outerRadius) && (newPos.x > -outerRadius)){
		if((oldPos.x < -innerRadius) && (newPos.x < -innerRadius)){
			if((oldPos.y > 0) && (newPos.y < 0))
				return true; //crossed the checkpoint
		}
	}

	return false;
}

bool crossedFinishLine (Point oldPos, Point newPos){
	float innerRadius = arenaIn->get_radius();
	float outerRadius = arenaOut->get_radius();

	if((oldPos.x < outerRadius) && (newPos.x < outerRadius)){
		if((oldPos.x > innerRadius) && (newPos.x > innerRadius)){
			if((oldPos.y < 0) && (newPos.y > 0))
				return true; //crossed the finish line
		}
	}

	return false;
}

void updatePlayer(bool w, bool s, bool a, bool d, GLdouble timeDiff) {

	// Calculate new position
	Point delta = player->update(w,s,a,d,timeDiff);

	//gy += delta.y;
	//gx += delta.x;

	// Check collisions
	Point oldPos = player->get_position();

	player->inc_position(delta.x,0);
	bool collisionE = collidedWithEnemies(player);
	bool collisionA = collidedWithArena(player);

	if(collisionE || collisionA)
		player->inc_position(-delta.x,0);

	player->inc_position(0,delta.y);
	collisionE = collidedWithEnemies(player);
	collisionA = collidedWithArena(player);

	if(collisionE || collisionA){
	 	player->inc_position(0,-delta.y);
	}


	Point newPos = player->get_position();

	// Check if player has crossed checkpoint of finish line
	if(!crossedCheckpoint){
		crossedCheckpoint = checkCheckpoint(oldPos,newPos);
	}else if(crossedFinishLine(oldPos,newPos)){
		gameOver = true;
		playerWon = true;
	}

}

void mouse(int botao, int estado, int x, int y){
	// Changing y axis orientation
	y = windowHeight - y;

	if(botao == GLUT_LEFT_BUTTON){

		if(estado == GLUT_DOWN)
			mousePressed = true;
		else if(mousePressed && gameStarted){
			// In this case the mouse was pressed.
			// trying to detect a press+release task
			mousePressed = false;
			Point canonPosition = player->getBulletInitPos();
			float playerAngle = player->get_cAngle();
			float canonAngle = player->get_cnAngle();
			float canonAngleZ = player->get_cnAngleZ();
			Bullet* b = new Bullet(canonPosition,GREEN,playerAngle+canonAngle,canonAngleZ);
			player_bullets.push_back(b);
		}
	}else if(botao == GLUT_RIGHT_BUTTON){
		if(estado == GLUT_DOWN)
			rightButtonPressed = true;
		else
			rightButtonPressed = false;
	}

	lastMouseX = x;
	lastMouseY = y;

	glutPostRedisplay();
}

void mouseMotion(int x, int y){
	if(rightButtonPressed && toggleCam == 3){
		float angleInc = 3;
		if(x > lastMouseX)
			XYAngle -= angleInc;
		else if(x < lastMouseX)
			XYAngle += angleInc;

		if(y > lastMouseY && XZAngle >= 2+angleInc)
			XZAngle -= angleInc;
		else if(y < lastMouseY && XZAngle <= 90-angleInc)
			XZAngle += angleInc;
	}

	lastMouseX = x;
	lastMouseY = y;
}

void passiveMotion(int x, int y){

	if(x > lastMouseX)
		player->inc_cnAngle(-ANGLE_SPEED);
	else if (x < lastMouseX)
		player->inc_cnAngle(ANGLE_SPEED);

	if(y > lastMouseY)
		player->inc_cnAngleZ(-ANGLE_SPEED);
	else if (y < lastMouseY)
		player->inc_cnAngleZ(ANGLE_SPEED);

	lastMouseX = x;
	lastMouseY = y;
}

void keyUp(unsigned char key, int x, int y)
{
	keyStatus[key] = 0;
}
void keyPress(unsigned char key, int x, int y)
{
	//keyStatus[key] = 1;

	switch(key)
  {
      case '1':
          toggleCam = 1;
          break;
      case '2':
          toggleCam = 2;
          break;
      case '3':
          toggleCam = 3;
          break;
			case 'x':
			case 'X':
				useComplexModel = not(useComplexModel);
				break;
			case 'n':
			case 'N':
				noturnMode = not(noturnMode);
					break;
      default:
          keyStatus[key] = 1;
  }
}

// Check if any of element of a list of bullets has hit a car
bool checkBulletHit(list<Bullet*>* bullets, Car* car) {
		list<Bullet*>::iterator it;
		bool notHit = true;

		for(it = bullets->begin() ; it != bullets->end() ; it++)
				notHit = notHit && car->outsideOf( (*it) );

		return !notHit;
}

void printEndGameMessage(){

	const char *str;

	if(playerWon)
		str = YOU_WON.c_str();
	else
		str = YOU_LOST.c_str();

	//Define the position to start printing
	glColor3f(1.0,0.0,0.0);

	PrintText(0.4,0.5,str,1,0,0,playerWonFont);

}

void idle (void)
{
	static GLdouble previousTime = 0;
	GLdouble currentTime;
	GLdouble timeDifference;

	// Get time from the beginning of the game
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	timeDifference = currentTime - previousTime;
	previousTime = currentTime;

	// Save the previous values of gx and gy
	pgx = gx;
	pgy = gy;

	float wheelAngle = player->get_wAngle();
	bool wPressed, sPressed, aPressed, dPressed, zPressed, xPressed, nPressed;

	dPressed = (keyStatus['D'] == 1 || keyStatus['d'] == 1) && wheelAngle > -45+ANGLE_SPEED;

	aPressed = (keyStatus['A'] == 1 || keyStatus['a'] == 1) && wheelAngle < 45-ANGLE_SPEED;

	wPressed = keyStatus['W'] == 1 || keyStatus['w'] == 1;

	sPressed = keyStatus['S'] == 1 || keyStatus['s'] == 1;

	// Check game start
	if(!gameStarted && wPressed){
		gameStarted = true;
		timeGameStarted = glutGet(GLUT_ELAPSED_TIME);
	}

	if(gameStarted && (!gameOver || (gameOver && playerWon))){

		updatePlayer(wPressed,sPressed,aPressed,dPressed,timeDifference);
		updateEnemies(timeDifference);

		if(!gameOver)
			controlEnemyShots();

		updateBullets(timeDifference);

		if(!gameOver){
			bool playerHit = checkBulletHit(&enemy_bullets,player);

			if(playerHit){
				gameOver = true;
				playerWon = false; // player lost
			}
		}

		vector<Car*>::iterator it;

		for(it = enemies.begin() ; it != enemies.end() ; ){
				if(checkBulletHit(&player_bullets,(*it)))
					it = enemies.erase(it);
				else
					it++;
		}
	}

	glutPostRedisplay();

}

void printTimer(){

	static int minutes = 0;
	static GLdouble seconds = 0;
	GLdouble currentTime;
	GLdouble elapsed;
	char str[32];

		if(gameStarted && !gameOver){
			// Get time from the beginning of the game
			currentTime = glutGet(GLUT_ELAPSED_TIME);

			elapsed = currentTime - timeGameStarted;
			minutes = (int) elapsed/60000;
			seconds = elapsed/1000 - minutes*60;
		}

		sprintf(str, "Time: %2d:%02.2f", minutes, seconds );

		PrintText(0.7,0.9,str,0,1,0,font);
}


void drawWorld(){

	// Sky
	drawSky(500,10,skyTX);

	//Arena Out
	Point originOut = {0,0,0};
	float outRadius = arenaOut->get_radius();
	drawCircle3D(outRadius,originOut,asphaltTX);

	//Arena IN
	Point originIn = {0,0,2};
	float inRadius = arenaIn->get_radius();
	drawCircle3D(inRadius,originIn,groundTX);


	// FinishLine
	startEnd->draw(finishLineTX);

	// Curb
	//drawCurb3D(inRadius,1,20,originIn,1,72,cementTX);

	float wallHeight = 8*player->get_height();
	drawWallArena3D(outRadius,1,wallHeight,originOut,1,72,stonewallTX);

	// Draw Ceiling
	glDisable(GL_TEXTURE_2D);
	Point originCeiling = {0,0,wallHeight};
	drawArenaCeiling(outRadius,originCeiling,1);
	glEnable(GL_TEXTURE_2D);

	player->draw(useComplexModel);

	vector<Car*>::iterator it;
	for(it = enemies.begin();it != enemies.end(); it++)
		(*it)->draw(useComplexModel);

	displayBullets();

}

void changeCamera(float x1, float y1, float x2, float y2){
	 // Compute aspect ratio of the new window
	 GLfloat aspect;

	 if (y2 - y1 == 0)
	 	aspect = (GLfloat)(x2-x1) / 1; // To prevent divide by 0
	 else
	 	aspect = (GLfloat)(x2-x1) / ((GLfloat) y2-y1);

	 // Set the viewport to cover the new window
	 //glViewport(0, 0, width, (5.0/7.0)*height);

	 glViewport(x1,y1,x2,y2);

	 // Set the aspect ratio of the clipping volume to match the viewport
	 glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	 glLoadIdentity();             // Reset
	 // Enable perspective projection with fovy, aspect, zNear and zFar
	 gluPerspective(45.0f, aspect, 0.1f, 2000.0f);
}

void enableLights(float arenaHeight, float arenaRadius){

	if(!noturnMode)
	{
		GLfloat light2_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
		GLfloat light2_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light2_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light2_position[] = { 0, 10, 0, 1.0 };
		GLfloat spot_direction[] = { 0, 1, 0.0 };

		glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
		glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
		glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
		glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.5);
		glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.5);
		glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.2);

		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 180.0);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 1.0);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT2);
		glDisable(GL_LIGHT1);
	}
	else
	{
		Point pos = player->get_position();
		float angle = player->get_cAngle();

		GLfloat light1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
		GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light1_position[] = { pos.x, pos.y, pos.z, 1.0 };
		GLfloat spot_direction[] = { 1.0*cos((angle+90)*M_PI/180), 1.0*sin((angle+90)*M_PI/180), 0.0 };

		glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2);

		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1.0);

		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT2);
	}

	/*GLfloat light_position2[] = { 100, 200, 200, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
*/
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	enableLights(0,0);

	glLoadIdentity();

 // Desenhar visão do retrovisor
 float sizeOfCar = player->get_size()/2;
 float angleOfCar = player->get_cAngle();

 float xCameraRotated = cos((90 + angleOfCar)*M_PI/180);
 float yCameraRotated = sin((90 + angleOfCar)*M_PI/180);
 float zCameraRotated;

 Point playerPos = player->get_position();

 changeCamera(0,(5.0/7.0)*windowHeight,windowWidth,windowHeight);

 gluLookAt(  playerPos.x+sizeOfCar*xCameraRotated,
	 					 playerPos.y+sizeOfCar*yCameraRotated,
						 2*sizeOfCar*BODY_HEIGHT, // at the top front of the car
					   playerPos.x,
						 playerPos.y,
						 2*sizeOfCar*BODY_HEIGHT+8, //Looking down
					   0,
						 0,
						 1); //Lookup pointing to z axis(ceiling)

	drawWorld();

	// Desenhar visão frontal

	changeCamera(0,0,windowWidth,(5.0/7.0)*windowHeight);

	switch(toggleCam)
  {
 		 case 1:
 		 		 gluLookAt(  playerPos.x+sizeOfCar*xCameraRotated,
					 					 playerPos.y+sizeOfCar*yCameraRotated,
										 sizeOfCar*BODY_HEIGHT, //Camera at the top front of the car
 										 playerPos.x+2*sizeOfCar*xCameraRotated,
										 playerPos.y+2*sizeOfCar*yCameraRotated,
										 sizeOfCar*BODY_HEIGHT, //Looking down
 										 0,0,1); //Lookup pointing to z axis(ceiling)
 				 break;
 		 case 2:
		 {
				Point canonPosition = player->getBulletInitPos();

				float carAngle = player->get_cAngle();
				float cannonAngle = player->get_cnAngle();
				float cannonAngleZ = player->get_cnAngleZ();

				  float x = canonPosition.x;
				  float newX = x - sin(M_PI*(carAngle+cannonAngle)/180.0);
				  float y = canonPosition.y;
				  float newY = y + cos(M_PI*(carAngle+cannonAngle)/180.0);
				  float z = canonPosition.z;
				  float newZ = z + sin(M_PI*(cannonAngleZ)/180.0);

					gluLookAt(canonPosition.x,canonPosition.y,canonPosition.z,
							canonPosition.x + (newX - x),
					 		canonPosition.y + (newY - y),
							canonPosition.z + (newZ - z),
							0,
							0,
							1);
 				 break;
		 }
 		 case 3:
				 float cameraXYAngleRotated = XYAngle + player->get_cAngle();
				 xCameraRotated = sin((XZAngle)*M_PI/180) * cos((cameraXYAngleRotated - 90)*M_PI/180);
				 yCameraRotated = sin((XZAngle)*M_PI/180) * sin((cameraXYAngleRotated - 90)*M_PI/180);
				 zCameraRotated = cos((XZAngle)*M_PI/180);

				 gluLookAt(	playerPos.x+sizeOfCar*xCameraRotated*distCam,
								playerPos.y+sizeOfCar*yCameraRotated*distCam,
								playerPos.z+sizeOfCar*zCameraRotated*distCam,
							 playerPos.x,playerPos.y,playerPos.z,
							 cos(abs(XZAngle)*M_PI/180)*-sin(cameraXYAngleRotated*M_PI/180),
								cos(abs(XZAngle)*M_PI/180)*cos(cameraXYAngleRotated*M_PI/180),
								sin(abs(XZAngle)*M_PI/180));
				 break;
  }

	drawWorld();


	printTimer();

	if(gameOver)
		printEndGameMessage();


	/* Trocar buffers */
	glutSwapBuffers();
	gx = gy = 0;

	time(&seedMag);
	time(&seedSign);
}

void reshape (GLsizei width, GLsizei height){

	windowWidth = width;
	windowHeight = height;

}


void init (Color bgColor, float xlim1, float xlim2, float ylim1, float ylim2)
{
	/*select background color */
	float r = bgColor.r;
	float g = bgColor.g;
	float b = bgColor.b;
	glClearColor((GLfloat) r,(GLfloat) g,(GLfloat) b,0.0);
	glShadeModel(GL_SMOOTH);
	glViewport(0,0,500,500);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//Initialize textures
	asphaltTX = LoadTextureRAW("./images/asphalt.bmp");
	groundTX = LoadTextureRAW("./images/ground.bmp");
	cementTX = LoadTextureRAW("./images/cement.bmp");
	stonewallTX = LoadTextureRAW("./images/stonewall.bmp");
	skyTX = LoadTextureRAW("./images/sky.bmp");
	finishLineTX = LoadTextureRAW("./images/finishline.bmp");

	//Load Objects
	wheelOBJ = readOBJFile("./objects/Pneu.obj");
	carOBJ = readOBJFile("./objects/Ferrari.obj");
	cannonOBJ = readOBJFile("./objects/Cannon.obj");
	//loadTexturesFromMaterials(wheelOBJ->materials);

	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_PROJECTION);
	//glOrtho(xlim1,xlim2,ylim1,ylim2,-1.0,1.0);
//	gluPerspective(60.0,1,-1,500);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

int main (int argc, char** argv)
{
	string filePath;

	// Confere se o arquivo XML foi passado como parametro de entrada
	if (argc > 1 ){
		filePath = argv[1];
		filePath += "config.xml";
	}else{
		cout << "Por favor entre com o arquivo XML" << endl;
		cout << "Ex: trabalhocg <caminho para pasta de config.xml>" << endl;
		return 0;
	}

	if(filePath != ""){
		string arenaFile = parseConfigFile(filePath);

		if(arenaFile != "")
				parseSVGFile(arenaFile);

				glutInit(&argc,argv);

				// Utilizar Double Buffer, esquema de cores RGB e depth buffer
				glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);

				// Definindo o tamanho da janela a ser aberta
				windowWidth = INIT_WINDOW_WIDTH;
				windowHeight = INIT_WINDOW_HEIGHT;
				glutInitWindowSize(windowWidth,windowHeight);

				// Escolher posicao da janela na tela
				glutInitWindowPosition(250,250);

				// Definindo o titulo da janela
				glutCreateWindow("Arena");

				//Fundo azul claro
				Color bgColor = {135,206,250};

				// Points to define the ortho
				float xlim1 = -arenaOut->get_radius();
				float xlim2 = arenaOut->get_radius();
				float ylim1 = xlim1;
				float ylim2 = xlim2;

				init(bgColor,xlim1,xlim2,ylim1,ylim2);
				setNewOrigin();
				glutDisplayFunc(display);
				glutKeyboardFunc(keyPress);
				glutMouseFunc(mouse);
				glutMotionFunc(mouseMotion);
				glutKeyboardUpFunc(keyUp);
				glutIdleFunc(idle);
				glutPassiveMotionFunc(passiveMotion);
				glutReshapeFunc(reshape);

				//map<string,Material>* materials = readMTLFile("./objects/Pneu.mtl");
				//printMaterialsMap(materials);

			//	Object *o = readOBJFile("./objects/Pneu.obj");

				/*cout << "vertices: " << endl;
				for(int i = 0 ; i < o->nTexels ; i++){
					printf("u: %f v: %f ",o->texels[i].u,o->texels[i].v);
					cout << ((o->texels[i]).m)->name << endl;
					//cout << i << endl;
				}*/

				glutMainLoop();
	}else{
		cout << "Nome do arquivo da arena vazio. Por favor verifique o seu config.xml\n";
		exit(1);
	}

	return 0;
}
