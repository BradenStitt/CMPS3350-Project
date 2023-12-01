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
Platform blackholeTest;
Platform trophy;
Texture t, s, soccer, p, he;
StartMenu startMenu;
Enemy enemy;
// floating point random numbers
typedef float Flt;

// const float GRAVITY = 0.75;

#define PI 3.141592653589793

// const int MAXPLATFORMS = 10;
int numPlatforms = 0;
int snehalTest = 0;
int snehalsInstructions = 0;
bool stop = false;
bool pKey = true;
bool kKey = true;

vector<Platform> testEnemies;

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
int inStartMenu = 1;
Background img[1] = {"underwater.png"};
Background space[1] = {"spacebck.png"};
Background sprite[1] = {"finalspritepls.png"};
Background trophyImage[1] = {"trophypls.png"};
Background hearts[1] = {"heart.png"};
void init_opengl(void);
void physics(void);
void render(void);
extern void render_hearts(void);

//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
	logOpen();
	init_opengl();
	// printf("Press Up arrow to Jump.\n");
	// printf("Press Left or Right arrows to move player.\n");
	// printf("Press Space to Shoot.\n");
	// printf("Press R to restart game.\n");

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

		render();
		if (!inStartMenu) {
			physics();
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
			stop = false;
			kKey = true;
			snehalsInstructions = 0;
			break;
		case XK_s:
			g.showNerdStats = !g.showNerdStats;
			break;
		case XK_p:
			if (pKey) {
				inStartMenu = 0;
				player.init();
			}
			pKey = false;
			break;
		case XK_m:
			inStartMenu = 1;
			kKey = true;
			pKey = true;
			snehalTest = 0;
			gameManager.resetGame();
			player.lives = 3;
			snehalsInstructions = 0;
			break;
		case XK_k:
			if (kKey) {
				snehalTest = 1;
				player.pos[0] = g.xres / 2;
				player.pos[1] = 40.0f;
				player.lives = 5;
				player.score = 0;
				testEnemies.clear();
				snehalTestBackground();
			}
			kKey = false;
			break;
		case XK_i:
			snehalsInstructions = !snehalsInstructions;
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

	t.tex.backImage = &img[0];
	soccer.tex.backImage = &space[0];
	s.tex.backImage = &sprite[0];
	p.tex.backImage = &trophyImage[0];
	he.tex.backImage = &hearts[0];
	// create opengl texture elements
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
}

