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
        GLXContext glc;
        Display *dis;
        Window win;
        XVisualInfo *vi;
        XSetWindowAttributes swa;
        int count;

        Background();
        ~Background();
        GLXContext create_display(Display *dis, Window root);
        Window create_window(Display *dis, Window root, int w, int h);
        void background_display();
        int count_physics_function(int count);

};
#endif