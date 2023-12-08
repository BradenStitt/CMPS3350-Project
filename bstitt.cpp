// Braden Stitt
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
#include "log.h"
#include "bstitt.h"
#include "global.h"
#include "jrivera2.h"
#include "bruiz.h"
#include "skumar.h"
#include <algorithm>
// #include "fonts.h"

using namespace std;

extern Global g;
extern GameManager gameManager;
extern Player player;
extern Platform trophy;
extern Platform testEnemy;
extern int snehalTest;
extern void makeTrophy();
extern void makeHeart(int i);

// void display_name(int x, int y, const char *name)
// {
//     Rect r;
//     r.bot = y;
//     r.left = x;
//     r.center = 0;
//     ggprint8b(&r, 16, 0x00ff0000, "%s", name);
// }

void display_border(int xres, int yres)
{
    // Draw a border around the window
    int b = 50;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    glPushMatrix();
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2i(0, 0);
    glVertex2i(0 + b, 0 + b);
    glVertex2i(0, 0 + yres);

    glVertex2i(0 + b, 0 + yres - b);
    glVertex2i(xres, 0 + yres);
    glVertex2i(xres - b, 0 + yres - b);

    glVertex2i(xres, 0);
    glVertex2i(xres - b, b);
    glVertex2i(0, 0);

    glVertex2i(0 + b, 0 + b);

    glEnd();
    glPopMatrix();
}

Platform::Platform()
{
    pos[0] = rand() % g.xres; // Random x coordinate
    pos[1] = g.yres;          // Start at the top of the screen
    width = 50.0f;
    height = 8.0f;
    velocity = 5.0f;
    isLanded = false;
    isDestroyed = false;
    defaultTrophyColor = true;
    countLanding = 0;
    disappearTimer = 0;
    hitCount = 0;
}

void Platform::draw_platform_fixed(float x, float y)
{
    // Draw the platform using the specified coordinates
    glPushMatrix();
    if (pType == -1) {
        glColor3ub(0, 0, 0);
        width = 700.0f;
    } else if (pType == 1)
        glColor3ub(0, 0, 250); // blue
    else if (pType == 2)
        glColor3ub(165, 42, 42); // brown
    else if (pType == 3) {
        if (!isDestroyed) {
            glColor3ub(250, 0, 0); // red
            // set it to a square
            width = 15.0f;
            height = 15.0f;
        } else {
            width = 0.0f;
            height = 0.0f;
        }
    } else if (pType == 4) {
        glColor3ub(0, 0, 0);
        // set it to a square
        width = 15.0f;
        height = 15.0f;
    } else if (pType == 5)
        glColor3ub(255, 0, 255);
    else if (pType == 6) {
        if (player.trophyDetected != 0)
            glColor3ub(0, 255, 255); // cyan
        if (defaultTrophyColor)
            glColor3ub(250, 250, 20); // yellow
        width = 10.0f;
        height = 10.0f;
    } else
        glColor3ub(250, 250, 20); // yellow

    if (pType == 6) {
        glTranslatef(x, y, 0.0f);
        makeTrophy();
    } else {
        glTranslatef(x, y, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(-width, -height);
        glVertex2f(-width, height);
        glVertex2f(width, height);
        glVertex2f(width, -height);
        glEnd();
        glColor3ub(20, 20, 20);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-width, -height);
        glVertex2f(-width, height);
        glVertex2f(width, height);
        glVertex2f(width, -height);
        glVertex2f(-width, -height);
        glEnd();
        glPopMatrix();
    }
}

void Platform::draw_platform_random()
{
    // Draw the platform using the specified coordinates
    glPushMatrix();
    if (pType == 1)
        glColor3ub(0, 0, 250); // blue
    else if (pType == 2)
        glColor3ub(165, 42, 42); // brown
    else if (pType == 3) {
        if (!isDestroyed) {
            glColor3ub(250, 0, 0); // red
            // set it to a square
            width = 15.0f;
            height = 15.0f;
        } else {
            width = 0.0f;
            height = 0.0f;
        }
    } else if (pType == 4) {
        glColor3ub(0, 0, 0);
        // set it to a square
        width = 25.0f;
        height = 25.0f;
    } else if (pType == 5)
        glColor3ub(255, 0, 255);
    else if (pType == 6) {
        width = 10.0f;
        height = 10.0f;
    } else
        glColor3ub(250, 250, 20); // yellow

    glTranslatef(pos[0], pos[1], 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-width, -height);
    glVertex2f(-width, height);
    glVertex2f(width, height);
    glVertex2f(width, -height);
    glEnd();
    glColor3ub(20, 20, 20);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-width, -height);
    glVertex2f(-width, height);
    glVertex2f(width, height);
    glVertex2f(width, -height);
    glVertex2f(-width, -height);
    glEnd();
    glPopMatrix();
}

