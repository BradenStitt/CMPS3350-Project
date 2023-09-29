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

#ifdef
using namespace std;

/*
    Displays characters on specified coordinates
*/
void display_name(int x, int y, const char *name)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ff0000, "%s", name);
}

/*
    Displays a red border around the screen
*/
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

/*
    Helper functiont to draw platforms via vertices
*/
Platform::void draw_platform()
{
    // Draw the platform
    glPushMatrix();
    glColor3ub(90, 140, 90);
    glTranslatef(platform.pos[0], platform.pos[1], platform.pos[2]);
    glBegin(GL_QUADS);
    glVertex2i(-platform.width, 0);
    glVertex2i(-platform.width, platform.height);
    glVertex2i(platform.width, platform.height);
    glVertex2i(platform.width, 0);
    glEnd();
    glPopMatrix();
}

/*
    Spawns a platform at a random location
*/
Platform::void spawnPlatformRandom()
{
    // Intialize platform vertices at a random location
    platform.pos[0] = rand() % 600 + 100; // 100 - 700
    platform.pos[1] = rand() % 400 + 100; // 100 - 500
    platform.width = 50.0f;
    platform.height = 8.0f;

    // Draw the platform
    glPushMatrix();
    glColor3ub(90, 140, 90);
    glTranslatef(platform.pos[0], platform.pos[1], 0.0f);
    glBegin(GL_QUADS);
    glVertex2i(-platform.width, 0);
    glVertex2i(-platform.width, platform.height);
    glVertex2i(platform.width, platform.height);
    glVertex2i(platform.width, 0);
    glEnd();
    glPopMatrix();
}