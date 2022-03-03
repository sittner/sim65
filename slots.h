/*
  slots.h
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

#ifndef INCLUDED_SLOTS_H
#define INCLUDED_SLOTS_H

#include "apple_cpu.h"  /* cpu_state_t */
#include "iie_memory.h" /* read_func_t, write_func_t */

enum
{
  SLOT_PEEK = 1,
  SLOT_CALL = 2,
  SLOT_TIME = 4
};

enum
{
  SLOT_0, SLOT_1, SLOT_2, SLOT_3, SLOT_4, SLOT_5, SLOT_6, SLOT_7,
  NUM_CARDSLOTS
};

typedef void (* init_func_t) (unsigned short,
                              cpu_state_t *,
                              r_func_t *,
                              w_func_t *);

typedef void (* info_func_t) (unsigned char *);

typedef struct card      card_t;
typedef struct aux_card  aux_card_t;

struct card
{
  init_func_t   init;
  info_func_t   info;
  r_func_t      read;
  w_func_t      write;

  void (* peek) (void);
  void (* call) (void);
  void (* time) (void);
};

struct aux_card
{
  int (* init) (unsigned char **);
  r_func_t  read;
  w_func_t  write;
};

/* The expansion bus */
extern card_t  slot [NUM_CARDSLOTS];

/* Auxiliary slot */
extern aux_card_t  aux_slot;

/* slot stuff */
extern int  slot_peeks[NUM_CARDSLOTS];
extern int  last_slot_peek;

extern int  slot_calls[NUM_CARDSLOTS];
extern int  last_slot_call;

extern int  slot_times[NUM_CARDSLOTS];
extern int  last_slot_time;

#endif /* INCLUDED_SLOTS_H */
