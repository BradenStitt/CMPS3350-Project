/**
 * Author:    Joseph Rivera
 * Created:   09-25-2023
 * 
 **/

#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <vector>
#include "defs.h"
#include "log.h"
#include "bstitt.h"
#include "jrivera2.h"
#include "global.h"

using namespace std;

extern Global g;

Background::background() {

}

GLXContext Background::create_display(Display *dis, Window root) //creates a display
{
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w = g.xres, h = g.yres;
    if (dis == NULL) {
		cout << "\n\tcannot connect to X server\n" << endl;
		exit(EXIT_FAILURE);
	}
	XVisualInfo *vi = glXChooseVisual(dis, 0, att);
	if (vi == NULL) {
		cout << "\n\tno appropriate visual found\n" << endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dis, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;

    return glXCreateContext(dis, vi, NULL, GL_TRUE);
	
}

Window Background::create_window(Display *dis, Window root) //creates a Window
{
    win = XCreateWindow(dis, root, 0, 0, w, h, 0, vi->depth,
		InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
        return win;
}

void Background::background_display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw Sky
	glPushMatrix();
	glBegin(GL_QUADS);
//		//Each vertex has a color.
		glColor3ub(250, 200,  90); glVertex2i(0, 0);
		glColor3ub(100,  80, 200); glVertex2i(0, g.yres);
		glColor3ub(100,  80, 200); glVertex2i(g.xres, g.yres);
		glColor3ub(250, 200,  90); glVertex2i(g.xres, 0);
	glEnd();
	glPopMatrix();
}



