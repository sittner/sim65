/*
  apple.h
  Copyright 2000,2001 by William Sheldon Simms III

  This file is a part of open apple -- a free Apple II emulator.
 
  Open apple is free software; you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software Foundation; either
  version 2, or (at your option) any later version.
 
  Open apple is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License along with open apple;
  see the file COPYING. If not, visit the Free Software Foundation website at http://www.fsf.org
*/

#ifndef INCLUDED_APPLE_H
#define INCLUDED_APPLE_H

#include "SDL.h"

/* allow for border */

/* #define USE_SDL */

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

#define SCREEN_WIDTH  560
#define SCREEN_HEIGHT 384

#define SCAN_WIDTH  910
#define SCAN_HEIGHT 524

#define SCANLINE_INCR 2

#define CPU_HZ       1020484  // The Apple II Circuit Description, p. 30
#define SOUND_HZ     22050
#define SOUND_BUFLEN 524288

extern int SCREEN_BORDER_WIDTH;
extern int SCREEN_BORDER_HEIGHT;

extern SDL_Surface * ssurf;
extern int sdl_ulx;
extern int sdl_uly;
extern int sdl_lrx;
extern int sdl_lry;

/* Apple II internal state */

extern unsigned char apple_vbl;
extern unsigned long apple_vbl_time;
extern int MHz;

/* keyboard stuff */

extern unsigned char kb_byte;
extern unsigned char key_pressed;
extern unsigned char key_autorepeat;
extern unsigned char open_apple_pressed;
extern unsigned char solid_apple_pressed;

/* speaker stuff */

extern unsigned char sample_buf[SOUND_BUFLEN];
extern unsigned int sample_idx;

/* user-selectable items */

extern unsigned char option_color_monitor;
extern unsigned char option_full_scan;
extern unsigned char option_limit_speed;

#endif /* INCLUDED_APPLE_H */
