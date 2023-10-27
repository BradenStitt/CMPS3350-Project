/**
 * Author:    Joseph Rivera
 * Created:   09-25-2023
 *
 **/

#include <cstdlib> // for rand
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

extern int count_physics_function();

class Background
{
public:
    int x, y, w, h;
    Display *dis;
    Window win;
    GLXContext glc;
    XVisualInfo *vi;
    XSetWindowAttributes swa;

    Background();
    ~Background();
    GLXContext background_display(Display *dis, Window root);
    Window create_window(Display *dis, Window root, int w, int h);
};