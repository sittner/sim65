/*
  tables.h
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

#ifndef INCLUDED_TABLES_H
#define INCLUDED_TABLES_H

typedef void (* ifunc) (void);

/* instruction decode tables */
extern ifunc    binary_instruction_table[];
extern ifunc    decimal_instruction_table[];
extern ifunc *  instruction_table;

/* debugger/disassembler tables */
extern char     instruction_size[];
extern char     instruction_cycles[];
extern char     instruction_is_branch[];
extern char     instruction_is_immediate[];

extern const char *   standard_instruction_strings[];
extern const char *   orthogonal_instruction_strings[];
extern const char **  instruction_strings;

#endif /* #ifndef INCLUDED_TABLES_H */
