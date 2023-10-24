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

class Background {
    public:
        int count;

        Background();
        void background_display();
        int count_physics_function(int count);

};
#endif