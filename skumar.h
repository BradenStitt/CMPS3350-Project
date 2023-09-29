#ifndef SKUMAR_H
#define SKUMAR_H

#include <GL/gl.h> 

extern void display_border(int xres, int yres);

class Player {
	//the player rocket
	public:
	float pos[2];
	float vel[2];
	float verts[3][2];
	// float thrust;
	double angle;
	Player() {
		init();
	}
	void init() {
		pos[0] = 100.0f; 
		pos[1] = 40.0f; 

		vel[0] = vel[1] = 0.0f;
		//3 vertices of triangle-shaped rocket player
		verts[0][0] = -10.0f;
		verts[0][1] =   0.0f;
		verts[1][0] =   0.0f;
		verts[1][1] =  30.0f;
		verts[2][0] =  10.0f;
		verts[2][1] =   0.0f;
		angle = 0.0;
	}

} player;

#endif