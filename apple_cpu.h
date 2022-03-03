/*
  apple_cpu.h
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

#ifndef INCLUDED_APPLE_CPU_H
#define INCLUDED_APPLE_CPU_H

extern int dont_execute;
extern int i_delay;

extern unsigned short video_address;
extern long flash;

#define IRQ_VECTOR 0xfffe
#define NMI_VECTOR 0xfffa
#define RES_VECTOR 0xfffc

typedef struct cpu_state cpu_state_t;
struct cpu_state
{
  unsigned char * A;
  unsigned char * X;
  unsigned char * Y;
  unsigned char * S;
  unsigned char * C;
  unsigned char * Z;
  unsigned char * I;
  unsigned char * D;
  unsigned char * B;
  unsigned char * V;
  unsigned char * N;
  unsigned char * irq;
  unsigned short * PC;
};

extern cpu_state_t cpu_ptrs;

void execute (void);

#endif /* #ifndef INCLUDED_APPLE_CPU_H */
