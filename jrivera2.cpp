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
#include "skumar.h"
#include "jrivera2.h"
#include "global.h"
#include "fonts.h"

using namespace std;

extern Global g;
extern Rect r;
extern Player player;
extern Texture graph, t, soc, hop, halloween;
extern Texture s, p, he, soccer, victory;
extern StartMenu startMenu;
extern int print_score();

int physics_count = 0;
float coord[4][2];
int fixer = 0;
int textColor = 0;
int black = 0x00000000;
int white = 0x00ffffff;
int blue = 0x000000ff;
int green = 0x0055ff55;
int yellow = 0x00ffff00;

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

void StartMenu::showStartScreen(Texture texture) {
    // This function displays the start screen on the game window
	switchColor();
	
	r.center = 0;
	r.bot = g.yres - 20;
	r.left = 10;

	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, texture.tex.backTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(texture.tex.xc[0], texture.tex.yc[1]); glVertex2i(0,      0);
		glTexCoord2f(texture.tex.xc[0], texture.tex.yc[0]); glVertex2i(0,      g.yres);
		glTexCoord2f(texture.tex.xc[1], texture.tex.yc[0]); glVertex2i(g.xres, g.yres);
		glTexCoord2f(texture.tex.xc[1], texture.tex.yc[1]); glVertex2i(g.xres, 0);

	//glColor3f(1.0, 1.0, 1.0);
	

	r.bot -=40;
    ggprint8b(&r, 64, textColor, "");
	r.bot -= 20;
    ggprint16(&r, 24, textColor, "        WELCOME TO SCRIBBLE-HOP!");
	r.bot -= 2;
	ggprint8b(&r, 16, textColor, 
		"                                   Press 'P' to Play");
	r.bot -=250;
	ggprint8b(&r, 16, textColor,                                 
	 	"                                 GAME CONTROLS");
	ggprint8b(&r, 16, textColor, 
	 	"                            Press Up arrow to Jump");
	ggprint8b(&r, 16, textColor, 
		"                Press Left or Right arrows to move player");
	ggprint8b(&r, 16, textColor, 
	 	"                             Press 'Space' to Shoot");
	ggprint8b(&r, 16, textColor, 
		"                            Press 'R' to restart game");
	r.bot -= 60;
	ggprint8b(&r, 16, textColor, 
		"          Created by: Snehal Kumar, Braden Stitt, Joseph Rivera ");
	ggprint8b(&r, 16, textColor, 
		"                                        & Brian Ruiz ");

	glPopMatrix();
    
}

void makeSprite()
{
	coord[0][0] = -15.0f;
	coord[0][1] = 0.0f;
	coord[1][0] = -15.0f;
	coord[1][1] = 30.0f;
	coord[2][0] = 15.0f;
	coord[2][1] = 30.0f;
	coord[3][0] = 15.0f;
	coord[3][1] = 0.0f;

	glBindTexture(GL_TEXTURE_2D, s.tex.backTexture);
	glBegin(GL_QUADS);
		glTexCoord2f(s.tex.xc[0], s.tex.yc[1]); glVertex2f(coord[0][0], coord[0][1]);
		glTexCoord2f(s.tex.xc[0], s.tex.yc[0]); glVertex2f(coord[1][0], coord[1][1]);
		glTexCoord2f(s.tex.xc[1], s.tex.yc[0]); glVertex2f(coord[2][0], coord[2][1]);
		glTexCoord2f(s.tex.xc[1], s.tex.yc[1]); glVertex2f(coord[3][0], coord[3][1]);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void youDied()
{
	r.center = 0;
	r.bot = g.yres - 20;
	r.left = 10;
	glPushMatrix();
	r.bot -= 275;
	ggprint16(&r, 24, 0xFF0000, "                  GAME OVER");
	r.bot -= 10;
	ggprint8b(&r, 16, 0xFF0000, 
	"                               Press 'R' to Restart");

}

void scoreboard()
{
	switchColor();

	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

    glBegin(GL_QUADS);
    glVertex2f(0.0, g.yres);  
    glVertex2f(0.0, g.yres - 40.0);  
    glVertex2f(g.xres, g.yres - 40.0);  
    glVertex2f(g.xres, g.yres);
	glEnd();
    glPopMatrix();


	r.center = 0;
	r.bot = g.yres - 30;
	r.left = 10;
	ggprint16(&r, 24, textColor, " score: %i", print_score() );
	glPopMatrix();

}

void makeTrophy()
{
	glBindTexture(GL_TEXTURE_2D, p.tex.backTexture);
	    glBegin(GL_QUADS);
		glTexCoord2f(p.tex.xc[0], p.tex.yc[1]); glVertex2f(-15, 0);
		glTexCoord2f(p.tex.xc[0], p.tex.yc[0]); glVertex2f(-15, 30);
		glTexCoord2f(p.tex.xc[1], p.tex.yc[0]); glVertex2f(15, 30);
		glTexCoord2f(p.tex.xc[1], p.tex.yc[1]); glVertex2f(15, 0);
	    glEnd();
	    glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
}

void makeHeart(int i)
{
	glBindTexture(GL_TEXTURE_2D, he.tex.backTexture);
		glColor3ub(255, 255, 255);
        glPushMatrix();
        glTranslatef(g.xres - 20 - i * 20, g.yres - 20, 0.0f);
	    glBegin(GL_QUADS);
		glTexCoord2f(he.tex.xc[0], he.tex.yc[1]); glVertex2f(-10, -10);
		glTexCoord2f(he.tex.xc[0], he.tex.yc[0]); glVertex2f(-10, 10);
		glTexCoord2f(he.tex.xc[1], he.tex.yc[0]); glVertex2f(10, 10);
		glTexCoord2f(he.tex.xc[1], he.tex.yc[1]); glVertex2f(10, -10);
	    glEnd();
	    glBindTexture(GL_TEXTURE_2D, 0);
		glEnd();
		glColor3ub(20, 20, 20);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-10, -10);
        glVertex2f(-10, 10);
        glVertex2f(10, 10);
        glVertex2f(10, -10);
        glVertex2f(-10, -10);
        glEnd();
        glPopMatrix();
}

