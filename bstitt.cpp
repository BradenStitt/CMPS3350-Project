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
//#include "fonts.h"

using namespace std;

// void display_name(int x, int y, const char *name)
// {
//     Rect r;
//     r.bot = y;
//     r.left = x;
//     r.center = 0;
//     ggprint8b(&r, 16, 0x00ff0000, "%s", name);
// }

void display_border(int xres, int yres)
{
    // Draw a border around the window
    int b = 50;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    glPushMatrix();
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2i(0, 0);
    glVertex2i(0 + b, 0 + b);
    glVertex2i(0, 0 + yres);

    glVertex2i(0 + b, 0 + yres - b);
    glVertex2i(xres, 0 + yres);
    glVertex2i(xres - b, 0 + yres - b);

    glVertex2i(xres, 0);
    glVertex2i(xres - b, b);
    glVertex2i(0, 0);

    glVertex2i(0 + b, 0 + b);

    glEnd();
    glPopMatrix();
}

Platform::Platform() {
    pos[0] = rand() % 600 + 100; // Random x coordinate
    pos[1] = 600; // Random y coordinate
    width = 50.0f;
    height = 8.0f;
}

void Platform::draw_platform_fixed(float x, float y)
{
    // Draw the platform using the specified coordinates
    glPushMatrix();
    glColor3ub(250, 250, 20);
    glTranslatef(x, y, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-width, -height);
    glVertex2f(-width, height);
    glVertex2f(width, height);
    glVertex2f(width, -height);
    glEnd();
    glColor3ub(20, 20, 20);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-width, -height);
    glVertex2f(-width, height);
    glVertex2f(width, height);
    glVertex2f(width, -height);
    glVertex2f(-width, -height);
    glEnd();
    glPopMatrix();
}

void Platform::draw_platform_random()
{
    // Draw the platform using the specified coordinates
    glPushMatrix();
    glColor3ub(250, 250, 20);
    glTranslatef(pos[0], pos[1], 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-width, -height);
    glVertex2f(-width, height);
    glVertex2f(width, height);
    glVertex2f(width, -height);
    glEnd();
    glColor3ub(20, 20, 20);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-width, -height);
    glVertex2f(-width, height);
    glVertex2f(width, height);
    glVertex2f(width, -height);
    glVertex2f(-width, -height);
    glEnd();
    glPopMatrix();
}
