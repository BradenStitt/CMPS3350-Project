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
#include "fonts.h"

using namespace std;

extern Global g;
extern Rect r;
extern Texture t;
extern Texture d;

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

StartMenu::StartMenu() {
    
}

StartMenu::~StartMenu() {
    
}

void StartMenu::showStartScreen() {
    // This function displays the start screen on the game window
	
	r.center = 0;
	r.bot = g.yres - 20;
	r.left = 10;

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, t.tex.backTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(t.tex.xc[0], t.tex.yc[1]); glVertex2i(0,      0);
		glTexCoord2f(t.tex.xc[0], t.tex.yc[0]); glVertex2i(0,      g.yres);
		glTexCoord2f(t.tex.xc[1], t.tex.yc[0]); glVertex2i(g.xres, g.yres);
		glTexCoord2f(t.tex.xc[1], t.tex.yc[1]); glVertex2i(g.xres, 0);

	//glColor3f(1.0, 1.0, 1.0);
	
	r.bot -=50;
    ggprint8b(&r, 64, 0x00000000, "");
	r.bot -= 20;
    ggprint16(&r, 24, 0x00000000, "      WELCOME TO SCRIBBLE-JUMP!");
	r.bot -= 2;
	ggprint8b(&r, 16, 0x00000000, "                                  Click 'P' to Start");
	r.bot -= 370;
	ggprint8b(&r, 16, 0x00000000, "        Created by: Snehal Kumar, Bradon Stitt & Joseph Rivera ");

	
	glPopMatrix();
    
}


int count_physics_function()
{
	physics_count++;
	return physics_count;
}