void Platform::physics_platform()
{
    // Move the platform down the screen
    if (pos[1] > -50.0f && pType != 4) {
        if (pType == 1 || pType == 3)
            pos[1] -= 1.0f;
        else
            pos[1] -= 2.0f;

        // Snehal's Test on Mac
        // pos[1] -= 2.0f;
        usleep(3500);
    }

    // If the platform is a moving platform or an enemy, move it side to side
    if (pType == 1 || pType == 3) {
        // Move the platform side to side, bouncing off edges of the screen
        pos[0] += velocity;

        // Bounce off the right wall
        if (pos[0] + width > g.xres) {
            pos[0] = g.xres - width;
            velocity = -velocity; // Change direction
        }

        // Bounce off the left wall
        if (pos[0] - width < 0) {
            pos[0] = width;
            velocity = -velocity; // Change direction
        }
    } else if (pType == 2) {
        // Breaking platform
        if (isLanded == true)
            pos[1] = -100.0f;
    } else if (pType == 4) {
        // Show the platform for only 5 seconds
        disappearTimer++;

        if (disappearTimer >= 200) {
            // Assuming 60 frames per second, 300 frames is 5 seconds
            // After 5 seconds, make the platform disappear
            pos[1] = -100.0f;
            blackholeExists = false;
        }
    } else if (pType == 5) {
        enemy.pos[1] = pos[1] + height;

        // Move the enemy side to side along with the platform
        enemy.pos[0] += enemy.velocity;

        // Check if the enemy reaches the right & left edge of the platform
        if (enemy.pos[0] + enemy.width > pos[0] + width) {
            // Reverse the direction when reaching the right edge
            enemy.pos[0] = pos[0] + width - enemy.width;
            enemy.velocity = -enemy.velocity;
        } else if (enemy.pos[0] - enemy.width < pos[0] - width) {
            // Reverse the direction when reaching the left edge
            enemy.pos[0] = pos[0] - width + enemy.width;
            enemy.velocity = -enemy.velocity;
        }
    }
}

// Game Manager class
GameManager::GameManager(int numPlatforms) : platformCreationTimer(0) {}

