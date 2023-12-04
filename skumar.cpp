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
#include "fonts.h"
#include "skumar.h"
#include "bstitt.h"
#include "bruiz.h"
#include "jrivera2.h"

using namespace std;

struct timespec bt;

extern Global g;
extern GameManager gameManager;
extern Player player;
extern Enemy enemy;
extern Platform testEnemy;
extern Platform testPlatform;
extern vector<Platform> testEnemies;
extern vector<Platform> testPlatforms;
extern Texture s;
extern Texture soccer;
extern Rect r;
extern int snehalTest;
extern bool bKey;
extern OpenALPlayer openALPlayer;

int renderCount = 0;

Bullet::Bullet()
{
	prevPosY = 0.0f;
}

void Bullet::physics()
{
	if (snehalTest)
		usleep(20000); // Mac 

	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);

	if (g.keys[XK_space]) {
		// Shoot a bullet...
		if (player.nbullets < MAX_BULLETS) {
			Bullet *b = &player.barr[player.nbullets];
			timeCopy(&b->time, &bt);

			// Adjust the y-position so it's just above the player with
            // more distance
			b->pos[1] = 38.0f;
			b->vel[0] = 0.0f;
			b->vel[1] = 8.0f + rnd() * 0.05f;
			b->color[0] = 1.0f;
			b->color[1] = 1.0f;
			b->color[2] = 1.0f;
			++player.nbullets;
		}
		// Play shoot sound
		openALPlayer.playSound("./arcadeLaser.wav"); 

		// Clear the space key state to continuously generate bullets
		g.keys[XK_space] = 0;
	}

	for (int i = player.nbullets - 1; i >= 0; i--) {
		Bullet *b = &player.barr[i];
		b->bulletHit = false;
		// How long has the bullet been alive?
		double ts = timeDiff(&b->time, &bt);

		if (ts > 2.5) {
			// Time to delete the bullet.
			if (i < player.nbullets - 1) {
				// Swap the current bullet with the last one and
                // decrease the bullet count 
				memcpy(&player.barr[i], &player.barr[player.nbullets - 1],
                                                         sizeof(Bullet));
			}
			player.nbullets--;
		} else {
			// Move the bullet
			b->pos[0] += b->vel[0];
			b->pos[1] += b->vel[1];

			// Calculate previous position of the bullet.
			b->prevPosY = b->pos[1] - b->vel[1];

			if (snehalTest) {
				// Check for collision with testEnemy
				for (unsigned int k = 0; k < testEnemies.size(); k++) {
					Platform *testEnemy = &testEnemies[k];

					if (player.pos[0] > testEnemy->pos[0] - testEnemy->width 
					&& player.pos[0] < testEnemy->pos[0]+testEnemy->width) {
						if (b->prevPosY>=testEnemy->pos[1]-testEnemy->height 
						&& b->pos[1]<=testEnemy->pos[1]+testEnemy->height) {
							b->pos[1] = testEnemy->pos[1]+testEnemy->height;
							b->bulletHit = true;
						}
					}
				
					if (b->bulletHit) {
						testEnemy->hitCount++;
						// Remove the bullet
						if (i < player.nbullets - 1) {
							// Swap the current bullet with the last one and 
							// decrease the bullet count.
							memcpy(&player.barr[i], 
									&player.barr[player.nbullets-1], 
															sizeof(Bullet));
						}
						player.nbullets--;

						if (testEnemy->hitCount >= 2) {
							if (testEnemy->pType == 3) {
								testEnemy->isDestroyed = true;
								player.score += 30;
							}
							testEnemy->hitCount = 0;
							testEnemies.erase(testEnemies.begin() + k);
						}
					}
				}
			} else {
				/* dynamic enemy bullet collision */
				for (unsigned int j=0; j<gameManager.platforms.size();j++) {
					Platform *platform = &gameManager.platforms[j];

					if (platform->pType == 3 || platform->pType == 5) {
						if (platform->pType == 3) {
							if (player.pos[0]>platform->pos[0] - 
															platform->width 
								&& player.pos[0] < platform->pos[0] 
														+ platform->width) {
								if (b->prevPosY>=platform->pos[1]
														-platform->height
									&& b->pos[1] <= platform->pos[1]
														+platform->height) {
									b->pos[1] = platform->pos[1]
														+platform->height;
									b->bulletHit = true;
								}
							}
						} else {
							if (player.pos[0] > platform->enemy.pos[0] 
								- platform->enemy.width && player.pos[0] < 
								platform->enemy.pos[0] 
												+ platform->enemy.width) {
								if (b->prevPosY >= platform->enemy.pos[1] 
									- platform->enemy.height && b->pos[1] 
												<= platform->enemy.pos[1] 
												+ platform->enemy.height) {
									b->pos[1] = platform->enemy.pos[1] 
													+platform->enemy.height;
									b->bulletHit = true;
								}
							}
						}
					
						if (b->bulletHit) {
							if (platform->pType == 3) {
								platform->hitCount++;
							} else {
								platform->enemy.hitCount++;
							}

							// Remove the bullet
							if (i < player.nbullets - 1) {
								// Swap the current bullet with the last one
								// and decrease the bullet count.
								memcpy(&player.barr[i], 
										&player.barr[player.nbullets-1], 
															sizeof(Bullet));
							}
							player.nbullets--;

							if (platform->hitCount >= 1 
								|| platform->enemy.hitCount >= 1) {
								if (platform->pType == 3) {
									platform->isDestroyed = true;
									player.score += 30;
								} else {
									platform->enemy.isDestroyed = true;
									player.score += 30;
								}
							}
						}
					}
				}
			}

			// Check for collision with window edges
			if (b->pos[0] < 0.0) {
				b->pos[0] = 0.0f;
			} else if (b->pos[0] > g.xres) {
				b->pos[0] = g.xres;
			}
		}
	}
}

