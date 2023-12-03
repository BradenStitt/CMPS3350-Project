#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "global.h"
#include "bstitt.h"
#include "skumar.h"
#include "jrivera2.h"
#include "bruiz.h"
#include <vector>
#include <chrono>
#include <thread>
#include "fonts.h"

using namespace std;

// Define Classes
Global g;
Rect r;
GameManager gameManager(10); // Adjust the number of platforms as needed
Player player;
Bullet bullet;
Platform testEnemy;
Platform testPlatform;
Platform blackholeTest;
Platform trophy;
Texture graph, t, soc, hop, ice;
Texture soccer, he, p, s, victory;
StartMenu startMenu;
Enemy enemy;

vector<Platform> testEnemies;
vector<Platform> testPlatforms;

// floating point random numbers
typedef float Flt;

// const float GRAVITY = 0.75;

#define PI 3.141592653589793

// const int MAXPLATFORMS = 10;
int numPlatforms = 0;
int inStartMenu = 1;
int snehalTest = 0;
int snehalControls = 0;
int snehalFeatures = 0;
bool done = false;
bool pKey = false;
bool kKey = false;
bool bKey = false;


class Platform2
{
	// landing zone
public:
	float pos[2];
	float width;
	float height;
	Platform2() {
		pos[0] = 100.0f;
		pos[1] = 20.0f;
		width = 50.0f;
		height = 8.0f;
	}
} pf;

class X11_wrapper
{
private:
	Display *dpy;
	Window win;
	GLXContext glc;

public:
	~X11_wrapper();
	X11_wrapper();
	void set_title();
	bool getXPending();
	XEvent getXNextEvent();
	void swapBuffers();
	void reshape_window(int width, int height);
	void check_resize(XEvent *e);
	void check_mouse(XEvent *e);
	int check_keys(XEvent *e);
} x11;

// Function prototypes
Background graphb[1] = {"better.jpeg"}; // Level 1 background
Background img[1] = {"underwater.png"}; // Level 2 background
Background socb[1] = {"soccerbck.png"}; // Level 3 background
Background hopb[1] = {"hop.png"}; // Level 4 background
Background iceb[1] = {"ice.png"}; // Level 5 background
Background sprite[1] = {"finalspritepls.png"};
Background trophyImage[1] = {"trophypls.png"};
Background hearts[1] = {"heart.png"};
Background space[1] = {"spacebck.png"}; // Snehals Test background
Background vict[1] = {"victoryscreen.png"};
void init_opengl(void);
void physics(void);
void render(void);
extern void render_hearts(void);
int levelNum = 0;

//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
	logOpen();
	init_opengl();

	// Main loop
	int done = 0;
	while (!done) {
		// Process external events.
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			x11.check_mouse(&e);
			done = x11.check_keys(&e);
		}

		if (inStartMenu)
			render();
		else if (!inStartMenu) {
			physics();
			render();
			if (!player.blackholeDetected && !snehalTest) {
				// Update physics for platforms
				gameManager.updatePhysics();
				// Render the platforms
				gameManager.render();
			}
		}

		x11.swapBuffers();
		usleep(400);
	}

	logClose();
	return 0;
}

X11_wrapper::~X11_wrapper()
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

X11_wrapper::X11_wrapper()
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w = g.xres, h = g.yres;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		cout << "\n\tcannot connect to X server\n" << endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		cout << "\n\tno appropriate visual found\n" << endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
		InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void X11_wrapper::set_title()
{
	// Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "Scribble-Hop");
}

bool X11_wrapper::getXPending()
{
	// See if there are pending events.
	return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
	// Get a pending event.
	XEvent e;
	XNextEvent(dpy, &e);
	return e;
}

void X11_wrapper::swapBuffers()
{
	glXSwapBuffers(dpy, win);
}

void X11_wrapper::reshape_window(int width, int height)
{
	// window has been resized.
	g.xres = width;
	g.yres = height;
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
	// The ConfigureNotify is sent by the
	// server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != g.xres || xce.height != g.yres) {
		// Window size did change.
		reshape_window(xce.width, xce.height);
	}
}
//-----------------------------------------------------------------------------

