#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <vector>




#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif


#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char* WINDOWTITLE = "OpenGL / GLUT Sample -- Joe Graphics";
const char* GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT,
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	WHITE2,
	RED,
	GREEN,
	BLUE,
	CYAN,
	MAGENTA,
	YELLOW
};

char* ColorNames[] =
{
	(char*)"White",
	(char*)"Red",
	(char*)"Green",
	(char*)"Blue",
	(char*)"Cyan",
	(char*)"Magenta",
	(char*)"Yellow"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 1., 1. },		// white
	{ 1., 0., 0. },		// red
	{ 0., 1., 0. },		// green
	{ 0., 0., 1. },		// blue
	{ 0., 1., 1. },		// cyan
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 0. }		// yellow
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;

// for lighting:

const float	WHITE[] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds
const int MSEC = 22000; //This sets how long the scene goes before restarting...currently 22 seconds.

// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
#define DEMO_Z_FIGHTING
#define DEMO_DEPTH_BUFFER


// non-constant global variables:
float	StepTracker;            // tracks which step we are on
int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		SpotLight = 1;				// turns on off spotlight for bat signal
GLuint	BoxList;				// object display list
GLuint	BuildingList;
GLuint	StandList;
GLuint  ArmsUpLeanRightList;
GLuint	SignalList;
GLuint  SigCircleList;
GLuint	JumpList;
GLuint	PrepJumpList;
GLuint	WalkLeftList;
GLuint	WalkRightList;
GLuint	BatList;
GLuint	WallList;
GLuint  WallTexture;
GLuint	RoadList;
GLuint	ManBat;
GLuint	BatWingList;
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowLight;				// current light type
int		LightColor;				// current light color
bool	Frozen;					// FREEZE false or UNFREEZE = true
int		NowProjection;		// ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
float	x;
float	y;
float	z;
float	temp_time;




// function prototypes:
void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoLightColorMenu(int);
void	DoLightTypeMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoViewMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);

void			Axes(float);
void			HsvRgb(float[3], float[3]);
void			Cross(float[3], float[3], float[3]);
float			Dot(float[3], float[3]);
float			Unit(float[3], float[3]);
float			Unit(float[3]);


// utility to create an array from 3 separate values:

float*
Array3(float a, float b, float c)
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float*
MulArray3(float factor, float array0[])
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float*
MulArray3(float factor, float a, float b, float c)
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}


float
Ranf(float low, float high)
{
	float r = (float)rand();               // 0 - RAND_MAX
	float t = r / (float)RAND_MAX;       // 0. - 1.

	return   low + t * (high - low);
}

// call this if you want to force your program to use
// a different random number sequence every time you run it:
void
TimeOfDaySeed()
{
	struct tm y2k;
	y2k.tm_hour = 0;    y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 2000; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t  now;
	time(&now);
	double seconds = difftime(now, mktime(&y2k));
	unsigned int seed = (unsigned int)(1000. * seconds);    // milliseconds
	srand(seed);
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
#include "osucone.cpp"
#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"
#include "vertexbufferobject.cpp"


Keytimes batMan;
Keytimes Xpos1, Xrot1;//bat man
Keytimes Xpos2, Xrot2;//bat
Keytimes Ypos1, Yrot1;//bat man
Keytimes Ypos2, Yrot2;//bat
Keytimes Zpos1, Zrot1;//bat man
Keytimes Zpos2, Zrot2;//bat
Keytimes WingX; //batwing x pos
Keytimes WingY; //batwing y pos
Keytimes WingZ; //batwing z pos
Keytimes WingRotY;//batwing y rotation
Keytimes SigOn;//turn on bat signal
Keytimes SigX;//bat signal point at x -- also coordinates for the symbol
Keytimes SigY;//bat signal point at y -- also coordinates for the symbol
Keytimes SigZ;//bat signal point at z -- also coordinates for the symbol
Keytimes SpotX;
Keytimes SpotY;
Keytimes SpotZ;
Keytimes Symbol;//symbol appears timing
Keytimes EyeX;
Keytimes EyeY;
Keytimes EyeZ;




// main program:

int
main(int argc, char* argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit(&argc, argv);

	// setup all the graphics stuff:

	InitGraphics();

	// create the display lists that **will not change**:

	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset();

	// setup all the user interface stuff:

	InitMenus();

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow(MainWindow);
	glutMainLoop();

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate()
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
	if (DepthBufferOn == 0)
		glDisable(GL_DEPTH_TEST);
#endif


	// specify shading to be flat:

	glShadeModel(GL_FLAT);

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (NowProjection == ORTHO)
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	else
		gluPerspective(70.f, 1.f, 0.1f, 1000.f);

	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set the eye position, look-at position, and up-vector:

	gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	// rotate the scene:

	glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);

	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}

	// possibly draw the axes:


	glEnable(GL_NORMALIZE);
	int msec = glutGet(GLUT_ELAPSED_TIME) % MSEC;
	float nowTime = (float)msec / 1000.;
	//make view into time coordinates
	gluLookAt(EyeX.GetValue(nowTime), EyeY.GetValue(nowTime), EyeZ.GetValue(nowTime), 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);




	//make spotlight on and spotlight position into time coordinates
	glEnable(GL_LIGHTING);


	SetPointLight(GL_LIGHT0, 0, 50, 20, 1, 1, 1);
	SetSpotLight(GL_LIGHT1, 100, 250, 0, SpotX.GetValue(nowTime), SpotY.GetValue(nowTime), SpotZ.GetValue(nowTime), 1, 1, 0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT0);
	
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glShadeModel(GL_SMOOTH);

	
	
	float batBodTracker = batMan.GetValue(nowTime); //checks batMan which sets which body position to use at what time 
	
	//draw Bat Man
	if (Ypos1.GetValue(nowTime) > -1.5) { // batman dissappers when he gets in the bat wing
		glPushMatrix();
		glTranslatef(Xpos1.GetValue(nowTime), Ypos1.GetValue(nowTime), Zpos1.GetValue(nowTime));
		glRotatef(Yrot1.GetValue(nowTime), 0.0f, 1.0f, 0.0f);
		if ((batBodTracker <= 1.5) && (batBodTracker > 0)) {
			glCallList(WalkRightList);
		}
		else if (batBodTracker > 1.5) {
			glCallList(WalkLeftList);
		}
		else {
			glCallList(StandList);
		}
		glPopMatrix();
	}

	//draw bat symbol in signal spotlight
	if (SigOn.GetValue(nowTime) > 1) {
		glPushMatrix();
			glTranslatef(SigX.GetValue(nowTime), SigY.GetValue(nowTime), SigZ.GetValue(nowTime));
			glCallList(SigCircleList);
			glCallList(SignalList);
		glPopMatrix();
	}

	//draw buildings and cityscape

	//buildings
	glCallList(BuildingList);
	//back wall
	glPushMatrix();
		glTranslatef(0., 100., -600.);
		glRotatef(90., 1., 0., 0.);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, WallTexture);
		glCallList(WallList);
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	//Road
	glPushMatrix();
		glTranslatef(0., -.1, 0.);
		glCallList(RoadList);//need for reflection..building obj doesn't have reflective properties
	glPopMatrix();
	

	//draw bat swarm
	glPushMatrix();
	glTranslatef(Xpos2.GetValue(nowTime), Ypos2.GetValue(nowTime), Zpos2.GetValue(nowTime));
	glRotatef(Xrot2.GetValue(nowTime), 1., 0., 0.); //swoops x the bats
	glRotatef(Zrot2.GetValue(nowTime), 0., 0., 1.); // swirls z the bats 
	glCallList(BatList);
	glPopMatrix();

	//draw bat wing vehicle
	glPushMatrix();
	glTranslatef(WingX.GetValue(nowTime), WingY.GetValue(nowTime), WingZ.GetValue(nowTime));
	glRotatef(WingRotY.GetValue(nowTime), 0., 1., 0.);
	glCallList(BatWingList);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	


	// draw the box object by calling up its display list:

	//glCallList( BoxList );


