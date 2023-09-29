#ifndef BSTITT_H
#define BSTITT_H

#include <cstdlib> // for rand
#include <GL/gl.h> // Assuming OpenGL header

extern void display_border(int xres, int yres);

class Platform {
public:
    float pos[2];
    float width;
    float height;

    Platform();
    void draw_platform_fixed(float x, float y);
    void draw_platform_random(); // Declare draw_platform_random as a member function
};

#endif
