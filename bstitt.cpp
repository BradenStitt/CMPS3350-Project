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
    GLfloat red[] = {1.0f, 0.0f, 0.0f};
    glColor3fv(red);
    glPushMatrix();
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2i(0, 0);
    glVertex2i(0 + 100, 0 + 100);
    glVertex2i(0, 0 + yres);

    glEnd();
    glPopMatrix();
}

int main()
{
    cout << "Hello, World!\n";
    return 0;
}