void GameManager::createPlatform()
{
    int platformType = rand() % 20; // Random number between 0 and 19

    Platform newPlatform;

    // Level 1 Probability
    if (player.trophyDetected == 0) {
        if (platformType == 0 || platformType == 1 || platformType == 2 || platformType == 3 || platformType == 4 || platformType == 5 || platformType == 6 || platformType == 7 || platformType == 8 || platformType == 9)
            newPlatform.pType = 0;
        else if (platformType == 10 || platformType == 11 || platformType == 12 || platformType == 13 || platformType == 14 ||
                 platformType == 15 || platformType == 16 || platformType == 17 || platformType == 18 || platformType == 19)
            newPlatform.pType = 1;
        // Level 2 Probability
    } else if (player.trophyDetected == 1) {
        if (platformType == 0 || platformType == 1 || platformType == 2 || platformType == 3 || platformType == 4 || platformType == 5 || platformType == 6 || platformType == 7 || platformType == 8 || platformType == 9)
            newPlatform.pType = 0;
        else if (platformType == 10 || platformType == 11 || platformType == 12 || platformType == 13 || platformType == 14)
            newPlatform.pType = 1;
        else if (platformType == 15 || platformType == 16 || platformType == 17 || platformType == 18 || platformType == 19)
            newPlatform.pType = 2;
        // Level 3 Probability
    } else if (player.trophyDetected == 2) {
        if (platformType == 0 || platformType == 1 || platformType == 2 || platformType == 3 || platformType == 4 || platformType == 5 || platformType == 6 || platformType == 7 || platformType == 8 || platformType == 9)
            newPlatform.pType = 0;
        else if (platformType == 10 || platformType == 11 || platformType == 12 || platformType == 13)
            newPlatform.pType = 1;
        else if (platformType == 14 || platformType == 15 || platformType == 16)
            newPlatform.pType = 2;
        else if (platformType == 17 || platformType == 18 || platformType == 19)
            newPlatform.pType = 5;
        // Level 4 Probability
    } else if (player.trophyDetected == 3) {
        if (platformType == 0 || platformType == 1 || platformType == 2 || platformType == 3 || platformType == 4 || platformType == 5 || platformType == 6 || platformType == 7 || platformType == 8 || platformType == 9)
            newPlatform.pType = 0;
        else if (platformType == 10 || platformType == 11 || platformType == 12 || platformType == 13)
            newPlatform.pType = 1;
        else if (platformType == 14 || platformType == 15)
            newPlatform.pType = 2;
        else if (platformType == 16 || platformType == 17)
            newPlatform.pType = 5;
        else if (platformType == 18 || platformType == 19) {
            newPlatform.pType = 4;
            newPlatform.pos[1] = rand() % (g.yres / 2) + (g.yres / 2);
            newPlatform.blackholeExists = true;
        }
        // Level 5 Probability
    } else if (player.trophyDetected == 4) {
        if (platformType == 0 || platformType == 1 || platformType == 2 || platformType == 3 || platformType == 4 || platformType == 5 || platformType == 6 || platformType == 7 || platformType == 8 || platformType == 9)
            newPlatform.pType = 0;
        else if (platformType == 10 || platformType == 11 || platformType == 12 || platformType == 13)
            newPlatform.pType = 1;
        else if (platformType == 14 || platformType == 15)
            newPlatform.pType = 2;
        else if (platformType == 16)
            newPlatform.pType = 3;
        else if (platformType == 17 && !newPlatform.blackholeExists) {
            newPlatform.pType = 4;
            newPlatform.pos[1] = rand() % (g.yres / 2) + (g.yres / 2);
            newPlatform.blackholeExists = true;
        } else if (platformType == 18 || platformType == 19)
            newPlatform.pType = 5;
    }

    platforms.push_back(newPlatform);
    platformCreationTimer = 0; // Reset the timer
}

void GameManager::updatePhysics()
{
    platformCreationTimer++;
    if (platformCreationTimer >= PLATFORM_CREATION_INTERVAL) {
        createPlatform();
        platformCreationTimer = 0; // Reset the timer
    }

    // Remove platforms that are out of screen bounds
    platforms.erase(
        remove_if(platforms.begin(), platforms.end(), [](const Platform &p) {
                      return p.pos[1] < -50.0f; // Adjust the value based on your requirements
                  }),
        platforms.end());

    // Update the physics for remaining platforms
    for (auto &platform : platforms)
        platform.physics_platform();
}

void GameManager::render()
{
    // 120 frames ~ 2 seconds (assuming 60 frames per second)
    platformCreationTimer++;
    if (platformCreationTimer >= 120)
        createPlatform();

    for (size_t i = 0; i < platforms.size(); i++) {
        platforms[i].draw_platform_random();
        if (platforms[i].pType == 5)
            platforms[i].enemy.drawEnemy();
    }
}

void GameManager::resetGame()
{
    platforms.clear();
    platformCreationTimer = 0;
    trophy.defaultTrophyColor = true;
}

int total_running_time(const bool running)
{
    static int first_run = 1;
    static int start_time;
    if (first_run) {
        start_time = time(NULL);
        first_run = 0;
    }
    if (running)
        return time(NULL) - start_time;
    return 0;
}

int time_since_mouse_moved(const bool get)
{
    static int first_run = 1;
    static int last_time;
    if (first_run) {
        first_run = 0;
        last_time = time(NULL);
    }
    if (get)
        return time(NULL) - last_time;

    last_time = time(NULL);
    return 0;
}

void render_hearts()
{
    // Render the hearts
    for (int i = 0; i < player.lives; i++)
        makeHeart(i);
}