void X11_wrapper::check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;

	// Weed out non-mouse events
	if (e->type != ButtonRelease &&
		e->type != ButtonPress &&
		e->type != MotionNotify) {
		// This is not a mouse event that we care about.
		return;
	}
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button == 1) {
			// Left button was pressed.
			time_since_mouse_moved(false);
			// int y = g.yres - e->xbutton.y;
			return;
		}
		if (e->xbutton.button == 3) {
			// Right button was pressed.
			time_since_mouse_moved(false);
			return;
		}
	}
	if (e->type == MotionNotify) {
		// The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			time_since_mouse_moved(false);
			// Code placed here will execute whenever the mouse moves.
		}
	}
}

int X11_wrapper::check_keys(XEvent *e)
{
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyPress)
		g.keys[key] = 1;
	if (e->type == KeyRelease)
		g.keys[key] = 0;
	if (e->type == KeyPress) {
		time_since_key_press(false);
		switch (key)
		{
			case XK_r:
				// Key R was pressed
				snehalTest = 0;
				g.landed = 0;
				gameManager.resetGame();
				player.lives = 3;
				player.score = 0;
				player.trophyDetected = 0;
				player.init();
				done = false;
				bKey = false;
				kKey = false;
				snehalControls = 0;
				snehalFeatures = 0;
				break;
			case XK_s:
				g.showNerdStats = !g.showNerdStats;
				break;
			case XK_p:
				if (!pKey) {
					inStartMenu = 0;
					player.init();
				}
				pKey = true;
				break;
			case XK_m:
				inStartMenu = 1;
				kKey = false;
				pKey = false;
				bKey = false;
				done = false;
				snehalTest = 0;
				gameManager.resetGame();
				player.lives = 3;
				snehalControls = 0;
				snehalFeatures = 0;
				break;
			case XK_k:
				if (!kKey) {
					snehalTest = 1;
					player.pos[0] = g.xres / 2;
					player.pos[1] = 40.0f;
					player.lives = 5;
					player.score = 0;
					testEnemies.clear();
					testPlatforms.clear();
					snehalTestBackground();
				}
				kKey = true;
				break;
			case XK_b:
				if (snehalTest) {
					testEnemies.clear();
					done = false;
					bKey = true;
				}
				break;
			case XK_c:
				if (snehalTest) {
					snehalControls = !snehalControls;
				}
				break;
			case XK_f:
				if (snehalTest) {
					snehalFeatures = !snehalFeatures;
				}
				break;
			case XK_Escape:
				// Escape key was pressed
				return 1;
		}
	}
	return 0;
}

