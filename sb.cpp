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
// #include "skumar.h"
#include <vector>
#include <chrono>
#include <thread>

using namespace std;

// Define Classes
Global g;
GameManager gameManager(10); // Adjust the number of platforms as needed

//floating point random numbers
#define rnd() (float)rand() / (float)RAND_MAX
typedef float Flt;
typedef float Vec[3];

//gravity pulling the player straight down
const float GRAVITY = 0.075;
//const float GRAVITY = 0.00005;
#define PI 3.141592653589793 
const int MAX_BULLETS = 11;
const int MAXPLATFORMS = 10;
int numPlatforms = 0;

extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

class Platform2 {
	//landing zone
	public:
	float pos[2];
	float width;
	float height;
	Platform2() {
		pos[0] = 100.0f; 
		pos[1] = 20.0f; 
		width =  50.0f;
		height =  8.0f;
	}
} pf;

class Bullet {
	public:
		Vec pos;
		Vec vel;
		float color[3];
		struct timespec time;
	public:
		Bullet() { }
};

class Player {
	//the player rocket
	public:
	Bullet *barr;
	int nbullets;
	struct timespec bulletTimer;

	float pos[2];
	float vel[2];
	float verts[3][2];
	// float thrust;
	double angle;

	Player() {
		init();
		barr = new Bullet[MAX_BULLETS];
		nbullets = 0;
		clock_gettime(CLOCK_REALTIME, &bulletTimer);
	}
	void init() {

		pos[0] = 100.0f; 
		pos[1] = 40.0f; 

		vel[0] = vel[1] = 0.0f;
		//3 vertices of triangle-shaped rocket player
		verts[0][0] = -10.0f;
		verts[0][1] =   0.0f;
		verts[1][0] =   0.0f;
		verts[1][1] =  30.0f;
		verts[2][0] =  10.0f;
		verts[2][1] =   0.0f;
		angle = 0.0;
		// thrust = 0.0f;
		g.failed_landing = 0;
	}

	~Player() {
		delete [] barr;
	}
	
} player;

class X11_wrapper {
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

//Function prototypes
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

	// defined globally 
    // GameManager gameManager(10); // Adjust the number of platforms as needed

	//Main loop
	int done = 0;
	while (!done) {
		//Process external events.
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			x11.check_mouse(&e);
			done = x11.check_keys(&e);
		}
		physics();
		render();
		
		// moved to physics()
		// Update physics for platforms
        //gameManager.updatePhysics();

		// moved to render()
        // Render the platforms
        //gameManager.render();

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
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "Scribble-Hop");
}

bool X11_wrapper::getXPending()
{
	//See if there are pending events.
	return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
	//Get a pending event.
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
	//window has been resized.
	g.xres = width;
	g.yres = height;
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != g.xres || xce.height != g.yres) {
		//Window size did change.
		reshape_window(xce.width, xce.height);
	}
}
//-----------------------------------------------------------------------------

void X11_wrapper::check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;

	//Weed out non-mouse events
	if (e->type != ButtonRelease &&
		e->type != ButtonPress &&
		e->type != MotionNotify) {
		//This is not a mouse event that we care about.
		return;
	}
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed.
			//int y = g.yres - e->xbutton.y;
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed.
			return;
		}
	}
	if (e->type == MotionNotify) {
		//The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			//Code placed here will execute whenever the mouse moves.


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
		switch (key) {
			case XK_r:
				//Key R was pressed
				player.init();
				g.landed = 0;
				break;
			case XK_Escape:
				//Escape key was pressed
				return 1;
		}
	}
	return 0;
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0); 
}

void physics()
{
    // Player physics
    if (g.failed_landing)
        return;
    player.pos[0] += player.vel[0];
    player.pos[1] += player.vel[1];
    player.vel[1] -= GRAVITY;

    // Update bullet positions
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    int i = 0;
    while (i < player.nbullets) {
        Bullet* b = &player.barr[i];
        // How long has bullet been alive?
        double ts = timeDiff(&b->time, &bt);
        if (ts > 2.5) {
            // time to delete the bullet.
            memcpy(&player.barr[i], &player.barr[player.nbullets - 1],
                sizeof(Bullet));
            player.nbullets--;
            // do not increment i.
            continue;
        }

        // move the bullet
        b->pos[0] += b->vel[0];
        b->pos[1] += b->vel[1];

        // Check for collision with window edges
        if (b->pos[0] < 0.0) {
            b->pos[0] += (float)g.xres;
        }
        else if (b->pos[0] > (float)g.xres) {
            b->pos[0] -= (float)g.xres;
        }
        else if (b->pos[1] < 0.0) {
            b->pos[1] += (float)g.yres;
        }
        else if (b->pos[1] > (float)g.yres) {
            b->pos[1] -= (float)g.yres;
        }
        ++i;
    }

    // Check keys pressed now
    if (g.keys[XK_Left])
        player.vel[0] -= 0.1;
    if (g.keys[XK_Right])
        player.vel[0] += 0.1;
    if (g.keys[XK_Up])
        player.vel[1] += 0.1;
    if (g.keys[XK_space]) {
   		// Shoot a bullet...
		if (player.nbullets < MAX_BULLETS) {
			Bullet* b = &player.barr[player.nbullets];
			timeCopy(&b->time, &bt);

			// Adjust the y-position so it's just above the player with more distance
			b->pos[1] = player.pos[1] + 30.0f; // Adjust as needed
			// Set bullet velocity to move farther upwards
			b->vel[0] = 0.0f;
			b->vel[1] = 0.03f + rnd() * 0.02f; // Adjust for more spread
			b->color[0] = 1.0f;
			b->color[1] = 1.0f;
			b->color[2] = 1.0f;
			++player.nbullets;
		}
		// Clear the space key state to continuously generate bullets
		g.keys[XK_space] = 0;
	}

	// Update physics for platforms
	gameManager.updatePhysics();

    // Check for landing failure...
    if (player.pos[1] < 0.0) {
        g.failed_landing = 1;
    }
}


