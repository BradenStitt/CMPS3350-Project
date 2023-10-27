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
// #include "fonts.h"

using namespace std;

extern Global g;

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
}

void Platform::draw_platform_fixed(float x, float y)
{
    // Draw the platform using the specified coordinates
    glPushMatrix();
    if (pType == 1)
    {
        glColor3ub(0, 0, 250);
    }
    else if (pType == 2)
    {
        glColor3ub(250, 0, 0);
    }
    else
    {
        glColor3ub(250, 250, 20);
    }
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
    if (pType == 1)
    {
        glColor3ub(0, 0, 250);
    }
    else if (pType == 2)
    {
        glColor3ub(250, 0, 0);
    }
    else
    {
        glColor3ub(250, 250, 20);
    }
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
    if (pos[1] > 0.0f - height)
    {
        // pos[1] -= 5.0f;

        // Snehal's Test on Mac
        pos[1] -= 2.0f;
    }

    // If the platform is a moving platform, move it side to side
    if (pType == 1)
    {
        // Move the platform side to side, bouncing off edges of the screen
        pos[0] += velocity;

        // Bounce off the right wall
        if (pos[0] + width > g.xres)
        {
            pos[0] = g.xres - width;
            velocity = -velocity; // Change direction
        }

        // Bounce off the left wall
        if (pos[0] - width < 0)
        {
            pos[0] = width;
            velocity = -velocity; // Change direction
        }
    }
    else if (pType == 2)
    {
        // Breaking platform
        if (isLanded == true)
        {
            // If the player is on the platform, break it
            pos[1] = -100.0f;
        }
    }
}

// Game Manager class
GameManager::GameManager(int numPlatforms) : platformCreationTimer(0) {}

void GameManager::createPlatform()
{
    int platformType = rand() % 25; // Random number between 0 and 24

    Platform newPlatform;

    if (platformType == 0)
    {
        // 1 in 25 chance of creating a moving platform
        newPlatform.pType = 1;
    }
    else if (platformType == 1)
    {
        // 1 in 25 chance of creating a breaking platform
        newPlatform.pType = 2;
    }
    else
    {
        // Regular platform
        newPlatform.pType = 0;
    }

    platforms.push_back(newPlatform);
    platformCreationTimer = 0; // Reset the timer
}

void GameManager::updatePhysics()
{
    platformCreationTimer++;
    if (platformCreationTimer >= PLATFORM_CREATION_INTERVAL)
    {
        createPlatform();
        platformCreationTimer = 0; // Reset the timer
    }

    for (auto &platform : platforms)
    {
        platform.physics_platform();
    }
}

void GameManager::render()
{
    platformCreationTimer++;
    if (platformCreationTimer >= 120)
    { // 120 frames ~ 2 seconds (assuming 60 frames per second)
        createPlatform();
    }
    for (size_t i = 0; i < platforms.size(); i++)
    {
        platforms[i].draw_platform_random();
    }
}

int total_running_time(const bool running)
{
    static int first_run = 1;
    static int start_time;
    if (first_run)
    {
        start_time = time(NULL);
        first_run = 0;
    }
    if (running)
    {
        return time(NULL) - start_time;
    }
    return 0;
}

int time_since_mouse_moved(const bool get)
{
    static int first_run = 1;
    static int last_time;
    if (first_run)
    {
        first_run = 0;
        last_time = time(NULL);
    }
    if (get)
    {
        return time(NULL) - last_time;
    }
    last_time = time(NULL);
    return 0;
}