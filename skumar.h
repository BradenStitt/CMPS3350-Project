#ifndef SKUMAR_H
#define SKUMAR_H

#include <GL/gl.h> 

extern void display_border(int xres, int yres); 
extern double timeDiff(struct timespec *start, struct timespec *end); 
extern void timeCopy(struct timespec *dest, struct timespec *source);

#define rnd() (float)rand() / (float)RAND_MAX
const float GRAVITY = 0.00005; 
const int MAX_BULLETS = 11;
typedef float Vec[3];

void dynamic_collision_detection();

class Bullet {
	public:
		Vec pos;
		Vec vel;
		float color[3];
		struct timespec time;

		Bullet();
		void physics();
		void draw_bullet();
};

class Player {
	//the player rocket
	public:
	Bullet *barr;
	int nbullets;
	struct timespec bulletTimer;

	float pos[2];
	float vel[2];
	float verts[3][2];
	// float thrust;
	double angle;

	void init();
	
	Player(); 
	void physics();
	void draw_player();
	~Player();
};

#endif