// Draws the bullet
void Bullet::draw_bullet()
{
	for (int i = 0; i < player.nbullets; i++) {
        Bullet *b = &player.barr[i];

        glColor3f(1.0, 0.0, 0.0); // red
        // Adjust size of bullet
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        glVertex2f(b->pos[0], b->pos[1]);
        glEnd();
        // Reset point size to default value
        glPointSize(1.0f);
    }
}

Player::Player()
{
	init();
	barr = new Bullet[MAX_BULLETS];
	nbullets = 0;
	clock_gettime(CLOCK_REALTIME, &bulletTimer);
	lives = 3;
	trophyDetected = 0;
}

Player::~Player()
{
	delete [] barr;
}

void Player::init()
{
	pos[0] = 200.0f;
	pos[1] = 40.0f;

	vel[0] = vel[1] = 0.0f;
	// rectangle shaped player
	width = 15.0f;
	height = 15.0f;

	verts[0][0] = -15.0f;
	verts[0][1] = 0.0f;
	verts[1][0] = -15.0f;
	verts[1][1] = 30.0f;
	verts[2][0] = 15.0f;
	verts[2][1] = 30.0f;
	verts[3][0] = 15.0f;
	verts[3][1] = 0.0f;

	jumpCount = 0;
	enemyDetected = 0;
	blackholeDetected = 0;
	g.failed_landing = 0;
	angle = 0.0;
}

// Physics for moving the player
void Player::physics()
{
	if (snehalTest)
		usleep(10000); // Mac

	// Player physics
	if (g.failed_landing)
		return;

	player.pos[0] += player.vel[0];
	player.pos[1] += player.vel[1];
	player.vel[1] -= GRAVITY;

	// Check keys pressed now
	if (g.keys[XK_Left]) {
		if (!enemyDetected)
			player.vel[0] -= 0.8;
		else 
			player.vel[0] = 0.0;
	}
	
	if (g.keys[XK_Right]) {
		if (!enemyDetected)
			player.vel[0] += 0.8;
		else 
			player.vel[0] = 0.0;
	}
	
	if (g.keys[XK_Up]) {
		if (jumpCount < 2) {
			player.vel[1] += 4.8;
			jumpCount++;
		}
	}

	// Check for collision with window edges
	if (pos[0] < 0.0) {
		player.pos[0] += g.xres;
		// player.pos[0] = 0.0f;
	} else if (pos[0] > g.xres) {
		player.pos[0] -= g.xres;
		// player.pos[0] = g.xres;
	}

	// check for landing failure...
	if (player.pos[1] < 0.0) {
		life_lost();
	}
	
}

