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
#include "log.h"
#include "bstitt.h"
#include "global.h"
//#include "fonts.h"

using namespace std;

Global gl;

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

Platform::Platform() {
    pos[0] = rand() % gl.xres; // Random x coordinate
    pos[1] = gl.yres; // Start at the top of the screen
    width = 50.0f;
    height = 8.0f;
}

void Platform::draw_platform_fixed(float x, float y)
{
    // Draw the platform using the specified coordinates
    glPushMatrix();
    glColor3ub(250, 250, 20);
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

void Platform::draw_platform_random()
{
    // Draw the platform using the specified coordinates
    glPushMatrix();
    glColor3ub(250, 250, 20);
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
    // // Move the platform down the screen
    // pos[1] -= 5.0f;

    // // Limit how far below the screen the platform can go
    // float lowerLimit = -10.0f;  // Adjust as needed
    // if (pos[1] < lowerLimit)
    // {
    //     pos[1] = lowerLimit;
    // }

    if (pos[1] > 0.0f - height)
    {
        pos[1] -= 5.0f;
    }
}

// Game Manager class
GameManager::GameManager(int numPlatforms) : platformCreationTimer(0) {
}

void GameManager::createPlatform() {
    Platform newPlatform;
    platforms.push_back(newPlatform);
    platformCreationTimer = 0;  // Reset the timer
}

void GameManager::updatePhysics() {
    platformCreationTimer++;
    if (platformCreationTimer >= PLATFORM_CREATION_INTERVAL) {
        createPlatform();
        platformCreationTimer = 0;  // Reset the timer
    }

    for (auto& platform : platforms) {
        platform.physics_platform();
    }
}

void GameManager::render() {
    platformCreationTimer++;
    if (platformCreationTimer >= 120) {  // 120 frames ~ 2 seconds (assuming 60 frames per second)
        createPlatform();
    }
    for (size_t i = 0; i < platforms.size(); i++) {
        platforms[i].draw_platform_random();
    }
}

