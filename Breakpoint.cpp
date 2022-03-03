//
// Breakpoint.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include "Breakpoint.h"

Breakpoint::Breakpoint ()
{
  flags = 0;
}

Breakpoint::Breakpoint (unsigned short addr)
{
  this->wval = addr;
  this->flags = bp_PC | bp_equal;
  this->enabled = true;
}

Breakpoint::Breakpoint (unsigned short addr, unsigned char byte)
{
  this->wval = addr;
  this->flags = bp_mem | bp_equal;
  this->enabled = true;
}

Breakpoint::Breakpoint (bpflags_t flags, unsigned char byte)
{
  this->bval = byte;
  this->flags = flags;
  this->enabled = true;
}

Breakpoint::Breakpoint (bpflags_t flags, unsigned short addr, unsigned char byte)
{
  this->wval = addr;
  this->flags = flags;
  this->enabled = true;
}

bool Breakpoint::Equals (bpflags_t flags, unsigned short addr, unsigned char byte)
{
  if ((this->flags & bp_PC) && (flags & bp_PC))
    if (this->wval == addr)
      return true;

  if ((this->flags & bp_mem) && (flags & bp_mem))
    if (this->wval == addr && this->bval == byte)
      return true;

  if ((this->flags == flags) && this->bval == byte)
    return true;

  return false;
}

// end
