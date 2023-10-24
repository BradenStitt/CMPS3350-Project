/**
 * Author:    Joseph Rivera
 * Created:   09-25-2023
 * 
 **/

#include <cstdlib> // for rand
#include <GL/gl.h> 

class Background {
    public:
        Display *dis;
        Window win;
        GLXContext glc;
        int count;

        Background();
        GLXContext create_display(Display *dis, Window root);
        Window create_window(Display *dis, Window root);
        void background_display();
        int count_physics_function(int count);

}