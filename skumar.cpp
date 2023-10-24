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
#include "log.h"
#include "global.h"
#include "skumar.h"
#include "bstitt.h"

using namespace std;

struct timespec bt;

extern Global g;
extern GameManager gameManager;
extern Player player;

int renderCount = 0;

Bullet:: Bullet()
{
}

void Bullet:: physics()
{
	if (g.keys[XK_space]) {
   		// Shoot a bullet...
		if (player.nbullets < MAX_BULLETS) {
			Bullet* b = &player.barr[player.nbullets];
			timeCopy(&b->time, &bt);

			// Adjust the y-position so it's just above the player with more distance
			b->pos[1] = 38.0f;
			// Set bullet velocity to move farther upwards
			b->vel[0] = 0.0f;
			b->vel[1] = 8.0f + rnd() * 0.05f; // Adjust for more spread
			// b->vel[1] = 10.0f; // Adjust as needed
			b->color[0] = 1.0f; 
			b->color[1] = 1.0f; 
			b->color[2] = 1.0f;
			++player.nbullets;
		}
		// Clear the space key state to continuously generate bullets
		g.keys[XK_space] = 0;
	}
}

// Draws the bullet
void Bullet:: draw_bullet()
{
	for (int i = 0; i < player.nbullets; i++) {
		Bullet* b = &player.barr[i];
		
		glColor3f(1.0, 0.0, 0.0); // red

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
}


Player:: Player()
{
	init();
	barr = new Bullet[MAX_BULLETS];
	nbullets = 0;
	clock_gettime(CLOCK_REALTIME, &bulletTimer);
}

Player:: ~Player()
{
	delete [] barr;
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
	jumpCount = 0;
	angle = 0.0;
}

// Physics for moving the player
void Player::physics()
{
    //Player physics
	if (g.failed_landing)
	   return;

	pos[0] += vel[0]; 
	pos[1] += vel[1];
	vel[1] -= GRAVITY;

	// Check for collision with window edges
	if (player.pos[0] < 0.0) {
		player.pos[0] += (float)g.xres;
	}
	else if (player.pos[0] > (float)g.xres) {
		player.pos[0] -= (float)g.xres;
	}
	// else if (player.pos[1] > (float)g.yres) {
	// 	player.pos[1] -= (float)g.yres;
	// }
	
	// Check keys pressed now
    if (g.keys[XK_Left])
        player.vel[0] -= 0.8;
		//player.vel[0] -= 0.1;
    if (g.keys[XK_Right])
         player.vel[0] += 0.8;
		//player.vel[0] += 0.1;
    if (g.keys[XK_Up])
		if (player.jumpCount < 2) {
			player.vel[1] += 4.8;
			player.jumpCount++;
		} 

	//check for landing failure...
	if (pos[1] < 0.0) {
		g.failed_landing = 1;
	}
}

// Draws the player
void Player::draw_player()
{
    glPushMatrix();
	glColor3ub(255, 255, 255); 
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

void dynamic_collision_detection()
{
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
				player.jumpCount = 0;

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
}

int count_render_function()
{
	renderCount++;
	return renderCount;
}