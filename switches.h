/*
  switches.h
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

#ifndef INCLUDED_SWITCHES_H
#define INCLUDED_SWITCHES_H

extern unsigned char switch_alt_cset;
extern unsigned char switch_graphics;
extern unsigned char switch_mixed;
extern unsigned char switch_hires;
extern unsigned char switch_page2;

extern unsigned char switch_auxzp;
extern unsigned char switch_auxread;
extern unsigned char switch_auxwrite;
extern unsigned char switch_page2_aux_text;
extern unsigned char switch_page2_aux_hires;
extern unsigned char switch_intc8rom;
extern unsigned char switch_slotc3rom;
extern unsigned char switch_slotcxrom;
extern unsigned char switch_80col;
extern unsigned char switch_80store;
extern unsigned char switch_dblhires;
extern unsigned char switch_ioudis;

#endif
