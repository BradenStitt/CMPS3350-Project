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
	
	Player();
	void init();
	void physics():
	void draw_player();
};

#endif