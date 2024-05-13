#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <curses.h>
#define CARDW_MIN 4
#define CARDH_MIN 3

int initGfx(int h, int w);
void endGfx();

void mvCursorDown();
void mvCursorUp();
void mvCursorRight();
void mvCursorLeft();

void select();
void unselect();

#endif
