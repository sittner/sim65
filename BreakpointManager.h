//
// BreakpointManager.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_BREAKPOINTMANGER_H
#define INCLUDED_BREAKPOINTMANGER_H

#include "Breakpoint.h"
#include "BreakpointListener.h"

#define MAX_NUM_BREAKPOINTS 256
#define MAX_NUM_BP_LISTENERS 4

class BreakpointManager
{
public:
  BreakpointManager();

  int NumBreakpoints ();

  void DeleteAll ();
  void EnableAll ();
  void DisableAll ();

  void DeleteBreakpointAtIndex (int idx);
  void EnableBreakpointAtIndex (int idx);
  void DisableBreakpointAtIndex (int idx);
  Breakpoint * GetBreakpointAtIndex (int idx);

  bool AddBreakpoint (Breakpoint bp);
  bool DeleteBreakpoint (Breakpoint * bpp);

  Breakpoint * GetBreakpoint (unsigned short addr);
  Breakpoint * GetBreakpoint (unsigned short addr, unsigned char byte);
  Breakpoint * GetBreakpoint (bpflags_t flags, unsigned char byte);
  Breakpoint * GetBreakpoint (bpflags_t flags, unsigned short addr, unsigned char byte);

  bool AddBreakpointListener (BreakpointListener * new_bp_listener);

private:
  int num_breakpoints;
  Breakpoint breakpoints[MAX_NUM_BREAKPOINTS];

  /* listeners */
  int num_bp_listeners;
  BreakpointListener * bp_listeners[MAX_NUM_BP_LISTENERS];
};

#endif // INCLUDED_BREAKPOINTMANGER_H
