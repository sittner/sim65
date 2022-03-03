/*
  mos6551.h
  Copyright 2003 William Sheldon Simms III

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

#ifndef INCLUDED_MOS6551_H
#define INCLUDED_MOS6551_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned char read_mos6551_data (unsigned short dummy);
unsigned char read_mos6551_status (unsigned short dummy);
unsigned char read_mos6551_command (unsigned short dummy);
unsigned char read_mos6551_control (unsigned short dummy);

void write_mos6551_data (unsigned short dummy, unsigned char byte);
void write_mos6551_status (unsigned short dummy, unsigned char byte);
void write_mos6551_command (unsigned short dummy, unsigned char byte);
void write_mos6551_control (unsigned short dummy, unsigned char byte);

void clock_mos6551 (void);
void update_mos6551_time (unsigned long nanoseconds);

void set_mos6551_bit_listener (void (*p)(unsigned long long, int));
void set_mos6551_byte_listener (void (*p)(unsigned char));

#ifdef __cplusplus
}
#endif

#endif // INCLUDED_MOS6551_H