void init_opengl(void)
{
	// OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	// Initialize matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Set 2D mode (no perspective)
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	// Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);

	// Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();

	t.tex.backImage = &img[0]; // underwater
	soccer.tex.backImage = &space[0]; // space
	s.tex.backImage = &sprite[0]; // Sprite
	p.tex.backImage = &trophyImage[0]; // trophy
	he.tex.backImage = &hearts[0]; // Heart
	soc.tex.backImage = &socb[0]; // soccer
	graph.tex.backImage = &graphb[0]; // graph
	hop.tex.backImage = &hopb[0]; // field
	ice.tex.backImage = &iceb[0]; // ice
	victory.tex.backImage = &vict[0];
	// create opengl texture elements
	//Underwater
	glGenTextures(1, &t.tex.backTexture);
	int w = t.tex.backImage->width;
	int h = t.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, t.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
				GL_RGB, GL_UNSIGNED_BYTE, t.tex.backImage->data);
	t.tex.xc[0] = 0.0;
	t.tex.xc[1] = 1.0;
	t.tex.yc[0] = 0.0;
	t.tex.yc[1] = 1.0;

	//Sprite
	glGenTextures(1, &s.tex.backTexture);
	int ws = s.tex.backImage->width;
	int hs = s.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, s.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, ws, hs, 0,
				GL_RGB, GL_UNSIGNED_BYTE, s.tex.backImage->data);
	s.tex.xc[0] = 0.0;
	s.tex.xc[1] = 1.0;
	s.tex.yc[0] = 0.0;
	s.tex.yc[1] = 1.0;

	// Space
	glGenTextures(1, &soccer.tex.backTexture);
	int wsoccer = soccer.tex.backImage->width;
	int hsoccer = soccer.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, soccer.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wsoccer, hsoccer, 0,
				GL_RGB, GL_UNSIGNED_BYTE, soccer.tex.backImage->data);
	soccer.tex.xc[0] = 0.0;
	soccer.tex.xc[1] = 1.0;
	soccer.tex.yc[0] = 0.0;
	soccer.tex.yc[1] = 1.0;

	// Trophy
	glGenTextures(1, &p.tex.backTexture);
	int wp = p.tex.backImage->width;
	int hp = p.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, p.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wp, hp, 0,
				GL_RGB, GL_UNSIGNED_BYTE, p.tex.backImage->data);
	p.tex.xc[0] = 0.0;
	p.tex.xc[1] = 1.0;
	p.tex.yc[0] = 0.0;
	p.tex.yc[1] = 1.0;

	// Hearts
	glGenTextures(1, &he.tex.backTexture);
	int wh = he.tex.backImage->width;
	int hh = he.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, he.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wh, hh, 0,
				GL_RGB, GL_UNSIGNED_BYTE, he.tex.backImage->data);
	he.tex.xc[0] = 0.0;
	he.tex.xc[1] = 1.0;
	he.tex.yc[0] = 0.0;
	he.tex.yc[1] = 1.0;

	// Soccer
	glGenTextures(1, &soc.tex.backTexture);
	int wsoc = soc.tex.backImage->width;
	int hsoc = soc.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, soc.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wsoc, hsoc, 0,
				GL_RGB, GL_UNSIGNED_BYTE, soc.tex.backImage->data);
	soc.tex.xc[0] = 0.0;
	soc.tex.xc[1] = 1.0;
	soc.tex.yc[0] = 0.0;
	soc.tex.yc[1] = 1.0;

	// Graph
	glGenTextures(1, &graph.tex.backTexture);
	int wgraph = graph.tex.backImage->width;
	int hgraph = graph.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, graph.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wgraph, hgraph, 0,
				GL_RGB, GL_UNSIGNED_BYTE, graph.tex.backImage->data);
	graph.tex.xc[0] = 0.0;
	graph.tex.xc[1] = 1.0;
	graph.tex.yc[0] = 0.0;
	graph.tex.yc[1] = 1.0;

	// Field
	glGenTextures(1, &hop.tex.backTexture);
	int whop = hop.tex.backImage->width;
	int hhop = hop.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, hop.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, whop, hhop, 0,
				GL_RGB, GL_UNSIGNED_BYTE, hop.tex.backImage->data);
	hop.tex.xc[0] = 0.0;
	hop.tex.xc[1] = 1.0;
	hop.tex.yc[0] = 0.0;
	hop.tex.yc[1] = 1.0;

	// Ice
	glGenTextures(1, &ice.tex.backTexture);
	int wice = ice.tex.backImage->width;
	int hice = ice.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, ice.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wice, hice, 0,
				GL_RGB, GL_UNSIGNED_BYTE, ice.tex.backImage->data);
	ice.tex.xc[0] = 0.0;
	ice.tex.xc[1] = 1.0;
	ice.tex.yc[0] = 0.0;
	ice.tex.yc[1] = 1.0;

	// victory
	glGenTextures(1, &victory.tex.backTexture);
	int wvictory = victory.tex.backImage->width;
	int hvictory = victory.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, victory.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wvictory, hvictory, 0,
				GL_RGB, GL_UNSIGNED_BYTE, victory.tex.backImage->data);
	victory.tex.xc[0] = 0.0;
	victory.tex.xc[1] = 1.0;
	victory.tex.yc[0] = 0.0;
	victory.tex.yc[1] = 1.0;

	
}

