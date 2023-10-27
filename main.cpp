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

// floating point random numbers
typedef float Flt;

// gravity pulling the player straight down
// const float GRAVITY = 0.75;

#define PI 3.141592653589793

// const int MAXPLATFORMS = 10;
int numPlatforms = 0;

class Platform2
{
	// landing zone
public:
	float pos[2];
	float width;
	float height;
	Platform2()
	{
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
void init_opengl(void);
void physics(void);
void render(void);

//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
	logOpen();
	init_opengl();
	printf("Press Up arrow to Jump.\n");
	printf("Press Left or Right arrows to move player.\n");
	printf("Press Space to Shoot.\n");
	printf("Press R to reset player.\n");

	// Main loop
	int done = 0;
	while (!done)
	{
		// Process external events.
		while (x11.getXPending())
		{
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			x11.check_mouse(&e);
			done = x11.check_keys(&e);
		}
		physics();
		render();

		// Update physics for platforms
		gameManager.updatePhysics();

		// Render the platforms
		gameManager.render();

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
	GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, None};
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
	{
		printf("\n\tcannot connect to X server\n\n");
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL)
	{
		printf("\n\tno appropriate visual found\n\n");
		exit(EXIT_FAILURE);
	}
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask |
		ButtonPressMask | ButtonReleaseMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
						vi->depth, InputOutput, vi->visual,
						CWColormap | CWEventMask, &swa);
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
	if (xce.width != g.xres || xce.height != g.yres)
	{
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
		e->type != MotionNotify)
	{
		// This is not a mouse event that we care about.
		return;
	}
	if (e->type == ButtonRelease)
	{
		return;
	}
	if (e->type == ButtonPress)
	{
		if (e->xbutton.button == 1)
		{
			// Left button was pressed.
			time_since_mouse_moved(false);
			// int y = g.yres - e->xbutton.y;
			return;
		}
		if (e->xbutton.button == 3)
		{
			// Right button was pressed.
			time_since_mouse_moved(false);
			return;
		}
	}
	if (e->type == MotionNotify)
	{
		// The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y)
		{
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
	if (e->type == KeyPress)
	{
		switch (key)
		{
		case XK_r:
			// Key R was pressed
			g.landed = 0;
			player.init();
			break;
		case XK_s:
			g.showNerdStats = !g.showNerdStats;
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
}

void physics()
{
	count_physics_function();

	// Player physics
	player.physics();
	bullet.physics();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw Grid
	glPushMatrix();
	glBegin(GL_QUADS);
	// Set the background color of the grid to black
	glColor3ub(0, 0, 0); // Set the vertex color to black
						 // Set the background color of the grid to off-white
	// glColor3ub(240, 240, 240);
	// background_display();

	glVertex2i(0, 0);
	glVertex2i(g.xres, 0);
	glVertex2i(g.xres, g.yres);
	glVertex2i(0, g.yres);
	// Set the color of the grid lines to gray
	glColor3ub(90, 90, 90); // Set the vertex color to gray
	// glColor3ub(255, 150, 50); // Set the vertex color to orange
	for (int i = 0; i <= g.xres; i += 20)
	{
		glBegin(GL_LINES);
		glVertex2i(i, 0);
		glVertex2i(i, g.yres);
		glEnd();
	}
	for (int i = 0; i <= g.yres; i += 20)
	{
		glBegin(GL_LINES);
		glVertex2i(0, i);
		glVertex2i(g.xres, i);
		glEnd();
	}
	glEnd();
	glPopMatrix();

	// Draw the platform
	Platform platform; // Declare an instance of the Platform class

	// Set the position explicitly
	platform.pos[0] = 100.0f;
	platform.pos[1] = 20.0f;

	// Draw the platform at the specified location
	platform.draw_platform_fixed(platform.pos[0], platform.pos[1]);

	// Draw the platform 2
	Platform platform2; // Declare an instance of the Platform class

	// Set the position explicitly
	platform2.pos[0] = 200.0f;
	platform2.pos[1] = 70.0f;

	// Draw the platform at the specified location
	platform2.draw_platform_fixed(platform2.pos[0], platform2.pos[1]);

	// Draw Player
	player.draw_player();
	
	if (player.pos[0] > (pf.pos[0] - pf.width) && player.pos[0] < (pf.pos[0] + pf.width))
	{
		if (player.pos[1] > (pf.pos[1] - pf.height) && player.pos[1] < (pf.pos[1] + pf.height))
		{
			player.pos[1] = (pf.pos[1]) + pf.height;
			player.vel[1] = 0.0;
			player.vel[0] = 0.0;
			player.jumpCount = 0;

			if (player.angle > 0.0 || player.angle < 0.0)
			{
				g.failed_landing = 1;
			}
			else
			{
				// g.landed = 1;
			}
		}
	}

	if (player.pos[0] > (pf.pos[0] + 100 - pf.width) && player.pos[0] < (pf.pos[0] + 100 + pf.width))
	{
		if (player.pos[1] > (pf.pos[1] + 50 - pf.height) && player.pos[1] < (pf.pos[1] + 50 + pf.height))
		{
			player.pos[1] = pf.pos[1] + 50 + pf.height;
			player.vel[1] = 0.0;
			player.vel[0] = 0.0;
			player.jumpCount = 0;

			if (player.angle > 0.0 || player.angle < 0.0)
			{
				g.failed_landing = 1;
			}
			else
			{
				// g.landed = 1;
			}
		}
	}

	// check for collision with dynamic platforms
	dynamic_collision_detection();

	// Draw the bullets
	bullet.draw_bullet();

	if (g.failed_landing)
	{
		// show crash graphics here...
	}

	if (g.showNerdStats)
	{
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
