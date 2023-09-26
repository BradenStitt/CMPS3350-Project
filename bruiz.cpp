// Brian Ruiz Showcase File

#include "fonts.h"
#include <GL/glx.h>
void display_borderint x, int y)
{
    // draw a border around the window
    int b =  50; 
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2i(0, 0);
        glVertex2i(0 + b, 0 + b);
        glVertex2i(0 , 0 + yres);
        glVertex2i(0, 0);

        glEnd();
        glPopMatrix();
}
void display_name(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 0;
    ggprint8b(&r, 0x00ffff00, "Brian")
}
// Adding enemies to the game