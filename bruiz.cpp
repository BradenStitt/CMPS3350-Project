// Brian Ruiz Showcase File
// Created 9/25/23
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "bruiz.h"
#include "log.h"
#include "global.h"

using namespace std;

extern Enemy enemy;

// Time since last key was pressed function
int time_since_key_press(const bool get)
{
    static int firstTime = 1;
    static int lastKeyPressTime;
    if (firstTime)
    {
        firstTime = 0;
        lastKeyPressTime = time(NULL);
    }
    if (get)
    {
        return time(NULL) - lastKeyPressTime;
    }
    lastKeyPressTime = time(NULL);
    return 0;
}

// Adding enemies to the game
Enemy::Enemy()
{
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    vel[0] = 0.0f;
    vel[1] = 0.0f;

    width = height = 15.0f;

    verts[0][0] = -15.0f;
    verts[0][1] = 0.0f;
    verts[1][0] = -15.0f;
    verts[1][1] = 30.0f;
    verts[2][0] = 15.0f;
    verts[2][1] = 30.0f;
    verts[3][0] = 15.0f;
    verts[3][1] = 0.0f;
}

Enemy::~Enemy()
{
}

// Update the enemy's position and velocity.
void Enemy::enemyPhysics()
{
    enemy.pos[1] -= 5.0f;
}

void Enemy::drawEnemy()
{
    glPushMatrix();
    glColor3ub(255, 0, 0);

    glTranslatef(pos[0], pos[1], 0.0f);
    // glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(verts[0][0], verts[0][1]); // Bottom left vertex
    glVertex2f(verts[1][0], verts[1][1]); // Top left vertex
    glVertex2f(verts[2][0], verts[2][1]); // Top right vertex
    glVertex2f(verts[3][0], verts[3][1]); // Bottom right vertex
    glEnd();

    glPopMatrix();
}
// Adding Sprites