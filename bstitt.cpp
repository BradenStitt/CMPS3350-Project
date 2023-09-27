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

using namespace std;

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