#ifdef DEMO_Z_FIGHTING
	if (DepthFightingOn != 0)
	{
		glPushMatrix();
		glRotatef(90.f, 0.f, 1.f, 0.f);
		glCallList(BoxList);
		glPopMatrix();
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable(GL_DEPTH_TEST);
	glColor3f(0.f, 1.f, 1.f);
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.f, 1.f, 1.f);
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers();

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();
}


void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	NowColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoLightTypeMenu(int id)
{
	NowLight = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoLightColorMenu(int id)
{
	LightColor = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{

	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	NowProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);

	//Bat man positions
	Xpos1.Init(); // bat man x
	Xpos1.AddTimeValue(0.0, 0.000);
	Xpos1.AddTimeValue(0.5, 0.000);
	Xpos1.AddTimeValue(1.0, 0.000);
	Xpos1.AddTimeValue(2.0, 0.000);
	Xpos1.AddTimeValue(2.5, 0.000);
	Xpos1.AddTimeValue(3.0, 0.000);
	Xpos1.AddTimeValue(3.5, 0.000);
	Xpos1.AddTimeValue(4.0, 0.000);
	Xpos1.AddTimeValue(4.5, 0.000);
	Xpos1.AddTimeValue(5.0, 0.000);
	Xpos1.AddTimeValue(5.5, 0.000);
	Xpos1.AddTimeValue(6.0, 0.000);
	Xpos1.AddTimeValue(6.5, 0.000);
	Xpos1.AddTimeValue(7.0, 0.000);
	Xpos1.AddTimeValue(7.5, 0.000);
	Xpos1.AddTimeValue(8.0, 0.000);
	Xpos1.AddTimeValue(8.5, 0.000);
	Xpos1.AddTimeValue(9.0, 0.000);//stop and turn to look at symbol
	Xpos1.AddTimeValue(9.5, 0.000);
	Xpos1.AddTimeValue(10.0, 0.000);
	Xpos1.AddTimeValue(10.5, 0.000);
	Xpos1.AddTimeValue(11.0, 0.000);
	Xpos1.AddTimeValue(11.5, 0.000);
	Xpos1.AddTimeValue(18.0, 0.000);//Turning back to jet
	Xpos1.AddTimeValue(18.5, 0.000);
	Xpos1.AddTimeValue(19.0, 0.000);//getting in jet
	Xpos1.AddTimeValue(20.0, 0.000);
	Xpos1.AddTimeValue(21.0, 0.000);
	Xpos1.AddTimeValue(22.0, 0.000);

	Ypos1.Init();//bat man y
	Ypos1.AddTimeValue(0.0, -1);
	Ypos1.AddTimeValue(.5, 1);
	Ypos1.AddTimeValue(1.0, -1);
	Ypos1.AddTimeValue(1.5, 1);
	Ypos1.AddTimeValue(2.0, -1);
	Ypos1.AddTimeValue(2.5, 1);
	Ypos1.AddTimeValue(3.0, -1);
	Ypos1.AddTimeValue(3.5, 1);
	Ypos1.AddTimeValue(4.0, -1);
	Ypos1.AddTimeValue(4.5, 1);
	Ypos1.AddTimeValue(5.0, -1);
	Ypos1.AddTimeValue(5.5, 1);
	Ypos1.AddTimeValue(6.0, -1);
	Ypos1.AddTimeValue(6.5, 1);
	Ypos1.AddTimeValue(7.0, -1);
	Ypos1.AddTimeValue(7.5, 1);
	Ypos1.AddTimeValue(8.0, -1);
	Ypos1.AddTimeValue(8.5, 1);
	Ypos1.AddTimeValue(9.0, -1);//stop and turn to look at symbol
	Ypos1.AddTimeValue(9.5, -1);
	Ypos1.AddTimeValue(10.0, -1);
	Ypos1.AddTimeValue(10.5, -1);
	Ypos1.AddTimeValue(11.0, -1);
	Ypos1.AddTimeValue(11.5, -1);
	Ypos1.AddTimeValue(18.0, -1);//Turning back to jet
	Ypos1.AddTimeValue(18.5, -5);
	Ypos1.AddTimeValue(19.0, -5);//getting in jet
	Ypos1.AddTimeValue(20.0, -5);
	Ypos1.AddTimeValue(21.0, -5);
	Ypos1.AddTimeValue(22.0, -5);

	Zpos1.Init();// bat man Z
	Zpos1.AddTimeValue(0.0, -200.);
	Zpos1.AddTimeValue(.5, -190.);
	Zpos1.AddTimeValue(1.0, -180.);
	Zpos1.AddTimeValue(1.5, -170.);
	Zpos1.AddTimeValue(2.0, -160.);
	Zpos1.AddTimeValue(2.5, -150.);
	Zpos1.AddTimeValue(3.0, -140.);
	Zpos1.AddTimeValue(3.5, -130.);
	Zpos1.AddTimeValue(4.0, -120.);
	Zpos1.AddTimeValue(4.5, -110.);
	Zpos1.AddTimeValue(5.0, -100.);
	Zpos1.AddTimeValue(5.5, -90.);
	Zpos1.AddTimeValue(6.0, -80.);
	Zpos1.AddTimeValue(6.5, -70.);
	Zpos1.AddTimeValue(7.0, -60.);
	Zpos1.AddTimeValue(7.5, -50.);
	Zpos1.AddTimeValue(8.0, -40.);
	Zpos1.AddTimeValue(8.5, -30.);
	Zpos1.AddTimeValue(9.0, -20.);//stop and turn to look at symbol
	Zpos1.AddTimeValue(9.5, -20.);
	Zpos1.AddTimeValue(10.0, -20.);
	Zpos1.AddTimeValue(10.5, -20.);
	Zpos1.AddTimeValue(11.0, -20.);
	Zpos1.AddTimeValue(16.0, -20.);
	Zpos1.AddTimeValue(16.5, -10.);
	Zpos1.AddTimeValue(17.0, 5.);
	Zpos1.AddTimeValue(17.5, 15.);
	Zpos1.AddTimeValue(18.0, 15.);//Turning back to jet
	Zpos1.AddTimeValue(18.5, 15.);
	Zpos1.AddTimeValue(19.0, 15.);//getting in jet
	Zpos1.AddTimeValue(20.0, 15.);
	Zpos1.AddTimeValue(21.0, 15.);
	Zpos1.AddTimeValue(22.0, 15.);

	Yrot1.Init();//bat man
	Yrot1.AddTimeValue(0.0, 0); //Walking
	Yrot1.AddTimeValue(0.5, 0);
	Yrot1.AddTimeValue(1.0, 0);
	Yrot1.AddTimeValue(1.5, 0);
	Yrot1.AddTimeValue(2.0, 0);
	Yrot1.AddTimeValue(2.5, 0);
	Yrot1.AddTimeValue(3.0, 0);
	Yrot1.AddTimeValue(3.5, 0);
	Yrot1.AddTimeValue(4.0, 0);
	Yrot1.AddTimeValue(4.5, 0);
	Yrot1.AddTimeValue(5.0, 0);
	Yrot1.AddTimeValue(5.5, 0);
	Yrot1.AddTimeValue(6.0, 0);
	Yrot1.AddTimeValue(6.5, 0);
	Yrot1.AddTimeValue(7.0, 0);
	Yrot1.AddTimeValue(7.5, 0);
	Yrot1.AddTimeValue(8.0, 0);
	Yrot1.AddTimeValue(8.5, 0);
	Yrot1.AddTimeValue(9.0, 0);
	Yrot1.AddTimeValue(9.5, 45); //Turning
	Yrot1.AddTimeValue(10.0, 90);
	Yrot1.AddTimeValue(10.5, 180);//Looking
	Yrot1.AddTimeValue(11.0, 180);
	Yrot1.AddTimeValue(11.5, 180.);
	Yrot1.AddTimeValue(14.5, 180.);
	Yrot1.AddTimeValue(15.0, 180.);
	Yrot1.AddTimeValue(15.5, 0.);
	Yrot1.AddTimeValue(16.0, 0.);
	Yrot1.AddTimeValue(16.5, 0.);
	Yrot1.AddTimeValue(17.0, 0.);
	Yrot1.AddTimeValue(17.5, 0.);
	Yrot1.AddTimeValue(18.0, 0.);//Turning back to jet
	Yrot1.AddTimeValue(18.5, 0.);
	Yrot1.AddTimeValue(19.0, 0.);//getting in jet
	Yrot1.AddTimeValue(20.0, 0.);
	Yrot1.AddTimeValue(21.0, 0.);
	Yrot1.AddTimeValue(22.0, 0.);

	batMan.Init();//bat man body selection list
	batMan.AddTimeValue(0.0, 0.000);//Walking body (right leg body, and left leg body)
	batMan.AddTimeValue(.5, 2.000);
	batMan.AddTimeValue(1.0, 4.000);
	batMan.AddTimeValue(1.5, 0.000);
	batMan.AddTimeValue(2.0, 4.000);
	batMan.AddTimeValue(2.5, 0.000);
	batMan.AddTimeValue(3.0, 4.000);
	batMan.AddTimeValue(3.5, 0.000);
	batMan.AddTimeValue(4.0, 4.000);
	batMan.AddTimeValue(4.5, 0.000);
	batMan.AddTimeValue(5.0, 4.000);
	batMan.AddTimeValue(5.5, 0.000);
	batMan.AddTimeValue(6.0, 4.000);
	batMan.AddTimeValue(6.5, 0.000);
	batMan.AddTimeValue(7.0, 4.000);
	batMan.AddTimeValue(7.5, 0.000);
	batMan.AddTimeValue(8.0, 4.000);
	batMan.AddTimeValue(8.5, 0.000);
	batMan.AddTimeValue(9.0, 4.000);
	batMan.AddTimeValue(9.5, 0.000);//turning standing pose
	batMan.AddTimeValue(10.0, -5.00);
	batMan.AddTimeValue(10.5, -5.00);
	batMan.AddTimeValue(11.0, -5.00);
	batMan.AddTimeValue(11.5, -5.00);
	batMan.AddTimeValue(16.0, 4.000);
	batMan.AddTimeValue(16.5, 0.000);
	batMan.AddTimeValue(17.0, 4.000);
	batMan.AddTimeValue(17.5, 0.000);
	batMan.AddTimeValue(18.0, -5.00);//Turning back to jet
	batMan.AddTimeValue(18.5, -5.00);
	batMan.AddTimeValue(19.0, -5.00);//getting in jet
	batMan.AddTimeValue(20.0, -20.00);
	batMan.AddTimeValue(21.0, -20.00);
	batMan.AddTimeValue(22.0, -20.00);


	///Bat symbol object drawn for spotlight
	Symbol.Init();//bat symbol time value for when to draw
	Symbol.AddTimeValue(0.0, 0.000);
	Symbol.AddTimeValue(5.0, 0.00);
	Symbol.AddTimeValue(7.0, 0.000);
	Symbol.AddTimeValue(8.0, 0.000);
	Symbol.AddTimeValue(9.0, 4.000);//Symbol appears at 9
	Symbol.AddTimeValue(10.0, 4.000);
	Symbol.AddTimeValue(11.0, 4.000);
	Symbol.AddTimeValue(13.0, 4.000);
	Symbol.AddTimeValue(14.0, 4.000);
	Symbol.AddTimeValue(15.0, 4.000);
	Symbol.AddTimeValue(16.0, 4.000);
	Symbol.AddTimeValue(18.0, 4.000);
	Symbol.AddTimeValue(19.0, 4.000);//getting in jet
	Symbol.AddTimeValue(20.0, 4.000);
	Symbol.AddTimeValue(21.0, 4.000);
	Symbol.AddTimeValue(22.0, 4.000);

	SigX.Init();//bat symbol x value
	SigX.AddTimeValue(0.0, 15.);
	SigX.AddTimeValue(5.0, 15.);
	SigX.AddTimeValue(7.0, 15.);
	SigX.AddTimeValue(8.0, 15.);
	SigX.AddTimeValue(9.0, 15.);//Symbol appears at 9 // spotlight turns off so bat symbol colors can show
	SigX.AddTimeValue(10.0, 15.);
	SigX.AddTimeValue(11.0, 15.);
	SigX.AddTimeValue(13.0, 15.);
	SigX.AddTimeValue(14.0, 15.);
	SigX.AddTimeValue(15.0, 15.);
	SigX.AddTimeValue(16.0, 15.);
	SigX.AddTimeValue(18.0, 15.);
	SigX.AddTimeValue(19.0, 15.);//getting in jet
	SigX.AddTimeValue(20.0, 15.);
	SigX.AddTimeValue(21.0, 15.);
	SigX.AddTimeValue(22.0, 15.);


	SigY.Init();//bat symbol y value
	SigY.AddTimeValue(0.0, 90.);
	SigY.AddTimeValue(7.0, 90.);
	SigY.AddTimeValue(8.0, 110.);
	SigY.AddTimeValue(9.0, 110.);//Symbol appears at 9
	SigY.AddTimeValue(10.0, 110.);
	SigY.AddTimeValue(11.0, 110.);
	SigY.AddTimeValue(13.0, 110.);
	SigY.AddTimeValue(14.0, 110.);
	SigY.AddTimeValue(15.0, 110.);
	SigY.AddTimeValue(16.0, 110.);
	SigY.AddTimeValue(18.0, 110.);
	SigY.AddTimeValue(19.0, 110.);//getting in jet
	SigY.AddTimeValue(20.0, 110.);
	SigY.AddTimeValue(21.0, 110.);
	SigY.AddTimeValue(22.0, 110.);

	SigZ.Init();//bat symbol z value
	SigZ.AddTimeValue(0.0, -550.);
	SigZ.AddTimeValue(5.0, -550.);
	SigZ.AddTimeValue(7.0, -550.);
	SigZ.AddTimeValue(8.0, -550.);
	SigZ.AddTimeValue(9.0, -600.);//Symbol appears at 9
	SigZ.AddTimeValue(9.25, -599.);//Symbol appears at 9
	SigZ.AddTimeValue(9.5, -590.);//Symbol appears at 9
	SigZ.AddTimeValue(9.75, -570.);//Symbol appears at 9
	SigZ.AddTimeValue(10.0, -550.);
	SigZ.AddTimeValue(11.0, -550.);
	SigZ.AddTimeValue(13.0, -550.);
	SigZ.AddTimeValue(14.0, -550.);
	SigZ.AddTimeValue(15.0, -550.);
	SigZ.AddTimeValue(16.0, -550.);
	SigZ.AddTimeValue(18.0, -550.);
	SigZ.AddTimeValue(19.0, -550.);//getting in jet
	SigZ.AddTimeValue(20.0, -550.);
	SigZ.AddTimeValue(21.0, -550.);
	SigZ.AddTimeValue(22.0, -550.);

	//Bat signal spotlight actual light portion
	SigOn.Init();//bat signal time value for when to turn on and off
	SigOn.AddTimeValue(0.0, 0.000); // turn on spotlight
	SigOn.AddTimeValue(5.0, 0.00);
	SigOn.AddTimeValue(7.0, 0.000);
	SigOn.AddTimeValue(8.0, 0.000);
	SigOn.AddTimeValue(9.0, 4.000);//turn off spotlight at 9 so symbol will show
	SigOn.AddTimeValue(10.0, 4.000);
	SigOn.AddTimeValue(11.0, 4.000);
	SigOn.AddTimeValue(13.0, 4.000);
	SigOn.AddTimeValue(14.0, 4.000);
	SigOn.AddTimeValue(15.0, 4.000);
	SigOn.AddTimeValue(16.0, 4.000);
	SigOn.AddTimeValue(18.0, 4.000);
	SigOn.AddTimeValue(19.0, 4.000);//getting in jet
	SigOn.AddTimeValue(20.0, 4.000);
	SigOn.AddTimeValue(21.0, 4.000);
	SigOn.AddTimeValue(22.0, 4.000);

	SpotX.Init();//bat spotlight x value
	SpotX.AddTimeValue(0.0, -75.);  // spotlight tracking to wall
	SpotX.AddTimeValue(5.0, -75.);
	SpotX.AddTimeValue(7.0, -150.);
	SpotX.AddTimeValue(8.0, -75.);
	SpotX.AddTimeValue(9.0, -75.);//Symbol appears at 9 // spotlight turns off so bat symbol colors can show
	SpotX.AddTimeValue(10.0, -75.);
	SpotX.AddTimeValue(11.0, -75.);
	SpotX.AddTimeValue(13.0, -75.);
	SpotX.AddTimeValue(14.0, -75.);
	SpotX.AddTimeValue(15.0, -75.);
	SpotX.AddTimeValue(16.0, -75.);
	SpotX.AddTimeValue(18.0, -75.);
	SpotX.AddTimeValue(19.0, -75.);//getting in jet
	SpotX.AddTimeValue(20.0, -75.);
	SpotX.AddTimeValue(21.0, -75.);
	SpotX.AddTimeValue(22.0, -75.);


	SpotY.Init();//bat spotlight y value
	SpotY.AddTimeValue(0.0, -250); // spotlight tracking to wall
	SpotY.AddTimeValue(5.0, -250);
	SpotY.AddTimeValue(7.0, -250.);
	SpotY.AddTimeValue(8.0, -150.);
	SpotY.AddTimeValue(9.0, -125.);//Symbol appears at 9 // spotlight turns off so bat symbol colors can show
	SpotY.AddTimeValue(10.0, -125.);
	SpotY.AddTimeValue(11.0, -125.);
	SpotY.AddTimeValue(13.0, -125.);
	SpotY.AddTimeValue(14.0, -125.);
	SpotY.AddTimeValue(15.0, -125.);
	SpotY.AddTimeValue(16.0, -125.);
	SpotY.AddTimeValue(18.0, -125.);
	SpotY.AddTimeValue(19.0, -125.);//getting in jet
	SpotY.AddTimeValue(20.0, -125.);
	SpotY.AddTimeValue(21.0, -125.);
	SpotY.AddTimeValue(22.0, -125.);

	SpotZ.Init();//bat spotlightl z value
	SpotZ.AddTimeValue(0.0, -150.); // spotlight tracking to wall
	SpotZ.AddTimeValue(5.0, -350.);
	SpotZ.AddTimeValue(7.0, -450.);
	SpotZ.AddTimeValue(8.0, -650);
	SpotZ.AddTimeValue(9.0, -550.);//Symbol appears at 9
	SpotZ.AddTimeValue(10.0, -550.);
	SpotZ.AddTimeValue(11.0, -550.);
	SpotZ.AddTimeValue(13.0, -550.);
	SpotZ.AddTimeValue(14.0, -550.);
	SpotZ.AddTimeValue(15.0, -550.);
	SpotZ.AddTimeValue(16.0, -550.);
	SpotZ.AddTimeValue(18.0, -550.);
	SpotZ.AddTimeValue(19.0, -550.);//getting in jet
	SpotZ.AddTimeValue(20.0, -550.);
	SpotZ.AddTimeValue(21.0, -550.);
	SpotZ.AddTimeValue(22.0, -550.);

	Zrot2.Init();//Bat Swarm z rotation
	Zrot2.AddTimeValue(0.0, 0.000);
	Zrot2.AddTimeValue(0.5, 0.000);
	Zrot2.AddTimeValue(2.0, 45);
	Zrot2.AddTimeValue(5.0, 90);
	Zrot2.AddTimeValue(8.0, 135);
	Zrot2.AddTimeValue(10.0, 180);
	Zrot2.AddTimeValue(12.0, 225.00);
	Zrot2.AddTimeValue(14.0, 270.000);
	Zrot2.AddTimeValue(16.0, 315.000);
	Zrot2.AddTimeValue(18.0, 0.000);
	Zrot2.AddTimeValue(19.0, 0.000);//getting in jet
	Zrot2.AddTimeValue(20.0, 0.000);
	Zrot2.AddTimeValue(21.0, 0.000);
	Zrot2.AddTimeValue(22.0, 0.000);

	Xrot2.Init();// Bat Swarm x rotation
	Xrot2.AddTimeValue(0.0, 0.000);
	Xrot2.AddTimeValue(0.5, 0.000);
	Xrot2.AddTimeValue(2.0, 0.000);
	Xrot2.AddTimeValue(5.0, 0.000);
	Xrot2.AddTimeValue(8.0, 0.000);
	Xrot2.AddTimeValue(10.0, 0.000);
	Xrot2.AddTimeValue(12.0, 0.000);
	Xrot2.AddTimeValue(14.0, 0.000);
	Xrot2.AddTimeValue(16.0, 0.000);
	Xrot2.AddTimeValue(18.0, 0.000);
	Xrot2.AddTimeValue(19.0, 0.000);//getting in jet
	Xrot2.AddTimeValue(20.0, 0.000);
	Xrot2.AddTimeValue(21.0, 0.000);
	Xrot2.AddTimeValue(22.0, 0.000);

	Xpos2.Init(); //Bat Swarm x value
	Xpos2.AddTimeValue(0.0, 0.000);
	Xpos2.AddTimeValue(0.5, 0.000);
	Xpos2.AddTimeValue(2.0, -1.000);
	Xpos2.AddTimeValue(5.0, 0.000);
	Xpos2.AddTimeValue(8.0, 2.000);
	Xpos2.AddTimeValue(10.0, 3.000);
	Xpos2.AddTimeValue(12.0, 2.000);
	Xpos2.AddTimeValue(14.0, 0.000);
	Xpos2.AddTimeValue(16.0, 0.000);
	Xpos2.AddTimeValue(18.0, 0.000);
	Xpos2.AddTimeValue(19.0, 0.000);//getting in jet
	Xpos2.AddTimeValue(20.0, 0.000);
	Xpos2.AddTimeValue(21.0, 0.000);
	Xpos2.AddTimeValue(22.0, 0.000);

	Ypos2.Init();//Bat Swarm y value
	Ypos2.AddTimeValue(0.0, 75.000);
	Ypos2.AddTimeValue(0.5, 75.000);
	Ypos2.AddTimeValue(2.0, 75.000);
	Ypos2.AddTimeValue(5.0, 50.000);
	Ypos2.AddTimeValue(8.0, 50.00);
	Ypos2.AddTimeValue(10.0, 40.00);
	Ypos2.AddTimeValue(12.0, 50.00);
	Ypos2.AddTimeValue(14.0, 50.000);
	Ypos2.AddTimeValue(16.0, 75.000);
	Ypos2.AddTimeValue(18.0, 75.000);
	Ypos2.AddTimeValue(19.0, 100.000);//getting in jet
	Ypos2.AddTimeValue(20.0, 200.00);
	Ypos2.AddTimeValue(21.0, 200.00);
	Ypos2.AddTimeValue(22.0, 200.00);

	Zpos2.Init();// Bat Swarm z value
	Zpos2.AddTimeValue(0.0, -250.000);
	Zpos2.AddTimeValue(0.5, -200.000);
	Zpos2.AddTimeValue(2.0, -200.000);
	Zpos2.AddTimeValue(5.0, -150.000);
	Zpos2.AddTimeValue(8.0, -100.000);
	Zpos2.AddTimeValue(10.0, -75.000);
	Zpos2.AddTimeValue(12.0, -20.00);
	Zpos2.AddTimeValue(14.0, -20.000);
	Zpos2.AddTimeValue(16.0, -20.000);
	Zpos2.AddTimeValue(18.0, -20.000);
	Zpos2.AddTimeValue(19.0, -20.000);//getting in jet
	Zpos2.AddTimeValue(20.0, -10.000);
	Zpos2.AddTimeValue(21.0, -5.000);
	Zpos2.AddTimeValue(22.0, -5.000);

	EyeX.Init(); //eye x value
	EyeX.AddTimeValue(0.0, -10.f);
	EyeX.AddTimeValue(0.5, -10.f);
	EyeX.AddTimeValue(2.0, -10.f);
	EyeX.AddTimeValue(5.0, -10.f);
	EyeX.AddTimeValue(8.0, -10.f);
	EyeX.AddTimeValue(10.0, -10.f);
	EyeX.AddTimeValue(12.0, -10.f);
	EyeX.AddTimeValue(14.0, -10.f);
	EyeX.AddTimeValue(16.0, -10.f);
	EyeX.AddTimeValue(18.0, -10.f);
	EyeX.AddTimeValue(19.0, -10.f);//getting in jet
	EyeX.AddTimeValue(20.0, -10.f);
	EyeX.AddTimeValue(21.0, -10.f);
	EyeX.AddTimeValue(22.0, -10.f);

	EyeY.Init();//eye y value
	EyeY.AddTimeValue(0.0, 10.f);
	EyeY.AddTimeValue(0.5, 10.f);
	EyeY.AddTimeValue(2.0, 10.f);
	EyeY.AddTimeValue(5.0, 10.f);
	EyeY.AddTimeValue(8.0, 10.f);
	EyeY.AddTimeValue(10.0, 10.f);
	EyeY.AddTimeValue(12.0, 50.f);
	EyeY.AddTimeValue(14.0, 50.f);
	EyeY.AddTimeValue(16.0, 50.f);
	EyeY.AddTimeValue(18.0, 50.f);
	EyeY.AddTimeValue(19.0, 50.f);//getting in jet
	EyeY.AddTimeValue(20.0, 50.f);
	EyeY.AddTimeValue(21.0, 50.f);
	EyeY.AddTimeValue(22.0, 50.f);

	EyeZ.Init();// eye z value
	EyeZ.AddTimeValue(0.0, 50.f);
	EyeZ.AddTimeValue(0.5, 50.f);
	EyeZ.AddTimeValue(2.0, 50.f);
	EyeZ.AddTimeValue(5.0, 50.f);
	EyeZ.AddTimeValue(8.0, 50.f);
	EyeZ.AddTimeValue(10.0, 50.f);
	EyeZ.AddTimeValue(12.0, 100.f);
	EyeZ.AddTimeValue(14.0, 150.f);
	EyeZ.AddTimeValue(16.0, 150.f);
	EyeZ.AddTimeValue(18.0, 150.f);
	EyeZ.AddTimeValue(19.0, 150.f);//getting in jet
	EyeZ.AddTimeValue(20.0, 150.f);
	EyeZ.AddTimeValue(21.0, 150.f);
	EyeZ.AddTimeValue(22.0, 150.f);

	WingX.Init(); //BatWing x value
	WingX.AddTimeValue(0.0, 0.00);
	WingX.AddTimeValue(0.5, 0.00);
	WingX.AddTimeValue(2.0, 0.00);
	WingX.AddTimeValue(5.0, 0.00);
	WingX.AddTimeValue(8.0, 0.00);
	WingX.AddTimeValue(10.0, 0.00);
	WingX.AddTimeValue(12.0, 0.00);
	WingX.AddTimeValue(14.0, 0.00);
	WingX.AddTimeValue(16.0, 0.00);
	WingX.AddTimeValue(18.0, 0.00);
	WingX.AddTimeValue(19.0, 0.00);//getting in jet
	WingX.AddTimeValue(20.0, 0.00);
	WingX.AddTimeValue(21.0, 0.00);
	WingX.AddTimeValue(22.0, 0.00);

	WingY.Init();//BatWing y value
	WingY.AddTimeValue(0.0, 300.);
	WingY.AddTimeValue(0.5, 300.);
	WingY.AddTimeValue(2.0, 300.);
	WingY.AddTimeValue(5.0, 300.);
	WingY.AddTimeValue(8.0, 300.);
	WingY.AddTimeValue(10.0, 200.);
	WingY.AddTimeValue(12.0, 100.);
	WingY.AddTimeValue(14.0, 10.);
	WingY.AddTimeValue(16.0, 1.);
	WingY.AddTimeValue(18.0, 1.);
	WingY.AddTimeValue(19.0, 1.);//getting in jet
	WingY.AddTimeValue(20.0, 50.);
	WingY.AddTimeValue(21.0, 300.00);
	WingY.AddTimeValue(22.0, 300.00);

	WingZ.Init();//BatWing z value
	WingZ.AddTimeValue(0.0, 100.);
	WingZ.AddTimeValue(0.5, 100.);
	WingZ.AddTimeValue(2.0, 100.);
	WingZ.AddTimeValue(5.0, 100.);
	WingZ.AddTimeValue(8.0, 75.);
	WingZ.AddTimeValue(10.0, 75.);
	WingZ.AddTimeValue(12.0, 50.);
	WingZ.AddTimeValue(14.0, 40.);
	WingZ.AddTimeValue(16.0, 40.);
	WingZ.AddTimeValue(18.0, 40.);
	WingZ.AddTimeValue(19.0, 40.00);//getting in jet
	WingZ.AddTimeValue(20.0, -50.00);
	WingZ.AddTimeValue(21.0, -150.00);
	WingZ.AddTimeValue(22.0, -600.00);

	WingRotY.Init();
	WingRotY.AddTimeValue(0.0, 0.00);
	WingRotY.AddTimeValue(0.5, 0.00);
	WingRotY.AddTimeValue(2.0, 0.00);
	WingRotY.AddTimeValue(5.0, 0.00);
	WingRotY.AddTimeValue(8.0, 0.00);
	WingRotY.AddTimeValue(10.0, 0.00);
	WingRotY.AddTimeValue(12.0, 0.00);
	WingRotY.AddTimeValue(14.0, 0.00);
	WingRotY.AddTimeValue(16.0, 0.00);
	WingRotY.AddTimeValue(18.0, 0.00);
	WingRotY.AddTimeValue(19.0, 0.00);//getting in jet
	WingRotY.AddTimeValue(20.0, 180);
	WingRotY.AddTimeValue(21.0, 180);
	WingRotY.AddTimeValue(22.0, 180);



	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc(Animate);

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:
	int width, height;
	char* file = (char*)"greybrick2.bmp";
	unsigned char* texture = BmpToTexture(file, &width, &height);
	if (texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", file, width, height);

	glGenTextures(1, &WallTexture);
	glBindTexture(GL_TEXTURE_2D, WallTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	glutSetWindow(MainWindow);

	// create the object:




	// create the axes:

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();

	// create the grid
	float XSIDE = 300.f;			// length of the x side of the grid
	#define X0      (-XSIDE/2.)		// where one side starts
	#define NX	100			// how many points in x
	#define DX	( XSIDE/(float)NX )	// change in x between the points

	#define YGRID	0.f			// y-height of the grid

	float ZSIDE = 400.f;			// length of the z side of the grid
	#define Z0      (-ZSIDE/2.)		// where one side starts
	#define NZ	100			// how many points in z
	#define DZ	( ZSIDE/(float)NZ )	// change in z between the points

	WallList = glGenLists(1);
	glNewList(WallList, GL_COMPILE);
	glPushMatrix();
		SetMaterial(0.f, 0.f, 0.f, 80.f);

		glNormal3f(0., 1., 0.);

		for (int i = 0; i < NZ; i++) {
			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j < NX; j++) {
				float s = (float)j / (NX - 1);
				float t = (float)i / (NZ - 1);
				glTexCoord2f(s, t);
				glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 0));
				glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 1));
			}
			glEnd();
		}

	glPopMatrix();
	glEndList();

	RoadList = glGenLists(1);
	glNewList(RoadList, GL_COMPILE);
	glPushMatrix();
	// or whatever else you want
	SetMaterial(0.2f, 0.2f, 0.2f, 80.f);
	glNormal3f(0., 1., 0.);
	XSIDE = 110;
	ZSIDE = 1040;
	
	for (int i = 0; i < NZ; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NX; j++)
		{
			float s = (float)j / (NX - 1) * (XSIDE / (float)XSIDE); 
			float t = (float)i / (NZ - 1) * (ZSIDE / (float)ZSIDE);
			glTexCoord2f(s, t);
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 0));
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 1));
		}
		glEnd();
	}
	glPopMatrix();
	glEndList();

	BuildingList = glGenLists(1);
	glNewList(BuildingList, GL_COMPILE);
	glPushMatrix();
		//glScalef(0.1f, 0.1f, 0.1f);
		glTranslatef(0.0f, 0.0f, -10.0f);
		glRotatef(180, 0, 1, 0);
		SetMaterial(0.3f, 0.3f, 0.3f, 100.f);
		LoadObjFile("street.obj");
		SetMaterial(1.0f, 1.0f, 1.0f, 1000.f);
		LoadObjFile("streetLights.obj");
	glPopMatrix();
	glEndList();

	StandList = glGenLists(1);
	glNewList(StandList, GL_COMPILE);
	glPushMatrix();
		glScalef(0.5f, 0.5f, 0.5f);
		SetMaterial(0.2f, 0.2f, 0.2f, 80.f);
		LoadObjFile("batBlackBod.obj");
		SetMaterial(1.0f, 1.0f, 1.0f, 80.f);
		LoadObjFile("headTorso.obj");
		SetMaterial(1.0f, 1.0f, 0.0f, 80.f);
		LoadObjFile("yellowBelt.obj");
	glPopMatrix();
	glEndList();

	BatWingList = glGenLists(1);
	glNewList(BatWingList, GL_COMPILE);
	glPushMatrix();
	glScalef(0.4f, 0.4f, 0.4f);
	glRotatef(180.f, 0.0, 1.0, 0.0);
	SetMaterial(0.0f, 0.0f, 0.0f, 100.f);
	LoadObjFile("batWingBlack.obj");
	SetMaterial(1.0f, 0.0f, 0.0f, 100.f);
	LoadObjFile("batWingRed.obj");
	glPopMatrix();
	glEndList();

	SignalList = glGenLists(1);
	glNewList(SignalList, GL_COMPILE);
	glPushMatrix();
		glScalef(.5f, .5f, .5f);
		glRotatef(90.f, 0.0f, 1.0f, 0.0f);
		SetMaterial(0.0f, 0.0f, 0.0f, 100.f);
		LoadObjFile("batSymbol.obj");
	glPopMatrix();
	glEndList();

	SigCircleList = glGenLists(1);
	glNewList(SigCircleList, GL_COMPILE);
	glPushMatrix();
	SetMaterial(1.0f, 1.0f, 0.0f, 100.f);
	glTranslatef(0.0f, 0.0f, -5.f);
	int radius = 50;
	int numSegments = 200;
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // Center of circle
		for (int i = 0; i <= numSegments; i++) {
			GLfloat angle = i * 2.0f * M_PI / numSegments;
			GLfloat dx = radius * cos(angle);
			GLfloat dy = radius * sin(angle);
			glVertex2f(x + dx, y + dy);
		}
		glEnd();
	glPopMatrix();
	glEndList();

	JumpList = glGenLists(1);
	glNewList(JumpList, GL_COMPILE);
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glTranslatef(-5, 0, 10);
	//SetMaterial(1.0f, 1.0f, 1.0f, 100.f);
	LoadObjFile("batJump.obj");
	glPopMatrix();
	glEndList();

	PrepJumpList = glGenLists(1);
	glNewList(PrepJumpList, GL_COMPILE);
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glTranslatef(-5, 0, 10);
	//SetMaterial(1.0f, 1.0f, 1.0f, 100.f);
	LoadObjFile("batPrepJump.obj");
	glPopMatrix();
	glEndList();

	WalkLeftList = glGenLists(1);
	glNewList(WalkLeftList, GL_COMPILE);
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	SetMaterial(0.2f, 0.2f, 0.2f, 80.f);
	LoadObjFile("walkLeftBlack.obj");
	SetMaterial(1.0f, 1.0f, 1.0f, 80.f);
	LoadObjFile("walkLeftWhite.obj");
	SetMaterial(1.0f, 1.0f, 0.0f, 80.f);
	LoadObjFile("walkLeftYellow.obj");
	glPopMatrix();
	glEndList();

	WalkRightList = glGenLists(1);
	glNewList(WalkRightList, GL_COMPILE);
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	SetMaterial(0.2f, 0.2f, 0.2f, 80.f);
	LoadObjFile("walkRightBlack.obj");
	SetMaterial(1.0f, 1.0f, 1.0f, 80.f);
	LoadObjFile("walkRightWhite.obj");
	SetMaterial(1.0f, 1.0f, 0.0f, 80.f);
	LoadObjFile("walkRightYellow.obj");
	glPopMatrix();
	glEndList();

	BatList = glGenLists(1);
	glNewList(BatList, GL_COMPILE);
	glPushMatrix();
		SetMaterial(0.0f, 0.0f, 0.0f, 200.f);
		LoadObjFile("batSwarm.obj");
	glPopMatrix();
	glEndList();


}

