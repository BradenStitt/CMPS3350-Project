// This is snehal's individual source file

#include <GL/glx.h>
#include "fonts.h"

void display_border(int xres, int yres)
{
    //Draw a border around the window
    int b = 50;
	glColor3f(1.0f, 1.0f, 0.0f);
	glPushMatrix();
	glBegin(GL_TRIANGLES_STRIP);
        glVertex2i(0,            0);
        glVertex2i(0+b         0+b);
        glVertex2i(0,       0+yres);
        glVertex2i(0+b,     0+yres-b);
        glVertex2i(xres     0+yres);
        glVertex2i(xres-b,     0+yres-b);
        glVertex2i(xres,     0);
        glVertex2i(xres-b,     b);
        glVertex2i(0,     0);
        glVertex2i(0+b         0+b);
	glEnd();
	glPopMatrix();
}

void display_name(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 0;
    ggprint8b(&r, 0, 0x00000000, "Snehal");
}