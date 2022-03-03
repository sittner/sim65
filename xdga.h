/*
  xdga.h
  Copyright 2000,2001,2002 by William Sheldon Simms III

  This file is a part of open apple -- a free Apple II emulator.
 
  Open apple is free software; you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software Foundation;
  either version 2, or (at your option) any later version.
 
  Open apple is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License along with open apple;
  see the file COPYING. If not, visit the Free Software Foundation website at
  http://www.fsf.org
*/

#ifndef INCLUDED_XDGA_H
#define INCLUDED_XDGA_H

#include <X11/Xlib.h>
#include <X11/extensions/xf86dga.h>

#ifdef BIGENDIAN
#define BYTE_IDX 3
#define HALFWORD_IDX 1
#else
#define BYTE_IDX 0
#define HALFWORD_IDX 0
#endif

typedef struct
{
  int depth;
  int width;
  int height;
  int bytes_per_line;
  int bytes_per_pixel;
  char * base;

  int event_base;
  int error_base;

  Display * xdisp;
  XDGADevice * device;
  Colormap cmap;

  union
  {
    unsigned char  byte[4];
    unsigned short halfword[2];
    unsigned long  word;
  }
  pixel[16];
}
xdga_info_t;

int xdga_mode (Display * xdisp, int request_w, int request_h, xdga_info_t * info, char ** why);

#endif /* INCLUDED_XDGA_H */
