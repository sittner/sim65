/*
  iie_memory.h
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

#ifndef INCLUDED_IIE_MEMORY_H
#define INCLUDED_IIE_MEMORY_H

#define READ(a) (r_page[(a)/256]((a)))
#define WRITE(a,b) (w_page[(a)/256]((a),(b)))

#define READ_ZP(a)    (r_memory[0][(a)])
#define READ_STACK(a) (r_memory[1][(a)])

#define WRITE_ZP(a,b)    (w_memory[0][(a)] = (b))
#define WRITE_STACK(a,b) (w_memory[1][(a)] = (b))

/*
** pointer to function types for reading from and writing to Apple II memory.
** (see r_page[] and w_page[] below)
*/

typedef unsigned char  (* r_func_t) (unsigned short);
typedef void           (* w_func_t) (unsigned short, unsigned char);

/*
** All Apple II memory access is directed through these memory arrays. Every
** pointer in r_memory[] and w_memory[] points to a 256 byte 'page' of memory
** in the 64k address space. Bank switching can change the pointers in r_memory[]
** and w_memory[] allowing more than 64k physical memory to be accessed.
*/

extern unsigned char * aux_memory [256];
extern unsigned char * std_memory [256];

extern unsigned char * r_memory [256];
extern unsigned char * w_memory [256];

extern unsigned char * aux_bank0;

/*
** Just as the memory[] array (above) allows the physical memory addressed to
** changed by bank switching, the r_page[] and w_page[] arrays allow
** the effects of writing and reading various addresses to be changed.
** For example, what happens when video memory is written
** depends on what the current video mode is. Putting pointers to the appropriate
** function into the appropriate slots of w_page[] implements the current
** video mode
*/

extern r_func_t  r_page [];
extern w_func_t  w_page [];

/*
** Direct pointers to hgr/dhgr memory to allow fast access by functions that
** require it: plot_hgr...(), plot_dhgr...(), etc.
*/

#if 0
extern unsigned char *  hgr1_stdmem;
extern unsigned char *  hgr2_stdmem;
extern unsigned char *  hgr1_auxmem;
extern unsigned char *  hgr2_auxmem;
#endif

/*   */

extern unsigned char  switch_auxzp;
extern unsigned char  switch_auxread;
extern unsigned char  switch_auxwrite;
extern unsigned char  switch_page2_aux_text;
extern unsigned char  switch_page2_aux_hires;
extern unsigned char  switch_intc8rom;
extern unsigned char  switch_slotc3rom;
extern unsigned char  switch_slotcxrom;

/*********/

extern unsigned char  aux_text;
extern unsigned char  aux_hires;

extern unsigned char  speaker_state;

/* prototypes for standard page_read/page_write functions */

unsigned char  r_std (unsigned short  r_addr);
unsigned char  r_rom (unsigned short  r_addr);
unsigned char  r_nop (unsigned short  r_addr);

void  w_std (unsigned short  w_addr,
	     unsigned char   w_byte);
void  w_nop (unsigned short  w_addr,
	     unsigned char   w_byte);


void  slotcxrom_on  (void);
void  slotcxrom_off (void);

void  slotc3rom_on  (void);
void  slotc3rom_off (void);

void  auxzp_on  (void);
void  auxzp_off (void);

void  auxread_on   (void);
void  auxread_off  (void);
void  auxwrite_on  (void);
void  auxwrite_off (void);

void  switch_80store_on  (void);
void  switch_80store_off (void);

void  switch_page2_on  (void);
void  switch_page2_off (void);

void  switch_hires_on  (void);
void  switch_hires_off (void);

void  init_memory (void);

#endif /* INCLUDED_IIE_MEMORY_H */
