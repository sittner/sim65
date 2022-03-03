/*
  hdisk.h
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

#ifndef INCLUDED_HDISK_H
#define INCLUDED_HDISK_H

#include "iie_memory.h"
#include "slots.h"

unsigned char  hdisk_read (unsigned short  r_addr);

void  hdisk_write (unsigned short  w_addr,
                   unsigned char   w_byte);

void  hdisk_peek (void);
void  hdisk_time (void);
void  hdisk_call (void);

void  hdisk_info (unsigned char *  bits);

void  hdisk_init (unsigned short  slot,
                  cpu_state_t *   cpup,
                  r_func_t *      rtab,
                  w_func_t *      wtab);

#endif /* INCLUDED_HDISK_H */
