/*
  disasm.c
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

#include <stdio.h>
#include <string.h>
#include "instructions.h"
#include "tables.h"

#ifdef OPENAPPLE
#include "iie_memory.h"
#else
#include "AddressPeripheral.h"
#endif

void disasm (char * buf, unsigned short addr, int maxbytes, int trace)
{
  unsigned char op;
  int goodsize;
  int isize;
  int len;

  buf[0] = 0;

  op = READ(addr);
  isize = instruction_size[op];

  if (trace)
    {
      sprintf(buf, "A=%2.2x X=%2.2x Y=%2.2x P=%2.2x S=%2.2x    ", A, X, Y, build_P(), S);
    }

  len = strlen(buf);

  goodsize = isize;
  if ((maxbytes > 0) && (isize > maxbytes))
    goodsize = 0;

  switch (goodsize)
    {
    case 1:
      sprintf(buf+len, "%4.4x :   %2.2x         %s", addr, op, standard_instruction_strings[op]);
      break;

    case 2:
      if (instruction_is_branch[op])
	{
	  unsigned short target;
	  target = (unsigned short)((int)(addr+2) + (signed char)READ(addr+1));
	  len += sprintf(buf+len, "%4.4x :   %2.2x %2.2x      ", addr, op, READ(addr+1));
	  sprintf(buf+len, standard_instruction_strings[op], target, (signed char)READ(addr+1));
	}
      else
	{
	  len += sprintf(buf+len, "%4.4x :   %2.2x %2.2x      ", addr, op, READ(addr+1));
	  sprintf(buf+len, standard_instruction_strings[op], READ(addr+1));
	}
      break;

    case 3:
      len += sprintf(buf+len, "%4.4x :   %2.2x %2.2x %2.2x   ", addr, op, READ(addr+1), READ(addr+2));
      sprintf(buf+len, standard_instruction_strings[op], READ(addr+1) + (256 * READ(addr+2)));
      break;

    default:
      if (isize > 0)
	sprintf(buf+len, "%4.4x :   %2.2x         hex  $%2.2x", addr, op, op);
      else
	sprintf(buf+len, "%4.4x :   %2.2x         ???", addr, op);
      break;
    }
}

