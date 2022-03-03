//
// BreakpointManager.h
// Copyright © 2003 William Sheldon Simms III
//

#include <cstdlib>
#include "BreakpointManager.h"

BreakpointManager::BreakpointManager ()
{
  num_breakpoints = 0;
  num_bp_listeners = 0;
}

void BreakpointManager::DeleteAll ()
{
  num_breakpoints = 0;
  for (int idx = 0; idx < num_bp_listeners; ++idx)
    bp_listeners[idx]->TellDeletedBreakpoint(0);
}

void BreakpointManager::EnableAll ()
{
  for (int idx = 0; idx < num_breakpoints; ++idx)
    breakpoints[idx].Enable();

  for (int idx = 0; idx < num_bp_listeners; ++idx)
    bp_listeners[idx]->TellEnabledBreakpoint(0);
}

void BreakpointManager::DisableAll ()
{
  for (int idx = 0; idx < num_breakpoints; ++idx)
    breakpoints[idx].Disable();

  for (int idx = 0; idx < num_bp_listeners; ++idx)
    bp_listeners[idx]->TellDisabledBreakpoint(0);
}

bool BreakpointManager::AddBreakpoint (Breakpoint bp)
{
  if (num_breakpoints >= MAX_NUM_BREAKPOINTS)
    return false;

  breakpoints[num_breakpoints] = bp;

  for (int idx = 0; idx < num_bp_listeners; ++idx)
    bp_listeners[idx]->TellAddedBreakpoint(breakpoints + num_breakpoints);

  ++num_breakpoints;
  return true;
}

bool BreakpointManager::DeleteBreakpoint (Breakpoint * bpp)
{
  if (bpp == NULL) return false;

  --num_breakpoints;

  for (int idx = 0; idx < num_bp_listeners; ++idx)
    bp_listeners[idx]->TellDeletedBreakpoint(bpp);

  for (int idx = bpp - breakpoints; idx < num_breakpoints; ++idx)
    breakpoints[idx] = breakpoints[idx+1];

  return true;
}

int BreakpointManager::NumBreakpoints ()
{
  return num_breakpoints;
}

void BreakpointManager::EnableBreakpointAtIndex (int idx)
{
  Breakpoint * bpp = GetBreakpointAtIndex(idx);
  if (bpp == NULL) return;

  bpp->Enable();

  for (int idx = 0; idx < num_bp_listeners; ++idx)
    bp_listeners[idx]->TellEnabledBreakpoint(bpp);
}

void BreakpointManager::DisableBreakpointAtIndex (int idx)
{
  Breakpoint * bpp = GetBreakpointAtIndex(idx);
  if (bpp == NULL) return;

  bpp->Disable();

  for (int idx = 0; idx < num_bp_listeners; ++idx)
    bp_listeners[idx]->TellDisabledBreakpoint(bpp);
}

void BreakpointManager::DeleteBreakpointAtIndex (int idx)
{
  Breakpoint * bpp = GetBreakpointAtIndex(idx);
  if (bpp == NULL) return;

  --num_breakpoints;

  for (int i = 0; i < num_bp_listeners; ++i)
    bp_listeners[i]->TellDeletedBreakpoint(bpp);

  while (idx < num_breakpoints)
    {
      breakpoints[idx] = breakpoints[idx+1];
      ++idx;
    }

  return;
}

Breakpoint * BreakpointManager::GetBreakpointAtIndex (int idx)
{
  if (idx >= num_breakpoints)
    return NULL;

  return breakpoints + idx;
}

Breakpoint * BreakpointManager::GetBreakpoint (unsigned short addr)
{
  for (int idx = 0; idx < num_breakpoints; ++idx)
    {
      if (breakpoints[idx].Equals(bp_PC, addr, 0))
	return breakpoints + idx;
    }

  return NULL;
}

Breakpoint * BreakpointManager::GetBreakpoint (unsigned short addr, unsigned char byte)
{
  for (int idx = 0; idx < num_breakpoints; ++idx)
    {
      if (breakpoints[idx].Equals(bp_mem, addr, byte))
	return breakpoints + idx;
    }

  return NULL;
}

Breakpoint * BreakpointManager::GetBreakpoint (bpflags_t flags, unsigned char byte)
{
  for (int idx = 0; idx < num_breakpoints; ++idx)
    {
      if (breakpoints[idx].Equals(flags, 0, byte))
	return breakpoints + idx;
    }

  return NULL;
}

Breakpoint * BreakpointManager::GetBreakpoint (bpflags_t flags, unsigned short addr, unsigned char byte)
{
  for (int idx = 0; idx < num_breakpoints; ++idx)
    {
      if (breakpoints[idx].Equals(flags, addr, byte))
	return breakpoints + idx;
    }

  return NULL;
}

bool BreakpointManager::AddBreakpointListener (BreakpointListener * new_bp_listener)
{
  if (num_bp_listeners >= MAX_NUM_BP_LISTENERS)
    return false;

  bp_listeners[num_bp_listeners++] = new_bp_listener;
  return true;
}

// end
