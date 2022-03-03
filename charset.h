/*
  charset.h
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

#ifndef INCLUDED_CHARSET_H
#define INCLUDED_CHARSET_H

#define NUM_CHARS   256
#define CHAR_WIDTH  7
#define CHAR_HEIGHT 8

#define GET_CHAR_BITS(c) (character_bits + ((c) * CHAR_HEIGHT))

extern unsigned short wide_character_bits [NUM_CHARS * CHAR_HEIGHT];
extern unsigned char character_bits [NUM_CHARS * CHAR_HEIGHT];

void init_character_bits (void);

#endif /* INCLUDED_CHARSET_H */
