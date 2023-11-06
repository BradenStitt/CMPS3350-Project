#ifndef SKUMAR_H
#define SKUMAR_H

#include <GL/gl.h>

extern void display_border(int xres, int yres);
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

#define rnd() (float)rand() / (float)RAND_MAX
const float GRAVITY = 0.5;
const int MAX_BULLETS = 5;
typedef float Vec[3];

void dynamic_collision_detection();
int count_render_function();
int print_score();

class Bullet
{
public:
	Vec pos;
	Vec vel;
	bool bulletHit;
	float color[3];
	float prevPosY;
	struct timespec time;

	Bullet();
	void physics();
	void draw_bullet();
};

class Player
{
public:
	Bullet *barr;
	int nbullets;
	struct timespec bulletTimer;

	float pos[2];
	float vel[2];
	float verts[4][2];
	float width, height;
	int jumpCount;
	int enemyDetected;
	int blackholeDetected;
	int score;
	// float thrust;
	double angle;

	void init();

	Player();
	void physics();
	void draw_player();
	~Player();
};

#endif