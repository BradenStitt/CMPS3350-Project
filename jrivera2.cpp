/**
 * Author:    Joseph Rivera
 * Created:   09-25-2023
 * 
 **/

#include <iostream>
#include <cmath>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <vector>
#include "log.h"
#include "bstitt.h"
#include "jrivera2.h"
#include "global.h"

using namespace std;

int physics_count = 0;

//StartMenu menu;

//Background background;

Background::Background(const char *back) {
if (back[0] == '\0')
		return;
	char name[40];
	strcpy(name, back);
	int slen = strlen(name);
	name[slen-4] = '\0';
	char ppmname[80];
	sprintf(ppmname,"%s.ppm", name);
	char ts[100];
	sprintf(ts, "convert %s %s", back, ppmname);
	system(ts);
	FILE *fpi = fopen(ppmname, "r");
	if (fpi) {
		char line[200];
		fgets(line, 200, fpi);
		fgets(line, 200, fpi);
		//skip comments and blank lines
		while (line[0] == '#' || strlen(line) < 2)
			fgets(line, 200, fpi);
		sscanf(line, "%i %i", &width, &height);
		fgets(line, 200, fpi);
		//get pixel data
		int n = width * height * 3;			
		data = new unsigned char[n];			
		for (int i=0; i<n; i++)
			data[i] = fgetc(fpi);
		fclose(fpi);
	} else {
		printf("ERROR opening image: %s\n", ppmname);
		exit(0);
	}
	unlink(ppmname);
}
Background::~Background() {
	delete [] data;
}


// StartMenu::StartMenu(Display* display, Window window) {
//     dpy = display;
//     win = window;
    
// }

// StartMenu::~StartMenu() {
    
// }

// void StartMenu::show() {

// }

// int StartMenu::handleInput(XEvent *e) {
    
//     return 0; 
// }


int count_physics_function()
{
	physics_count++;
	return physics_count;
}