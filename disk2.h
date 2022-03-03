/*
  disk2.h - Disk II Emulation
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

#ifndef INCLUDED_DISK2_H
#define INCLUDED_DISK2_H

#include "apple_cpu.h"
#include "iie_memory.h"

/* maximum and minimum values reachable by d2_adjust_rpm() */
/* "acceptable" range is approximately from 297.0 to 303.0 */
#define D2_MIN_RPM 290.0
#define D2_MAX_RPM 310.0

typedef struct disk2 disk2_t;

/* read the current motor RPM - standard is 300.0 */
double d2_get_rpm (disk2_t * drive);

/* adjust the current motor RPM */
double d2_adjust_rpm (int direction, disk2_t * drive);

/* load a 140k disk image */
int d2_load_dosorder (int drive_idx, char * filename);

/* interface functions */
unsigned char d2_read (unsigned short addr);
void d2_write (unsigned short addr, unsigned char byte);
void d2_info (unsigned char * bits);
void d2_init (unsigned short slot, cpu_state_t * cpup, r_func_t * rtab, w_func_t * wtab);
void d2_peek (void);
void d2_call (void);
void d2_time (void);

#endif /* INCLUDED_DISK2_H */
