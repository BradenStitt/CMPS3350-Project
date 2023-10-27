// Brian Ruiz Showcase File
// Created 9/25/23
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
#include "bruiz.h"
#include "log.h"
#include "global.h"

using namespace std;


// Adding enemies to the game


// Since last key press 
XEvent e;
int time_since_key_press (const bool get)
{
    static int firstTime = 1; 
    static int startTime;
    if (firstTime){
        startTime = time(NULL);
        firstTime = 0; 
    }
    if (get) {
        if (e.type == KeyPress) {
            startTime = time(NULL);
        }
        return time (NULL) - startTime;
    }
    return 0; 
}