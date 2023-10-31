#ifndef BSTITT_H
#define BSTITT_H

#include <cstdlib> // for rand
#include <GL/gl.h> // Assuming OpenGL header
#include <vector>

using namespace std;

extern void display_border(int xres, int yres);
extern int time_since_mouse_moved(const bool get);
extern int total_running_time(const bool running);
extern void enemy_collision_detection();

class Platform
{
public:
    int pType; // 0 = fixed, 1 = moving, 2 = breakable, 3 = enemy
    float pos[2];
    float width;
    float height;
    float velocity;
    bool isLanded;

    Platform();
    void draw_platform_fixed(float x, float y);
    void draw_platform_random();
    void physics_platform();
};

class GameManager {
public:
    vector<Platform> platforms;
    int platformCreationTimer;                        // Timer to control platform creation
    static const int PLATFORM_CREATION_INTERVAL = 60; // Adjust as needed (frames)

    GameManager(int numPlatforms);

    void updatePhysics();
    void render();
    void createPlatform(); // Function to create a new platform
};

#endif
