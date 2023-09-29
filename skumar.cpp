/**
 * Author:    Snehal Kumar
 * Created:   09.29.2023
 **/

#include <iostream>
#include <GL/gl.h>
#include <X11/keysym.h>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include "defs.h"
#include "log.h"
#include "global.h"
#include "skumar.h"

using namespace std;

extern Global g;

const float GRAVITY = 0.00005; 

// void display_border(int xres, int yres)
// {
//     // Draw a border around the window
//     int b = 50;
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     glEnable(GL_BLEND);
//     glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
//     glPushMatrix();
//     glBegin(GL_TRIANGLE_STRIP);
//     glVertex2i(0, 0);
//     glVertex2i(0 + b, 0 + b);
//     glVertex2i(0, 0 + yres);

//     glVertex2i(0 + b, 0 + yres - b);
//     glVertex2i(xres, 0 + yres);
//     glVertex2i(xres - b, 0 + yres - b);

//     glVertex2i(xres, 0);
//     glVertex2i(xres - b, b);
//     glVertex2i(0, 0);

//     glVertex2i(0 + b, 0 + b);

//     glEnd();
//     glPopMatrix();
// }

// void display_name(int x, int y)
// {
//     Rect r;
//     r.bot = y;
//     r.left = x;
//     r.center = 0;
//     ggprint8b(&r, 0, 0x00000000, "Snehal");
// }

Player:: Player()
{
	init();
}

void Player:: init()
{
	pos[0] = 100.0f; 
	pos[1] = 40.0f; 

	vel[0] = vel[1] = 0.0f;
	//3 vertices of triangle-shaped player 
	verts[0][0] = -10.0f;
	verts[0][1] =   0.0f;
	verts[1][0] =   0.0f;
	verts[1][1] =  30.0f;
	verts[2][0] =  10.0f;
	verts[2][1] =   0.0f;
	angle = 0.0;
}

// Physics for pressing keys and moving the player
void Player::physics()
{
    //Player physics
	if (g.failed_landing)
	   return;

	pos[0] += vel[0]; 
	pos[1] += vel[1];
	vel[1] -= GRAVITY;
	
	if (g.keys[XK_Left])
		vel[0] -= 0.0001;
	if (g.keys[XK_Right])
		vel[0] += 0.0001;
	if (g.keys[XK_Up])
		vel[1] += 0.0002; 

	//check for landing failure...
	if (pos[1] < 0.0) {
		g.failed_landing = 1;
	}
}

// Draws the player
void Player::draw_player()
{
    glPushMatrix();
	glColor3ub(0, 0, 0); 
	if (g.failed_landing)
		glColor3ub(250, 0, 0);
	if (g.landed)
		glColor3ub(0, 250, 0);  
	glTranslatef(pos[0], pos[1], 0.0f); 
	glRotated(angle, 0.0, 0.0, 1.0);
	glBegin(GL_TRIANGLES); 
		for (int i=0; i<3; i++) {
			glVertex2f(verts[i][0], verts[i][1]); 
		}
	glEnd();
}