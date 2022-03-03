//
// Breakpoint.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_BREAKPOINT_H
#define INCLUDED_BREAKPOINT_H

typedef unsigned int bpflags_t;

static const bpflags_t bp_A         = 0x1;
static const bpflags_t bp_X         = 0x2;
static const bpflags_t bp_Y         = 0x4;
static const bpflags_t bp_S         = 0x8;
static const bpflags_t bp_P         = 0x10;
static const bpflags_t bp_PC        = 0x20;
static const bpflags_t bp_mem       = 0x40;
static const bpflags_t bp_operand   = 0xff;
static const bpflags_t bp_equal     = 0x100;
static const bpflags_t bp_notequal  = 0x200;
static const bpflags_t bp_less      = 0x400;
static const bpflags_t bp_greater   = 0x800;
static const bpflags_t bp_condition = 0xf00;
static const bpflags_t bp_automatic = 0x8000;

class Breakpoint
{
public:
  Breakpoint ();
  Breakpoint (unsigned short addr);
  Breakpoint (unsigned short addr, unsigned char byte);
  Breakpoint (bpflags_t flags, unsigned char byte);
  Breakpoint (bpflags_t flags, unsigned short addr, unsigned char byte);

  void Enable () { enabled = true; }
  void Disable () { enabled = false; }
  bool IsEnabled () { return enabled; }
  bool IsAutomatic() { return (flags & bp_automatic) ? true : false; }

  unsigned short GetValue () { return bval; }
  unsigned short GetAddress () { return wval; }

  bpflags_t GetOperandFlags () { return flags & bp_operand; }
  bpflags_t GetConditionFlags () { return flags & bp_condition; }

  bool Equals (bpflags_t flags, unsigned short addr, unsigned char byte);

private:
  bool enabled;
  
  unsigned char bval;
  unsigned short wval;

  bpflags_t flags;
};

#endif // INCLUDED_BREAKPOINT_H
