/**
 * Author:    Snehal Kumar
 * Created:   09.29.2023
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
#include "fonts.h"
#include "log.h"
#include "skumar.h"

using namespace std;

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

Player::void physics()
{
    //Player physics
	if (g.failed_landing)
	   return;

	player.pos[0] += player.vel[0]; 
	player.pos[1] += player.vel[1];
	player.vel[1] -= GRAVITY;
	
	if (g.keys[XK_Left])
		player.vel[0] -= 0.0001;
	if (g.keys[XK_Right])
		player.vel[0] += 0.0001;
	if (g.keys[XK_Up])
		player.vel[1] += 0.0002; 

	//check for landing failure...
	if (player.pos[1] < 0.0) {
		g.failed_landing = 1;
	}
}

Player::void draw_player()
{
    glPushMatrix();
	glColor3ub(0, 0, 0); 
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
}