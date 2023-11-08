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
int time_since_key_press(const bool get) {
    static int firstTime = 1;
    static int lastKeyPressTime;
    if (firstTime) {
        firstTime = 0;
        lastKeyPressTime = time(NULL);
    }
    if (get) {
        return time(NULL) - lastKeyPressTime;
    }
    lastKeyPressTime = time(NULL);
    return 0;
}

// Adding enemies to the game
Enemy::Enemy() {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    vel[0] = 0.0f;
    vel[1] = 0.0f;
}

Enemy::~Enemy() {

}

// Update the enemy's position and velocity.
void Enemy::enemyPhysics() {

}

void Enemy::drawEnemy() {
    glPushMatrix();
    glColor3ub(255, 0, 0);

    glTranslatef(pos[0], pos[1], 0.0f);
    // glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(-0.5f, -0.5f); // Bottom-left vertex
    glVertex2f(0.5f, -0.5f);  // Bottom-right vertex
    glVertex2f(0.5f, 0.5f);   // Top-right vertex
    glVertex2f(-0.5f, 0.5f);  // Top-left vertex
    glEnd();

    glPopMatrix();
}
// Adding Sprites