/*
  video.h
  Copyright 2000, 2001, 2002 William Sheldon Simms III

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

#ifndef INCLUDED_VIDEO_H
#define INCLUDED_VIDEO_H

#define NUM_VIDEO_COLS 560
#define NUM_VIDEO_ROWS 192
#define NUM_VIDEO_BITS (NUM_VIDEO_ROWS * NUM_VIDEO_COLS)

#define H_VIDEO_COUNTS 65
#define V_VIDEO_COUNTS 262
#define VIDEO_BITS_PER_COLUMN 14

extern int dirty_line_beg [V_VIDEO_COUNTS];
extern int dirty_line_end [V_VIDEO_COUNTS];
extern unsigned char gr1[192][560];
extern unsigned char gr2[192][560];
extern unsigned char agr1[192][560];
extern unsigned char agr2[192][560];
extern unsigned char dgr1[192][560];
extern unsigned char dgr2[192][560];
extern unsigned char hgr1[192][560];
extern unsigned char hgr2[192][560];
extern unsigned char ahgr1[192][560];
extern unsigned char ahgr2[192][560];
extern unsigned char dhgr1[192][560];
extern unsigned char dhgr2[192][560];
extern unsigned char text1[192][560];
extern unsigned char text2[192][560];
extern unsigned char atext1[192][560];
extern unsigned char atext2[192][560];
extern unsigned char dtext1[192][560];
extern unsigned char dtext2[192][560];
extern const unsigned long apple_color_pixels [16];

/* intialization */
void init_video (void);
void reset_video (void);
void set_scan (void);

/* writes to video memory */
void hgr_p1_write (unsigned short w_addr, unsigned char w_byte);
void hgr_p2_write (unsigned short w_addr, unsigned char w_byte);

void text_p1_write (unsigned short w_addr, unsigned char w_byte);
void text_p2_write (unsigned short w_addr, unsigned char w_byte);

void flash_text (void);

/* set the current video mode based on the video switches */
void set_video_mode (void);

#endif /* INCLUDED_VIDEO_H */