// Draws the player
void Player::draw_player()
{
	glPushMatrix();
	if (snehalTest)
		glColor3ub(255, 255, 255);
	else
		glColor3ub(255, 165, 0); 
	if (enemyDetected)
		glColor3ub(255, 165, 0); // orange
	if (blackholeDetected)
		glColor3ub(250, 0, 250); // purple
	if (g.failed_landing)
		glColor3ub(250, 0, 0); // red
	// draws the player
	glTranslatef(player.pos[0], player.pos[1], 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	if (snehalTest) {
		glBegin(GL_QUADS);
			glVertex2f(verts[0][0], verts[0][1]);
			glVertex2f(verts[1][0], verts[1][1]);
			glVertex2f(verts[2][0], verts[2][1]);
			glVertex2f(verts[3][0], verts[3][1]);
		glEnd();
	} else {
		makeSprite();
	}
}

void dynamic_collision_detection()
{
	// check for collision with dynamic platforms
	if (!player.enemyDetected && !player.blackholeDetected) {
		for (unsigned int i = 0; i < gameManager.platforms.size(); i++) {
			Platform *platform = &gameManager.platforms[i];

			if (platform->pType == 3 || platform->pType == 4) {
				if ((player.pos[0] + player.width > platform->pos[0]
				- platform->width && player.pos[0] <=platform->pos[0]) || 
					(player.pos[0] - player.width < platform->pos[0] + 
					platform->width && player.pos[0] >= platform->pos[0])) {
					if ((player.pos[1] - player.height <= platform->pos[1] + 
					platform->height && player.pos[1] - player.height >= 
														platform->pos[1]) || 
					(player.pos[1] + player.height >= platform->pos[1] - 
					platform->height && player.pos[1] + 
										player.height <=platform->pos[1])) {
						if (platform->pType == 3) {
							player.enemyDetected = 1;
							player.vel[1] = -8.0f;
						}		

						if (platform->pType == 4) {
							player.blackholeDetected = 1;
						}
					}
				}
			} else {
				if (player.pos[0] > (platform->pos[0] - platform->width) 
				&& player.pos[0] < (platform->pos[0] + platform->width)) {
					if ((platform->pType == 5 &&platform->enemy.isDestroyed) 
												|| platform->pType != 5) {
						if (player.pos[1] >
										(platform->pos[1]-platform->height) 
						&& player.pos[1] < (platform->pos[1] 
													+ platform->height)) {
							// Player is colliding with platform
							platform->isLanded = true;
							platform->countLanding++;

							player.pos[1] = platform->pos[1]
														+platform->height;
							player.vel[1] = 0.0;
							player.vel[0] = 0.0;
							player.jumpCount = 0;

							if (platform->countLanding == 1 
												&& platform->pType != 2) {
								player.score += 10;
							}

							if (platform->pType == 2) {
								openALPlayer.playSound("./eggCrack.wav");
							}

							if (player.angle > 0.0 || player.angle < 0.0) {
								g.failed_landing = 1;
							} 
						}
					} else {
						if ((player.pos[0]+player.width>
													platform->enemy.pos[0]
						 - platform->enemy.width && player.pos[0] 
						 		<= platform->enemy.pos[0]) ||(player.pos[0]- 
						 				player.width<platform->enemy.pos[0]+ 
						 				platform->enemy.width&&player.pos[0] 
						 						>=platform->enemy.pos[0])) {
							if ((player.pos[1] - player.height <= 
							platform->enemy.pos[1]+platform->enemy.height && 
							player.pos[1] - player.height >= 
												platform->enemy.pos[1]) || 
							(player.pos[1] + player.height >= 
							platform->enemy.pos[1]-platform->enemy.height &&
							player.pos[1] + player.height 
												<=platform->enemy.pos[1])) {
								player.enemyDetected = 1;
								player.vel[1] = -8.0f;
							}
						}
					}
				}
			}
		}
	}
}

void blackhole_screen()
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2i(0, 0);
	glVertex2i(0, g.yres);
	glVertex2i(g.xres, g.yres);
	glVertex2i(g.xres, 0);
	glEnd();
}

