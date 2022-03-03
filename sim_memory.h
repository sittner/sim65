/*
  generic_memory.h
  Copyright 2000,2001,2002 William Sheldon Simms III

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

#ifndef INCLUDED_GENERIC_MEMORY_H
#define INCLUDED_GENERIC_MEMORY_H

#define READ(a)        (r_func[(a)]((a)))
#define READ_ZP(a)     (r_func[(a)]((a)))
#define READ_STACK(a)  (r_func[(0x100 + a)]((0x100 + a)))

#define WRITE(a,b)       (w_func[(a)]((a),(b)))
#define WRITE_ZP(a,b)    (w_func[(a)]((a),(b)))
#define WRITE_STACK(a,b) (w_func[(0x100 + a)]((0x100 + a),(b)))

#ifdef __cplusplus
extern "C" {
#endif

/*
** pointer to function types for reading from and writing to memory.
** (see r_page[] and w_page[] below)
*/

typedef unsigned char (* r_func_t) (unsigned short);
typedef void          (* w_func_t) (unsigned short, unsigned char);

extern r_func_t r_func [];
extern w_func_t w_func [];

/* prototypes */

int address_is_rom (unsigned short address);

int address_is_readable (unsigned short address);

void init_ram_memory_read (unsigned short cpu_address_begin,
			   unsigned short cpu_address_end,
			   unsigned short mem_address);

void init_ram_memory_write (unsigned short cpu_address_begin,
			    unsigned short cpu_address_end,
			    unsigned short mem_address);

void init_rom_memory (unsigned short cpu_address_begin,
		      unsigned short cpu_address_end,
		      unsigned short mem_address);

void load_rom (unsigned short address, unsigned char byte);

#ifdef __cplusplus
}
#endif

#endif /*  INCLUDED_GENERIC_MEMORY_H */
