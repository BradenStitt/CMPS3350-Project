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

// Time since last key was pressed function
int time_since_key_press(const bool get) {
    static int firstTime = 1;
    static int lastKeyPressTime;
    if (firstTime) {
        firstTime = 0;
        lastKeyPressTime = time(NULL);
    }
    if (get) {
        return time(NULL) - lastKeyPressTime;
    }
    lastKeyPressTime = time(NULL);
    return 0;
}

// Adding enemies to the game