/**
 * Author:    Joseph Rivera
 * Created:   09-25-2023
 * 
 **/
#ifndef JRIVERA_H
#define JRIVERA_H

#include <cstdlib> // for rand
#include <GL/gl.h> 
#include <X11/Xlib.h>
#include <X11/keysym.h>

class Background {
    public:
        Display *dis;
        Window win;
        GLXContext glc;
        int count;

        Background();
        GLXContext create_display(Display *dis, Window root);
        Window create_window(Display *dis, Window root, int w, int h);
        void background_display();
        int count_physics_function(int count);

};
#endif