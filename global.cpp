#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "bstitt.h"
#include <vector>
#include "global.h"

using namespace std;

Global::Global()
{
    xres = 400;
    yres = 600;
    landed = 0;
    failed_landing = 0;
    test_mode = 1;
    showNerdStats = 1;
}