void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw Grid
	glPushMatrix();
	glBegin(GL_QUADS); 
    //Set the background color of the grid to black
	glColor3ub(0, 0, 0); // Set the vertex color to black
	//Set the background color of the grid to off-white
    // glColor3ub(240, 240, 240);
    glVertex2i(0, 0);
    glVertex2i(g.xres, 0);
    glVertex2i(g.xres, g.yres);
    glVertex2i(0, g.yres);
    //Set the color of the grid lines to gray
	glColor3ub(90, 90, 90); // Set the vertex color to gray
    // glColor3ub(255, 150, 50); // Set the vertex color to orange
    for (int i = 0; i <= g.xres; i += 20) {
        glBegin(GL_LINES);
            glVertex2i(i, 0);
            glVertex2i(i, g.yres);
        glEnd();
    }
    for (int i = 0; i <= g.yres; i += 20) {
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
	static Platform random;

	random.draw_platform_random();
	random.physics_platform();

	// Draw the platform 2
	Platform platform2; // Declare an instance of the Platform class

	// Set the position explicitly
	platform2.pos[0] = 200.0f;
	platform2.pos[1] = 70.0f;

	// Draw the platform at the specified location
	platform2.draw_platform_fixed(platform2.pos[0], platform2.pos[1]);

	//Draw Player
	glPushMatrix();
	// glColor3ub(0, 0, 0); 
	glColor3ub(255, 255, 255);
	if (g.failed_landing)
		glColor3ub(250, 0, 0);
	if (g.landed)
		glColor3ub(0, 250, 0);  
	glTranslatef(player.pos[0], player.pos[1], 0.0f); 
	glRotated(player.angle, 0.0, 0.0, 1.0);
	glBegin(GL_TRIANGLES); 
		for (int i=0; i<3; i++) {
			glVertex2f(player.verts[i][0], player.verts[i][1]); 
		}
	glEnd();

	// Render the platforms
	gameManager.render();


	if (player.pos[0] > (pf.pos[0] - pf.width) && player.pos[0] < (pf.pos[0] + pf.width)) 
	{
		if (player.pos[1] > (pf.pos[1] - pf.height) && player.pos[1] < (pf.pos[1] + pf.height)) 
		{
			player.pos[1] = (pf.pos[1]) + pf.height; 
			player.vel[1] = 0.0;
			player.vel[0] = 0.0;
			if (player.angle > 0.0 || player.angle < 0.0) {
				g.failed_landing = 1;
			}
			else {
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
			if (player.angle > 0.0 || player.angle < 0.0) {
				g.failed_landing = 1;
			}
			else {
				// g.landed = 1;
			}
		}
	}

	// check for collision with dynamic platforms
	for (unsigned int i = 0; i < gameManager.platforms.size(); i++) 
	{
		Platform* platform = &gameManager.platforms[i];

		if (player.pos[0] > (platform->pos[0] - platform->width) && player.pos[0] < (platform->pos[0] + platform->width)) 
		{
			if (player.pos[1] > (platform->pos[1] - platform->height) && player.pos[1] < (platform->pos[1] + platform->height)) 
			{
				// Player is colliding with platform
				player.pos[1] = (platform->pos[1]) + platform->height;
				player.vel[1] = 0.0;
				player.vel[0] = 0.0;

				if (player.angle > 0.0 || player.angle < 0.0) {
					g.failed_landing = 1;
				}
				else {
					// Player landed successfully
					// g.landed = 1;
				}
			}
		}
	}


	// Draw the bullets
	for (int i = 0; i < player.nbullets; i++) {
		Bullet* b = &player.barr[i];
		// Log("draw bullet...\n");

		glColor3f(1.0, 0.0, 0.0); // red
		// glColor3f(0.0, 0.0, 0.0); // black

		// Adjust the size of the bullet by changing the vertex positions
		glBegin(GL_POINTS);
		glVertex2f(b->pos[0],      b->pos[1]);
		glVertex2f(b->pos[0] - 2.0f, b->pos[1]); 
		glVertex2f(b->pos[0] + 2.0f, b->pos[1]); 
		glVertex2f(b->pos[0],      b->pos[1] - 2.0f); 
		glVertex2f(b->pos[0],      b->pos[1] + 2.0f); 
		glColor3f(0.8, 0.8, 0.8); 
		glVertex2f(b->pos[0] - 2.0f, b->pos[1] - 2.0f); 
		glVertex2f(b->pos[0] - 2.0f, b->pos[1] + 2.0f); 
		glVertex2f(b->pos[0] + 2.0f, b->pos[1] - 2.0f); 
		glVertex2f(b->pos[0] + 2.0f, b->pos[1] + 2.0f); 
		glEnd();
	}

	if (g.failed_landing) {
		//show crash graphics here...
	}
	glPopMatrix();
}