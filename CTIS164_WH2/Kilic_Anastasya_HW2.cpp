/*********
CTIS164 - Template Source Program
----------
STUDENT : ANASTASYA DUYGU KILIC
SECTION : 001
HOMEWORK: HW2
----------
PROBLEMS: -After clicking on screen for the frog to appear, sometimes the angle on screen doesn't immidiately increase
          but stays as zero but it can still find the right place to hit and after hitting the first target it fixes itself
		  -While in automatic mode if you try to change to manual mode or the timer runs out while the tongue is streching out
		  it glitches out but it fixes itself after switching to automatic and then to manual again when the tongue is not 
		  stretched out
		  -Only When the target appears between first and fourth region of the window sometimes CLOCKWISE and COUNTERCLOCKWISE desicion does 
		  not work properly
----------
ADDITIONAL FEATURES: - When the required angle is reached tongue extends and the mouth opens until target is reached
                     - There are three modes you can switch between using END MENU commands: AUTOMATIC <F3>, INTERACTIVE <F4>, END MENU <F2>
					 - You can only switch between AUTOMATIC and INTERACTIVE modes while you are in END PHASE mode, but you can press <F2> to
					 reach END PHASE menu any time you want
					 - In INTERACTIVE mode you can set the timer as you want and after pressing <F1> hit as many flies as you
					 can in the time you have set
					 - In challenge mode the you can also see how many targets you have hit in the time you have set
					 - Also in INTERACTIVE mode when you press <spacebar> the tongue expands 150 pixels regardless of whether it is going to
					 hit the target or not
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532


#define START 3
#define RUN 4
#define END 5

#define COUNTERCLOCKWISE 1
#define CLOCKWISE 2


typedef struct {
	float frogX;
	float frogY;
}frog_t;

typedef struct {
	float flyX;
	float flyY;
}fly_t;


/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false, F1 = false, F2 = false, F3 = false, F4 = false;
int  winWidth, winHeight; // current Window width and height
float lenght = 40, ax, ay, angleHit, angle = 1, mode = COUNTERCLOCKWISE, r = 0, g = 0, b = 0, counter = 3600, gameTimer = 0, flyHit = 0, speed = 4;
bool run = true; //turns false when timer runs out
bool hit = false; //if true reset target
bool tongue = false; //if true tongue extends
bool challenge = false; //if activated in interactive mode it counts how many flies have been hit in the setted amount of time
fly_t fly{ 100,100 };
frog_t frog;
bool stopTongue = false; //if true it stops
int window = START; //display options

float tonguex = 0, tonguey = 20, interx, intery;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

//
// To display onto window using OpenGL commands
//

void drawTarget() {
	int i = -1;
	for (int k = 0; k < 2; k++)
	{
		//wings
		glColor3ub(96, 96, 96);
		circle(fly.flyX + 10 * i, fly.flyY + 4, 7);
		circle(fly.flyX + 14 * i, fly.flyY + 6, 8);
		circle(fly.flyX + 16 * i, fly.flyY + 6, 9);
		if (i == -1)
			i = 1;
	}
	glColor3ub(r, g, b);
	circle(fly.flyX, fly.flyY, 10);
	i = -1;
	for (int k = 0; k < 2; k++)
	{
		//eyes
		glColor3f(1, 1, 1);
		circle(fly.flyX + 6 * i, fly.flyY + 2, 5);
		glColor3ub(0, 0, 0);
		circle(fly.flyX + 5 * i, fly.flyY + 2, 3);

		if (i == -1)
			i = 1;
	}
}

void resetTarget() {
	fly.flyX = rand() % (winWidth - 40) - (winWidth - 50) / 2;
	fly.flyY = rand() % (winHeight - 100) - (winHeight - 100) / 2;
	r = rand() % 256;
	g = rand() % 256;
	b = rand() % 256;
}

void drawTongue() {
	ax = lenght * cos(angle * D2R);
	ay = lenght * sin(angle * D2R) - 9;
	glLineWidth(3);
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(frog.frogX, frog.frogY + 20);
	glVertex2d(ax, ay + 30);
	glEnd();
}

void drawTongueOut() { //activefire == true
	glLineWidth(3);
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(frog.frogX, frog.frogY + 15);
	glVertex2d(tonguex, tonguey);
	glEnd();
}

void drawTongueOutInteractive() {
	interx = 150 * cos(angle * D2R); //lenght of tongue is 150
	intery = 150 * sin(angle * D2R); //lenght of tongue is 150
	glLineWidth(3);
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(frog.frogX, frog.frogY + 20);
	glVertex2d(interx, intery);
	glEnd();
}

void displayLegs() {
	int i = -1;

	for (int k = 0; k < 2; k++)
	{
		glBegin(GL_TRIANGLES);
		glVertex2d(frog.frogX, frog.frogY - 31);
		glVertex2d(frog.frogX - 42 * i, frog.frogY);
		glVertex2d(frog.frogX - 37 * i, frog.frogY - 31);
		glEnd();
		circle(frog.frogX + 44 * i, frog.frogY, 5);
		glBegin(GL_POLYGON);
		glVertex2d(frog.frogX, frog.frogY - 30);
		glVertex2d(frog.frogX + 38 * i, frog.frogY);
		glVertex2d(frog.frogX + 49 * i, frog.frogY);
		glVertex2d(frog.frogX + 37 * i, frog.frogY - 30);
		//feet
		glVertex2d(frog.frogX + 37 * i, frog.frogY - 30);
		glVertex2d(frog.frogX + 45 * i, frog.frogY - 32);
		glVertex2d(frog.frogX + 40 * i, frog.frogY - 35);
		glVertex2d(frog.frogX + 37 * i, frog.frogY - 30);
		glEnd();
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2d(frog.frogX + 37 * i, frog.frogY - 30);
		glVertex2d(frog.frogX + 50 * i, frog.frogY - 31);

		glVertex2d(frog.frogX + 37 * i, frog.frogY - 33);
		glVertex2d(frog.frogX + 50 * i, frog.frogY - 35);

		glVertex2d(frog.frogX + 37 * i, frog.frogY - 34);
		glVertex2d(frog.frogX + 50 * i, frog.frogY - 39);
		glEnd();
		circle(frog.frogX + 50 * i, frog.frogY - 39, 2);
		circle(frog.frogX + 50 * i, frog.frogY - 35, 2);
		circle(frog.frogX + 50 * i, frog.frogY - 31, 2);
		if (i == -1)
			i = 1;
	}
}

void displayMouthClose() {
#define PI 3.1415
	float angle;
	int r = 20;
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = -PI * i / 100;
		glVertex2f(frog.frogX + r * cos(angle), frog.frogY + 35 + r * sin(angle));
	}
	glEnd();
	glColor3ub(158, 217, 138);
	glRectf(frog.frogX - 30, frog.frogY + 33, frog.frogX + 30, frog.frogY + 36);
}

void displayMouthOpen() {
#define PI 3.1415
	float angle;
	int r = 20;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = -PI * i / 100;
		glVertex2f(frog.frogX + r * cos(angle), frog.frogY + 35 + r * sin(angle));
	}
	glEnd();
}

void displayFrog() {
	//body
	glColor3ub(158, 217, 138);
	circle(frog.frogX, frog.frogY, 30);

	glColor3ub(176, 225, 159);
	circle(frog.frogX, frog.frogY, 20);

	glColor3ub(158, 217, 138);
	circle(frog.frogX, frog.frogY + 30, 32);
	circle(frog.frogX - 25, frog.frogY + 60, 15);
	circle(frog.frogX + 25, frog.frogY + 60, 15);
	//legs
	displayLegs();
	//eyes
	glColor3f(1, 1, 1);
	circle(frog.frogX - 25, frog.frogY + 60, 12);
	circle(frog.frogX + 25, frog.frogY + 60, 12);
	glColor3ub(4, 80, 9);
	circle(frog.frogX - 25, frog.frogY + 60, 9);
	circle(frog.frogX + 25, frog.frogY + 60, 9);
	glColor3f(0, 0, 0);
	circle(frog.frogX - 24, frog.frogY + 60, 5);
	circle(frog.frogX + 24, frog.frogY + 60, 5);
	//nose
	glColor3ub(4, 80, 9);
	glBegin(GL_LINES);
	glVertex2d(frog.frogX + 8, frog.frogY + 40);
	glVertex2d(frog.frogX + 6, frog.frogY + 43);

	glVertex2d(frog.frogX - 8, frog.frogY + 40);
	glVertex2d(frog.frogX - 6, frog.frogY + 43);
	glEnd();
	//mouth
	if (stopTongue == true)
		displayMouthOpen();
	else displayMouthClose();
	//blush
	glColor3ub(255, 204, 204);
	circle(frog.frogX + 25, frog.frogY + 35, 5);
	circle(frog.frogX - 25, frog.frogY + 35, 5);

}

void drawLilypad() {
	int size = 60;
	glColor3ub(39, 199, 108);
	circle(frog.frogX, frog.frogY - 15, size);
	glColor3ub(200, 224, 243);
	glBegin(GL_TRIANGLES);
	glVertex2d(frog.frogX - 18, frog.frogY - 80);
	glVertex2d(frog.frogX + 18, frog.frogY - 80);
	glVertex2d(frog.frogX, frog.frogY - 35);
	glEnd();
}

void border() {
	glColor3ub(255, 128, 0);
	glRectf(-winWidth / 2, winHeight / 2 - 80, winWidth / 2, winHeight / 2);
	glRectf(-winWidth / 2, -winHeight / 2, winWidth / 2, -winHeight / 2 + 50);
	glColor3ub(255, 178, 102);
	glRectf(-winWidth / 2 + 10, winHeight / 2 - 70, winWidth / 2 - 10, winHeight / 2 - 10);
	glRectf(-winWidth / 2 + 10, -winHeight / 2 + 10, winWidth / 2 - 10, -winHeight / 2 + 40);

	glColor3f(0, 0, 0);
	vprint(-winWidth / 2 + 20, winHeight / 2 - 30, GLUT_BITMAP_8_BY_13, "ANASTASYA DUYGU KILIC");
	vprint(-100, winHeight / 2 - 60, GLUT_BITMAP_TIMES_ROMAN_24, "CATCHING FLIES");
	if (run)
		vprint(-winWidth / 2 + 20, -winHeight / 2 + 24, GLUT_BITMAP_8_BY_13, "Time : %.2f", counter / 60);
	else
		vprint(-winWidth / 2 + 20, -winHeight / 2 + 24, GLUT_BITMAP_8_BY_13, "Time : %.2f", gameTimer / 60);
}

void gradientBG() {

	glBegin(GL_POLYGON);

	glColor3ub(200, 224, 243);
	glVertex2f(-winWidth / 2, -winHeight / 2);
	glVertex2f(winWidth / 2, -winHeight / 2);

	glColor3ub(182, 209, 241);
	glVertex2f(winWidth / 2, winHeight / 2);
	glVertex2f(-winWidth / 2, winHeight / 2);
	glEnd();
}

void menu() {
	glColor3ub(255, 128, 0);
	glRectf(-winWidth / 2 + 90, winHeight / 2 - 160, winWidth / 2 - 80, winHeight / 2 - 335 - 90);
	glColor3f(1, 1, 1);
	glRectf(-winWidth / 2 + 100, winHeight / 2 - 170, winWidth / 2 - 90, winHeight / 2 - 335 - 80);
	glColor3f(0, 0, 0);
	vprint(winWidth / 2 - 160, winHeight / 2 - 30, GLUT_BITMAP_8_BY_13, "MODE : END PHASE");
	vprint(-winWidth / 2 + 260, winHeight / 2 - 215, GLUT_BITMAP_TIMES_ROMAN_24, "MENU");
	vprint(-winWidth / 2 + 120, winHeight / 2 - 245, GLUT_BITMAP_TIMES_ROMAN_24, "******************************");
	vprint(-winWidth / 2 + 120, winHeight / 2 - 270 - 10, GLUT_BITMAP_TIMES_ROMAN_24, "1. To enter AUTOMATIC phase");
	vprint(-winWidth / 2 + 145, winHeight / 2 - 300 - 10, GLUT_BITMAP_TIMES_ROMAN_24, "press ");

	vprint(-winWidth / 2 + 120, winHeight / 2 - 335 - 20, GLUT_BITMAP_TIMES_ROMAN_24, "2. To enter INTERACTIVE phase");
	vprint(-winWidth / 2 + 145, winHeight / 2 - 365 - 20, GLUT_BITMAP_TIMES_ROMAN_24, "press ");

	glColor3ub(255, 128, 0);
	vprint(-winWidth / 2 + 120, winHeight / 2 - 270 - 10, GLUT_BITMAP_TIMES_ROMAN_24, "1.");
	vprint(-winWidth / 2 + 120, winHeight / 2 - 335 - 20, GLUT_BITMAP_TIMES_ROMAN_24, "2.");
	glColor3f(1, 0, 0);
	vprint(-winWidth / 2 + 145, winHeight / 2 - 300 - 10, GLUT_BITMAP_TIMES_ROMAN_24, "          <F3>");
	vprint(-winWidth / 2 + 145, winHeight / 2 - 365 - 20, GLUT_BITMAP_TIMES_ROMAN_24, "          <F4>");
}

void display() {
	//
	// clear window to black
	//
	glClearColor(146 / 255., 184 / 255., 229 / 255., 0);
	glClear(GL_COLOR_BUFFER_BIT);
	frog.frogX = 0;
	frog.frogY = 0;
	gradientBG();
	border();
	switch (window) {
	case START:
		glColor3f(0, 0, 0);
		vprint(-120, 0, GLUT_BITMAP_9_BY_15, "<Click on screen to start>");
		break;
	case RUN:
		if (run)
			vprint(winWidth / 2 - 160, winHeight / 2 - 30, GLUT_BITMAP_8_BY_13, "MODE : AUTOMATIC");
		else
			vprint(winWidth / 2 - 160, winHeight / 2 - 30, GLUT_BITMAP_8_BY_13, "MODE : INTERACTIVE");
		drawLilypad();
		displayFrog();
		drawTarget();

		glColor3f(0, 0, 0);
		vprint(-winWidth / 2 + 160, winHeight / 2 - 135, GLUT_BITMAP_8_BY_13, "Press <F2> to go to the END MENU");
		glColor3f(1, 0, 0);
		vprint(-winWidth / 2 + 160, winHeight / 2 - 135, GLUT_BITMAP_8_BY_13, "      <F2>");

		angleHit = atan2(fly.flyY - frog.frogY - 15, fly.flyX - frog.frogX) / D2R;
		if (angleHit < 0)
			angleHit += 360;

		glColor3f(1, 0, 0);
		if (angle >= 0 && angle <= 360)
			vprint(frog.frogX - 10, frog.frogY, GLUT_BITMAP_8_BY_13, "%.f", angle);
		else if (angle > 360)
			angle = 0;
		else
			vprint(frog.frogX - 10, frog.frogY, GLUT_BITMAP_8_BY_13, "%.f", angle + 360);

		if (stopTongue == true)
		{
			drawTongueOut();
			//resetTarget();
		}
		else if (stopTongue == false)
			if (run == true)
				drawTongue();
			else if (run == false && spacebar == false)
				drawTongue();


		//interactive mode
		if (!run) //interactive mode manual
		{
			vprint(-winWidth / 2 + 20, -winHeight / 2 + 60, GLUT_BITMAP_8_BY_13, "Hit Angle: %.f", angleHit);
			glColor3f(0, 0, 0);
			if (F1 == false)
			{
				vprint(-winWidth / 2 + 50, winHeight / 2 - 92, GLUT_BITMAP_8_BY_13, "After setting game timer press <F1> to activate challenge mode");
				vprint(-winWidth / 2 + 50, winHeight / 2 - 106, GLUT_BITMAP_8_BY_13, "<Challenge Mode : hit as many flies in the time you've set>");
				vprint(-winWidth / 2 + 150, -winHeight / 2 + 75, GLUT_BITMAP_8_BY_13, "Hit <up> to set game timer up by 10 seconds");
				vprint(-winWidth / 2 + 150, -winHeight / 2 + 60, GLUT_BITMAP_8_BY_13, "Hit <down> to set game timer down by 10 seconds");

				glColor3f(1, 0, 0);
				vprint(-winWidth / 2 + 50, winHeight / 2 - 92, GLUT_BITMAP_8_BY_13, "                               <F1>");
				vprint(-winWidth / 2 + 150, -winHeight / 2 + 75, GLUT_BITMAP_8_BY_13, "    <up>");
				vprint(-winWidth / 2 + 150, -winHeight / 2 + 60, GLUT_BITMAP_8_BY_13, "    <down>");

			}
			glColor3f(0, 0, 0);
			vprint(-winWidth / 2 + 100, winHeight / 2 - 120, GLUT_BITMAP_8_BY_13, "Press <F1> again to pause / resume game timer");
			vprint(-winWidth / 2 + 150, -winHeight / 2 + 90, GLUT_BITMAP_8_BY_13, "Press right / left to rotate and change angle");
			vprint(-winWidth / 2 + 150, -winHeight / 2 + 30, GLUT_BITMAP_8_BY_13, "Hit <spacebar> to eat the fly at the right angle");
			vprint(-winWidth / 2 + 130, -winHeight / 2 + 15, GLUT_BITMAP_8_BY_13, "(as long as angle is within 5 degrees range of hit angle)");

			glColor3f(1, 0, 0);
			vprint(-winWidth / 2 + 150, -winHeight / 2 + 30, GLUT_BITMAP_8_BY_13, "    <spacebar>");
			vprint(-winWidth / 2 + 100, winHeight / 2 - 120, GLUT_BITMAP_8_BY_13, "      <F1>");
			if (F1 == true) {
				glColor3f(1, 0, 0);
				vprint(-winWidth / 2 + 150, -winHeight / 2 + 105, GLUT_BITMAP_8_BY_13, "HIT COUNTER : %.f", flyHit);
			}
			if (spacebar == true)
				drawTongueOutInteractive();

		}
		break;
	case END: menu();
		break;
	}

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ') {
		spacebar = true;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = false;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true;
		if (F1 == false)
			gameTimer += 10 * 60;
		break;
	case GLUT_KEY_DOWN: down = true;
		if (gameTimer > 0 && F1 == false)
			gameTimer -= 10 * 60;
		if (gameTimer < 0)
			gameTimer = 0;
		break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	case GLUT_KEY_F1:
		if (F1 == false)
			F1 = true;
		else F1 = false;
		break;
	case GLUT_KEY_F2:
		F2 = true;
		if (F2)
			window = END;
		break;
	case GLUT_KEY_F3:
		F3 = true;
		if (F3 && window==END)
		{
			window = RUN;
			counter = 3600;
			run = true;
		}
		break;
	case GLUT_KEY_F4:
		F4 = true;
		if (F4 && window == END)
		{
			window = RUN;
			run = false;
		}
		break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.


	if (button == GLUT_LEFT_BUTTON && stat == down)
		window = RUN;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	if (window == RUN) {
		if (counter != 0)
			counter--;
		if (counter == 0)
			run = false;
	}
	if (run == true) {
		if (window == RUN) {
			if ((int)angle >= 0 && (int)angle < 180)
				if ((int)angle < (int)angleHit)
					mode = COUNTERCLOCKWISE;
				else mode = CLOCKWISE;
			else if ((int)angle >= 180 && (int)angle < 360)
				if ((int)angle < (int)angleHit)
					mode = COUNTERCLOCKWISE;
				else mode = CLOCKWISE;

			if (stopTongue == false) {
				if (mode == COUNTERCLOCKWISE)
					angle++;
				else if (mode == CLOCKWISE)
				{
					if (angle < 0)
						angle += 360;
					else angle--;
				}
			}
		}

		if ((int)angle == (int)angleHit)
		{
			stopTongue = true;

		}
		else tongue = false;

		if (stopTongue)
		{
			if (fabs((int)tonguex) < fabs((int)fly.flyX)) //checks if the target is hit
			{
				tonguex += cos(angleHit * D2R) * speed;
				tonguey += sin(angleHit * D2R) * speed;
			}
		}
		else {
			tonguex = 0;
			tonguey = 20;
		}

		if (fabs((int)tonguex) >= fabs((int)fly.flyX)) //if the target is hit then it resets its position
		{
			resetTarget();
			stopTongue = false;
		}

	}
	else //interactive mode
	{
		if (right)
			angle--;
		if (angle < 0)
			angle += 360;
		if (left)
			angle++;

		if ((int)angle < (int)angleHit + 5 && (int)angle >(int)angleHit - 5 && spacebar == true)
		{
			tongue = true;
			resetTarget();
			if (F1 == true)
				flyHit++;
		}
		else tongue = false;
		if (F1 == true)
			gameTimer--;

		if (gameTimer < 0)
		{
			gameTimer = 0;
			F1 = false;
		}
	}



	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()


}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Frog catching flies - CTIS164_HW2");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}