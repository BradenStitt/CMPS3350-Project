// Brian Ruiz Header File
// Created 9/27/23
#ifndef BRUIZ_H
#define BRUIZ_H

using namespace std;

extern int time_since_key_press(const bool get);
extern void updateAndPrintScore();

class Enemy {
public:
    float pos[2];
    float vel[2];
    float velocity;
    float verts[4][2];
    float width, height;
    int hitCount;
    bool isDestroyed;

    Enemy();
    ~Enemy();
    void enemyPhysics();
    void drawEnemyFixed(float x, float y);
    void drawEnemy();
};

#endif