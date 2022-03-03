//
// BreakpointListener.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_BREAKPOINTLISTENER_H
#define INCLUDED_BREAKPOINTLISTENER_H

#include "Breakpoint.h"

class BreakpointListener
{
public:
  virtual void TellAddedBreakpoint(Breakpoint * bp) = 0;
  virtual void TellDeletedBreakpoint(Breakpoint * bp) = 0;
  virtual void TellEnabledBreakpoint(Breakpoint * bp) = 0;
  virtual void TellDisabledBreakpoint(Breakpoint * bp) = 0;
};

#endif // INCLUDED_BREAKPOINTLISTENER_H