void physics()
{
	count_physics_function();

	if (!snehalTest) {
		if (player.trophyDetected == 0) {
			graph.tex.yc[0] -= 0.0033;
			graph.tex.yc[1] -= 0.0033;
		} else if (player.trophyDetected == 1) {
			soc.tex.yc[0] -= 0.0033;
			soc.tex.yc[1] -= 0.0033;
		} else if (player.trophyDetected == 2) {
			t.tex.yc[0] -= 0.0033;
			t.tex.yc[1] -= 0.0033;
		} else if (player.trophyDetected == 3) {
			hop.tex.yc[0] -= 0.0033;
			hop.tex.yc[1] -= 0.0033;
		} else if (player.trophyDetected == 4){
			ice.tex.yc[0] -= 0.0033;
			ice.tex.yc[1] -= 0.0033;
		}
	}
	
	// Player physics
	player.physics();
	bullet.physics();

	if (snehalTest) {
		// testPlatform physics
		for (unsigned int i = 0; i < testPlatforms.size(); i++) {
			Platform testPlatform = testPlatforms[i];
			// cout << "testPlatform.pos[0] 1: " << testPlatform.pos[0] << endl;
			if (i == 0) {
				testPlatform.pos[0] -= testPlatform.velocity;
				// cout << "testPlatform.pos[0] 2: " << testPlatform.pos[0] << endl;
				if (testPlatform.pos[0] - testPlatform.width < 0.0f) {
					testPlatform.pos[0] = testPlatform.width;
					testPlatform.velocity = -testPlatform.velocity;
				} else if (testPlatform.pos[0] + testPlatform.width > g.xres) {
					testPlatform.pos[0] = g.xres - testPlatform.width;
					testPlatform.velocity = -testPlatform.velocity;
				}
				testPlatforms[i] = testPlatform;

			} else if (i == 1) {
				testPlatform.pos[0] += testPlatform.velocity;

				if (testPlatform.pos[0] + testPlatform.width > g.xres) {
					testPlatform.pos[0] = g.xres - testPlatform.width;
					testPlatform.velocity = -testPlatform.velocity;
				} else if (testPlatform.pos[0] - testPlatform.width < 0.0f) {
					testPlatform.pos[0] = testPlatform.width;
					testPlatform.velocity = -testPlatform.velocity;
				}
				testPlatforms[i] = testPlatform;
			}
		}

		// testEnemy collision
		for (unsigned int i = 0; i < testEnemies.size(); i++) {
			Platform testEnemy = testEnemies[i];

			if ((player.pos[0] + player.width > testEnemy.pos[0] - testEnemy.width && player.pos[0] <= testEnemy.pos[0]) ||
				(player.pos[0] - player.width < testEnemy.pos[0] + testEnemy.width && player.pos[0] >= testEnemy.pos[0])) {

				if ((player.pos[1] - player.height <= testEnemy.pos[1] + testEnemy.height && player.pos[1] - player.height >= testEnemy.pos[1]) ||
					(player.pos[1] + player.height >= testEnemy.pos[1] - testEnemy.height && player.pos[1] + player.height <= testEnemy.pos[1])) {
					player.enemyDetected = 1;
					player.vel[1] = -8.0;
				}
			}
		}

		if (!player.enemyDetected) {
			// testPlatform collision
			for (unsigned int j = 0; j < testPlatforms.size(); j++) 
			{
				Platform testPlatform = testPlatforms[j];

				if (player.pos[0] > (testPlatform.pos[0] - testPlatform.width) && player.pos[0] < (testPlatform.pos[0] + testPlatform.width)) 
				{
					if (player.pos[1] > (testPlatform.pos[1] - testPlatform.height) && player.pos[1] < (testPlatform.pos[1] + testPlatform.height)) 
					{
						// Player is colliding with platform
						player.pos[1] = testPlatform.pos[1] + testPlatform.height;
						player.vel[1] = 0.0;
						player.vel[0] = 0.0;
						player.jumpCount = 0;
					}
				}
			}
		}

	} else {
		
		// trophy collision
		if ((player.pos[0] + player.width > trophy.pos[0] - trophy.width && player.pos[0] <= trophy.pos[0]) ||
			(player.pos[0] - player.width < trophy.pos[0] + trophy.width && player.pos[0] >= trophy.pos[0])) {

			if ((player.pos[1] - player.height <= trophy.pos[1] + trophy.height && player.pos[1] - player.height >= trophy.pos[1]) ||
				(player.pos[1] + player.height >= trophy.pos[1] - trophy.height && player.pos[1] + player.height <= trophy.pos[1])) {
				trophy.defaultTrophyColor = false;
				player.trophyDetected++; 
				player.score = player.score + (1000 * player.trophyDetected);
				levelNum = 0;
				gameManager.resetGame();
				player.init();
			}
		}
	}
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (inStartMenu) {
		// Display the start menu.
		if (player.trophyDetected == 0) {
			startMenu.showStartScreen(graph);
		} else if (player.trophyDetected == 1) {
			startMenu.showStartScreen(soc);
		} else if (player.trophyDetected == 2) {
			startMenu.showStartScreen(t);
		} else if (player.trophyDetected == 3) {
			startMenu.showStartScreen(hop);
		} else if (player.trophyDetected == 4){
			startMenu.showStartScreen(ice);
		} else {
			victoryScreen();
		}
	} else {
		if (player.blackholeDetected) {
			blackhole_screen();
			youDied();
		}
		else {
			// Draw Grid
			if (snehalTest) {
				snehalTestBackground();
				snehalsText();
				scoreboard();
				render_hearts();

				r.center = 0;
				r.bot = g.yres - 45;
				r.left = 5;
				ggprint8b(&r, 16, 0x00000000, "");
				ggprint8b(&r, 16, 0x0055ff55, "   Press 'R' to Return");
				ggprint8b(&r, 16, 0x0055ff55, "   Press 'C' for Controls");
				ggprint8b(&r, 16, 0x0055ff55, "   Press 'F' for Features List");

				// Draw the platform
				Platform platform; // Declare an instance of the Platform class

				platform.pos[0] = g.xres / 2;
				platform.pos[1] = 20.0f;
				platform.pType = -1;

				if (!done) {
					pushTestPlatforms();
					done = true;
				}

				renderTestPlatforms();

				platform.draw_platform_fixed(platform.pos[0], platform.pos[1]);

				// Draw Player
				player.draw_player();

				if (!player.enemyDetected) {
					if (player.pos[0] > (platform.pos[0] - platform.width) && player.pos[0] < (platform.pos[0] + platform.width)) {
						if (player.pos[1] > (platform.pos[1] - platform.height) && player.pos[1] < (platform.pos[1] + platform.height)) {
							player.pos[1] = platform.pos[1] + platform.height;
							player.vel[1] = 0.0;
							player.vel[0] = 0.0;
							player.jumpCount = 0;

							if (player.angle > 0.0 || player.angle < 0.0) {
								g.failed_landing = 1;
							}
							else {
								// g.landed = 1;
							}
						}
					}
				}

				// Draw the bullets
				bullet.draw_bullet();
				updateAndPrintScore();

			} else {
				//levelOne(soccer);
				levelNum++;

				makeLevels(levelNum);
				scoreboard();
				render_hearts();

				if (player.trophyDetected == 5) {
					inStartMenu = 1;
				}

				// Draw the platform 2
				Platform platform2; // Declare an instance of the Platform class

				// Set the position explicitly
				platform2.pos[0] = 200.0f;
				platform2.pos[1] = 20.0f;

				// Draw the platform at the specified location
				platform2.draw_platform_fixed(platform2.pos[0], platform2.pos[1]);

				// Draw the trophy
				// trophy.pos[0] = g.xres / 2; // Center the trophy
				// trophy.pos[1] = g.yres - 130;
				// testing positions 
				trophy.pos[0] = 200.0f;
				trophy.pos[1] = 300.0f;
				trophy.pType = 6;
				trophy.draw_platform_fixed(trophy.pos[0], trophy.pos[1]);

				if (!player.enemyDetected) {
					if (player.pos[0] > (platform2.pos[0] - platform2.width) && player.pos[0] < (platform2.pos[0] + platform2.width)) {
						if (player.pos[1] > (platform2.pos[1] - platform2.height) && player.pos[1] < (platform2.pos[1] + platform2.height)) {
							player.pos[1] = platform2.pos[1] + platform2.height;
							player.vel[1] = 0.0;
							player.vel[0] = 0.0;
							player.jumpCount = 0;

							if (player.angle > 0.0 || player.angle < 0.0) {
								g.failed_landing = 1;
							}
							else {
								// g.landed = 1;
							}
						}
					}
				}

				// Draw Player
				player.draw_player();

				// Draw the bullets
				bullet.draw_bullet();
				dynamic_collision_detection();
				updateAndPrintScore();
			}

			if (g.failed_landing) {
				// show crash graphics here...
			}

			if (g.showNerdStats) {
				// Draw a box around the nerd stats
				glColor3ub(90, 90, 90); // Set the vertex color to gray
				glPushMatrix();
				glTranslatef(20.0f, 20.0f, 0.0f);
				int w = 200;
				int h = 100;
				glBegin(GL_QUADS);
				glVertex2f(0, 0);
				glVertex2f(0, h);
				glVertex2f(w, h);
				glVertex2f(w, 0);
				glEnd();
				glPopMatrix();
				r.bot = 130;
				r.left = 20;
				r.center = 0;
				ggprint8b(&r, 24, 0x0055ff55, " ");
				ggprint8b(&r, 12, 0x0055ff55, " Nerd Stats...");
				ggprint8b(&r, 12, 0x00ffff00, " Total Running Time: %i", total_running_time(true));
				ggprint8b(&r, 12, 0x00ffff00, " Time Since Key Press: %i", time_since_key_press(true));
				ggprint8b(&r, 12, 0x00ffff00, " Time Since Mouse Movement: %i", time_since_mouse_moved(true));
				ggprint8b(&r, 12, 0x00ffff00, " Number of Physics() Calls: %i", count_physics_function());
				ggprint8b(&r, 12, 0x00ffff00, " Number of Render() Calls: %i", count_render_function());
			}

			glPopMatrix();
		}
		if (snehalTest && !player.blackholeDetected) {
			if (snehalControls) {
				// Set the vertex color to gray
				glPushMatrix();
				glBegin(GL_QUADS);
				glColor3ub(90, 90, 90);
				glVertex2f(40, g.yres - 170.0);  
				glVertex2f(40, g.yres - 255.0);  
				glVertex2f(g.xres - 50.0, g.yres - 255.0);  
				glVertex2f(g.xres - 50.0, g.yres - 170.0);
				glEnd();

				r.center = 0;
				r.bot = g.yres - 20;
				r.left = 10;

				r.bot -= 167;
				ggprint8b(&r, 16, 0x0001dee6,                                 
				"                                 GAME CONTROLS");
				ggprint8b(&r, 16, 0x00f1b620, 
				"                            Press Up arrow to Jump");
				ggprint8b(&r, 16, 0x00f1b620, 
				"                Press Left or Right arrows to move player");
				ggprint8b(&r, 16, 0x00f1b620, 
				"                      Press 'B' to Bring Enemies Back");
				ggprint8b(&r, 16, 0x00f1b620, 
				"                             Press 'Space' to Shoot");
				glPopMatrix();
			}

			if (snehalFeatures) {
				glPushMatrix();
					glBegin(GL_QUADS);
					glColor3ub(90, 90, 90); 
					glVertex2f(40, g.yres - 255.0);  
					glVertex2f(40, g.yres - 320.0);  
					glVertex2f(g.xres - 50.0, g.yres - 320.0);  
					glVertex2f(g.xres - 50.0, g.yres - 255.0);
					glEnd();

					r.center = 0;
					r.bot = g.yres - 20;
					r.left = 10;

					r.bot -= 258;
					
					ggprint8b(&r, 16, 0x00fbf608,                                 
					"                                      FEATURES");
					ggprint8b(&r, 16, 0x004db3e4, 
					"                          Player Movement & Bullets");
					ggprint8b(&r, 16, 0x004db3e4, 
					"                 Collisions:"
					"					 - Bullet"
					"					 - Enemy"
					"					 - Platform");
				glPopMatrix();
			}
		}

	}
}
