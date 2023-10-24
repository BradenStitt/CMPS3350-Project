/**
 * Author:    Joseph Rivera
 * Created:   09-25-2023
 * 
 **/

#include <cstdlib> // for rand
#include <GL/gl.h> 

extern int count_physics_function();

class Background {
    public:
        Display *dis;
        Window win;
        GLXContext glc;

        Background();
        GLXContext background_display();

    
};