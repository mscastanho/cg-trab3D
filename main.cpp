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
static char str[32];
static char str2[16];
void * font = GLUT_BITMAP_9_BY_15;
void * playerWonFont = GLUT_BITMAP_TIMES_ROMAN_24;

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
				Bullet* b = new Bullet(canonPosition,RED,playerAngle+canonAngle);
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
		else
			(*it)->set_cAngle(oldcAngle+enemySpeed*180/(R*M_PI));

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
		else if(mousePressed){
			// In this case the mouse was pressed.
			// trying to detect a press+release task
			mousePressed = false;
			Point canonPosition = player->getBulletInitPos();
			float playerAngle = player->get_cAngle();
			float canonAngle = player->get_cnAngle();
			Bullet* b = new Bullet(canonPosition,GREEN,playerAngle+canonAngle);
			player_bullets.push_back(b);
		}
	}

	glutPostRedisplay();
}

void passiveMotion(int x, int y){

	float canonAngle = player->get_cnAngle();
	if(x > lastMouseX && canonAngle > -45+ANGLE_SPEED )
		player->inc_cnAngle(-ANGLE_SPEED);
	else if (x < lastMouseX && canonAngle < 45-ANGLE_SPEED)
		player->inc_cnAngle(ANGLE_SPEED);

	lastMouseX = x;
}

void keyUp(unsigned char key, int x, int y)
{
	keyStatus[key] = 0;
}
void keyPress(unsigned char key, int x, int y)
{
	keyStatus[key] = 1;
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

	//Create a string to be printed
	char *tmpStr;
	if(playerWon)
		sprintf(str2, "YOU WON!!!!");
	else
		sprintf(str2, "YOU LOST!!!");

	//Define the position to start printing
	glColor3f(1.0,0.0,0.0);
	glRasterPos2f(-5*12,-12);
	//Print  the first Char with a certain font
	//glutBitmapLength(font,(unsigned char*)str);
	tmpStr = str2;
	//Print each of the other Char at time

	while( *tmpStr ){
			glutBitmapCharacter(playerWonFont, *tmpStr);
			tmpStr++;
	}

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
	bool wPressed, sPressed, aPressed, dPressed;

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


		if(gameStarted && !gameOver){
			// Get time from the beginning of the game
			currentTime = glutGet(GLUT_ELAPSED_TIME);

			elapsed = currentTime - timeGameStarted;
			minutes = (int) elapsed/60000;
			seconds = elapsed/1000 - minutes*60;
		}

		//Create a string to be printed
		char *tmpStr;
		sprintf(str, "Time: %2d:%02.2f", minutes, seconds );
		//Define the position to start printing
		glColor3f(0.0,0.0,0.0);
		glRasterPos2f(windowWidth/2-15*9,windowHeight/2-20);
		//Print  the first Char with a certain font
		//glutBitmapLength(font,(unsigned char*)str);
		tmpStr = str;
		//Print each of the other Char at time

		while( *tmpStr ){
				glutBitmapCharacter(font, *tmpStr);
				tmpStr++;
		}
}

void DrawAxes(double size)
{
    GLfloat mat_ambient_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient_b[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
            no_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

    //x axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
        glColor3fv(mat_ambient_r);
        glScalef(size, size*0.02, size*0.02);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //y axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_g);
        glColor3fv(mat_ambient_g);
        glRotatef(90,0,0,1);
        glScalef(size, size*0.02, size*0.02);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //z axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
        glColor3fv(mat_ambient_b);
        glRotatef(-90,0,1,0);
        glScalef(size, size*0.02, size*0.02);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	//glMatrixMode(GL_VIEWPORT);
  //glViewport(0,0,windowWidth,windowHeight);

	glLoadIdentity();
//	glTranslatef(1.5f,0.0f,-7.0f);
	gluLookAt(0,200,200,0,0,0,0,-1,1);

	GLfloat light_position[] = { 200, 100, 100, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat light_position2[] = { 100, 200, 200, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

	DrawAxes(100);
//	glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

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
	player->draw();


	/* Trocar buffers */
	glutSwapBuffers();
	gx = gy = 0;

	time(&seedMag);
	time(&seedSign);
}

void reshape (GLsizei width, GLsizei height){
	// Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);

   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 0.1f, 2000.0f);
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
	int imgSize = 256;
	asphaltTX = png_texture_load("./images/asphalt.png",&imgSize,&imgSize);

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

				//Fundo branco
				Color bgColor = {1.0,1.0,1.0};

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
				glutKeyboardUpFunc(keyUp);
				glutIdleFunc(idle);
				glutPassiveMotionFunc(passiveMotion);
				glutReshapeFunc(reshape);
				glEnable(GL_LIGHT0);
				glEnable(GL_LIGHT1);
				glutMainLoop();
	}else{
		cout << "Nome do arquivo da arena vazio. Por favor verifique o seu config.xml\n";
		exit(1);
	}

	return 0;
}