// initialize the glui window:

void
InitMenus()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(float));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int lightcolormenu = glutCreateMenu(DoLightColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int lighttypemenu = glutCreateMenu(DoLightTypeMenu);
	glutAddMenuEntry("Point Light", 0);
	glutAddMenuEntry("Spot Light", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Axis Colors", colormenu);
	glutAddSubMenu("Light Colors", lightcolormenu);
	glutAddSubMenu("Light Type", lighttypemenu);


#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddSubMenu("Projection", projmenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'o':
	case 'O':
		NowProjection = ORTHO;
		break;

	case 'j':
	case 'J':
		NowProjection = PERSP;
		break;
	case 'p':
	case 'P':
		NowLight = 0;
		break;
	case 's':
	case 'S':
		NowLight = 1;
		break;
	case 'w':
	case 'W':
		LightColor = 0;
		break;
	case 'r':
	case 'R':
		LightColor = 1;
		break;
	case 'g':
	case 'G':
		LightColor = 2;
		break;
	case 'b':
	case 'B':
		LightColor = 3;
		break;
	case 'c':
	case 'C':
		LightColor = 4;
		break;
	case 'm':
	case 'M':
		LightColor = 5;
		break;
	case 'f':
	case 'F':
		Frozen = !Frozen;
		if (Frozen)
			glutIdleFunc(NULL);
		else

			glutIdleFunc(Animate);
		break;
	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}

	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	Frozen = false;
	LightColor = WHITE2;
	NowLight = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = { 0.f, 1.f, 0.f, 1.f };

static float xy[] = { -.5f, .5f, .5f, -.5f };

static int xorder[] = { 1, 2, -3, 4 };

static float yx[] = { 0.f, 0.f, -.5f, .5f };

static float yy[] = { 0.f, .6f, 1.f, 1.f };

static int yorder[] = { 1, 2, 3, -2, 4 };

static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit(float v[3])
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