void levelOne(Texture image)
{
	switchColor();

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, image.tex.backTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(image.tex.xc[0], image.tex.yc[1]); glVertex2i(0, 0);
	glTexCoord2f(image.tex.xc[0], image.tex.yc[0]); glVertex2i(0, g.yres);
	glTexCoord2f(image.tex.xc[1], image.tex.yc[0]); glVertex2i(g.xres, g.yres);
	glTexCoord2f(image.tex.xc[1], image.tex.yc[1]); glVertex2i(g.xres, 0);
	r.center = 0;
	r.bot = g.yres - 40;
	if (player.trophyDetected == 1)
		r.left = 15;
	else
		r.left = 10;
	ggprint8b(&r, 16, textColor, "");
	ggprint8b(&r, 16, textColor, " Press 'M' for MENU");
	ggprint8b(&r, 16, textColor, " Press 'S' for STATISTICS");
	ggprint8b(&r, 16, textColor, " Press 'K' for SNEHAL'S FEATURES");
	ggprint8b(&r, 16, textColor, " Press 'L' to lower TROPHY POSITION");
	glEnd();
}

void makeLevels(int increment)
{
	if (player.trophyDetected == 0) {
		levelOne(graph);
		if (increment <= 150) {
			r.bot = 300.0f;
			ggprint8b(&r, 16, 0x00000000, "");
			ggprint16(&r, 24, 0x00000000, "                      LEVEL 1");
		}
	} else if (player.trophyDetected == 1) {
		levelOne(soc);
		r.bot = 300.0f;
		if (increment <= 150) {
			ggprint8b(&r, 16, 0x00000000, "");
			ggprint16(&r, 24, 0x00000000, "                      LEVEL 2");
		}
	} else if (player.trophyDetected == 2) {
		levelOne(t);
		r.bot = 300.0f;
		if (increment <= 150) {
			ggprint8b(&r, 16, 0x00000000, "");
			ggprint16(&r, 24, 0x00000000, "                      LEVEL 3");
		}
	} else if (player.trophyDetected == 3) {
		levelOne(hop);
		r.bot = 300.0f;
		if (increment <= 150) {
			ggprint8b(&r, 16, 0x00000000, "");
			ggprint16(&r, 24, 0x00000000, "                      LEVEL 4");
		}
	} else if (player.trophyDetected == 4) {
		levelOne(halloween);
		r.bot = 300.0f;
		if (increment <= 150) {
			ggprint8b(&r, 16, 0x00000000, "");
			ggprint16(&r, 24, 0x00000000, "                      LEVEL 5");
		}
	} else {
		victoryScreen();
	}

}

void victoryScreen() 
{
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, victory.tex.backTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(victory.tex.xc[0], victory.tex.yc[1]); glVertex2i(0, 0);
	glTexCoord2f(victory.tex.xc[0], victory.tex.yc[0]); glVertex2i(0, g.yres);
	glTexCoord2f(victory.tex.xc[1], victory.tex.yc[0]); glVertex2i(g.xres, g.yres);
	glTexCoord2f(victory.tex.xc[1], victory.tex.yc[1]); glVertex2i(g.xres, 0);
	glEnd();
    glPopMatrix();
	r.center = 0;
	r.bot = g.yres - 420;
	r.left = 10;
	ggprint16(&r, 24, 0xFFFFFFFF, "                 Total Score: %i", print_score() );
	ggprint16(&r, 12, 0x00ffff00, "            Press 'R' to return to Menu" );
	//glEnd();
}

int count_physics_function()
{
	physics_count++;
	return physics_count;
}

void switchColor()
{
	if (player.trophyDetected == 0) 
		textColor = black;
	if (player.trophyDetected == 1)
		textColor = white;
	if (player.trophyDetected == 2)
		textColor = yellow;
	if (player.trophyDetected == 3)
		textColor = blue;
	if (player.trophyDetected == 4)
		textColor = green;
}