/**
 * Author:    Joseph Rivera
 * Created:   09-25-2023
 * 
 **/
#ifndef JRIVERA_H
#define JRIVERA_H

#include <cstdlib> // for rand
#include <GL/glx.h> 
#include <X11/Xlib.h>
#include <X11/keysym.h>

using namespace std;

extern int count_physics_function();


class Background {
    public:
        int width, height;
	    unsigned char *data;
        Background(const char *back);
        ~Background();
    
};

class Picture {
public:
	Background *backImage;
	GLuint backTexture;
	float xc[2];
	float yc[2];
};

class Texture {
public:
    Picture tex;
};

// class StartMenu {
// public:
//     StartMenu(Display* display, Window window);
//     ~StartMenu();
    
//     void show();
//     int handleInput(XEvent *e);
    
// private:
//     Display* dpy;
//     Window win;
    
// };
#endif