void physics()
{
	count_physics_function();

	if (!snehalTest) {
		t.tex.yc[0] -= 0.0033;
		t.tex.yc[1] -= 0.0033;
	}
	
	// Player physics
	player.physics();
	bullet.physics();

	if (snehalTest) {
		// static enemy collision
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
	} else {
		
		// trophy collision
		if ((player.pos[0] + player.width > trophy.pos[0] - trophy.width && player.pos[0] <= trophy.pos[0]) ||
			(player.pos[0] - player.width < trophy.pos[0] + trophy.width && player.pos[0] >= trophy.pos[0])) {

			if ((player.pos[1] - player.height <= trophy.pos[1] + trophy.height && player.pos[1] - player.height >= trophy.pos[1]) ||
				(player.pos[1] + player.height >= trophy.pos[1] - trophy.height && player.pos[1] + player.height <= trophy.pos[1])) {
				trophy.defaultTrophyColor = false;
				player.trophyDetected++; 
				player.score = player.score + (1000 * player.trophyDetected);
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
		startMenu.showStartScreen();
	}
	else {

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
				ggprint8b(&r, 16, 0x0055ff55, "   Press 'I' for Instructions");

				// Draw the platform
				Platform platform; // Declare an instance of the Platform class

				platform.pos[0] = g.xres / 2;
				platform.pos[1] = 40.0f;
				platform.pType = -1;

				if (!stop) {
					for (int i = 0; i < 3; i++) {
						Platform testEnemy;
						testEnemy.pos[0] = g.xres / 2 - 100 + (i * 100);
						testEnemy.pos[1] = 300.0f;
						testEnemy.pType = 3;
						testEnemies.push_back(testEnemy);
					}

					stop = true;
				}
				
				for (unsigned int j = 0; j < testEnemies.size(); j++) {
					testEnemy = testEnemies[j];
					testEnemy.draw_platform_fixed(testEnemy.pos[0], testEnemy.pos[1]);
				}

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
				glColor3f(1.0, 1.0, 1.0);
				glBindTexture(GL_TEXTURE_2D, t.tex.backTexture);
				glBegin(GL_QUADS);
				glTexCoord2f(t.tex.xc[0], t.tex.yc[1]);
				glVertex2i(0, 0);
				glTexCoord2f(t.tex.xc[0], t.tex.yc[0]);
				glVertex2i(0, g.yres);
				glTexCoord2f(t.tex.xc[1], t.tex.yc[0]);
				glVertex2i(g.xres, g.yres);
				glTexCoord2f(t.tex.xc[1], t.tex.yc[1]);
				glVertex2i(g.xres, 0);
				r.center = 0;
				r.bot = g.yres - 40;
				r.left = 10;
				ggprint8b(&r, 16, 0x00000000, "");
				ggprint8b(&r, 16, 0x0055ff55, " Press 'M' for MENU");
				ggprint8b(&r, 16, 0x0055ff55, " Press 'K' for SNEHAL'S FEATURES");
				glEnd();
				scoreboard();
				render_hearts();
				
				// Draw the platform
				// Platform platform; // Declare an instance of the Platform class

				// platform.pos[0] = 100.0f;
				// platform.pos[1] = 20.0f;
				// platform.draw_platform_fixed(platform.pos[0], platform.pos[1]);

				// Draw the platform 2
				Platform platform2; // Declare an instance of the Platform class

				// Set the position explicitly
				platform2.pos[0] = 200.0f;
				platform2.pos[1] = 20.0f;

				// Draw the platform at the specified location
				platform2.draw_platform_fixed(platform2.pos[0], platform2.pos[1]);

				// Draw the trophy
				trophy.pos[0] = g.xres / 2; // Center the trophy
				trophy.pos[1] = g.yres - 110;
				// testing positions 
				// trophy.pos[0] = 200.0f;
				// trophy.pos[1] = 300.0f;
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

				// if (!player.enemyDetected) {
				// 	if (player.pos[0] > (platform.pos[0] - platform.width) && player.pos[0] < (platform.pos[0] + platform.width)) {
				// 		if (player.pos[1] > (platform.pos[1] - platform.height) && player.pos[1] < (platform.pos[1] + platform.height)) {
				// 			player.pos[1] = platform.pos[1] + platform.height;
				// 			player.vel[1] = 0.0;
				// 			player.vel[0] = 0.0;
				// 			player.jumpCount = 0;

				// 			if (player.angle > 0.0 || player.angle < 0.0) {
				// 				g.failed_landing = 1;
				// 			}
				// 			else {
				// 				// g.landed = 1;
				// 			}
				// 		}
				// 	}
				// }

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
		if (snehalTest) {
			if (snehalsInstructions) {
				// Draw a box around the nerd stats
				
						 // Set the vertex color to gray
				glPushMatrix();
				glBegin(GL_QUADS);
				glColor3ub(90, 90, 90);
				glVertex2f(40, g.yres - 130.0);  
				glVertex2f(40, g.yres - 215.0);  
				glVertex2f(g.xres - 50.0, g.yres - 215.0);  
				glVertex2f(g.xres - 50.0, g.yres - 130.0);
				glEnd();

				r.center = 0;
				r.bot = g.yres - 20;
				r.left = 10;

				r.bot -= 127;
				ggprint8b(&r, 16, 0x0055ff55,                                 
				"                                 GAME CONTROLS");
				ggprint8b(&r, 16, 0x00ffff00, 
				"                            Press Up arrow to Jump");
				ggprint8b(&r, 16, 0x00ffff00, 
				"                Press Left or Right arrows to move player");
				ggprint8b(&r, 16, 0x00ffff00, 
				"                             Press 'Space' to Shoot");
				ggprint8b(&r, 16, 0x00ffff00, 
				"                              Press 'R' to return");
				glPopMatrix();
			}
		}
	}
}