void life_lost()
{
	if (player.lives == 1) {
		g.failed_landing = 1;
		player.pos[1] = 0.0;
		player.blackholeDetected = 1;
	} else if (snehalTest) {
		player.lives--;
		g.failed_landing = 0;
		player.enemyDetected = 0;
		player.pos[0] = g.xres / 2;
		player.pos[1] = 60.0f;
		player.vel[0] = player.vel[1] = 0.0f;
	} else {
		if (!player.blackholeDetected) {
			player.lives--;
			player.init();
		}
	}
}

int count_render_function()
{
	renderCount++;
	return renderCount;
}

int print_score()
{
	return player.score;
}

void snehalTestBackground()
{
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, soccer.tex.backTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(soccer.tex.xc[0], soccer.tex.yc[1]); glVertex2i(0,0);
		glTexCoord2f(soccer.tex.xc[0], soccer.tex.yc[0]); 
													glVertex2i(0,g.yres);
		glTexCoord2f(soccer.tex.xc[1], soccer.tex.yc[0]); 
												glVertex2i(g.xres, g.yres);
		glTexCoord2f(soccer.tex.xc[1], soccer.tex.yc[1]); 
													glVertex2i(g.xres, 0);
	glPopMatrix();
}

void snehalsText()
{
	r.center = 0;
	r.bot = g.yres - 20;
	r.left = 10;
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glVertex2f(40, g.yres - 110.0);  
    glVertex2f(40, g.yres - 160.0);  
    glVertex2f(g.xres - 50.0, g.yres - 160.0);  
	glVertex2f(g.xres - 50.0, g.yres - 110.0);

	r.bot -= 20;
    ggprint8b(&r, 64, 0x00000000, "");
	r.bot -= 43;
    ggprint16(&r, 24, 0xFFFFFFFF, "         SNEHAL'S FEATURE MODE");

	glEnd();
    glPopMatrix();
}

void pushTestPlatforms()
{
	for (int i = 0; i < 3; i++) {
		Platform testEnemy;
		testEnemy.pos[0] = g.xres / 2 - 100 + (i * 100);
		testEnemy.pos[1] = 250.0f;
		testEnemy.pType = 3;
		testEnemies.push_back(testEnemy);
	}

	if (!bKey) {
		for (int j = 0; j < 2; j++) {
			Platform testPlatform;
			testPlatform.pos[0] = g.xres / 2 - 100 + (j * 200);
			if (j == 0) {
				testPlatform.pos[1] = 100.0f;
			}
			else if (j == 1) {
				testPlatform.pos[1] = 150.0f;
			}
			testPlatform.pType = 1;
			testPlatforms.push_back(testPlatform);
		}
	}
}

void renderTestPlatforms()
{
	for (unsigned int j = 0; j < testEnemies.size(); j++) {
		testEnemy = testEnemies[j];
		testEnemy.draw_platform_fixed(testEnemy.pos[0], testEnemy.pos[1]);
	}

	for (unsigned int i = 0; i < testPlatforms.size(); i++) {
		testPlatform = testPlatforms[i];
		testPlatform.draw_platform_fixed(testPlatform.pos[0], 
													testPlatform.pos[1]);
	}
}