//
// BreakpointWindow.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_BREAKPOINTWINDOW_H
#define INCLUDED_BREAKPOINTWINDOW_H

#include "wx/wx.h"
#include "BreakpointManager.h"

class BreakpointWindow: public wxScrolledWindow, public BreakpointListener
{
public:
  BreakpointWindow (wxWindow * parent, BreakpointManager * nbpm);

  void OnPaint (wxPaintEvent& event);
  void OnMouseLeftDown (wxMouseEvent& event);
  void OnMouseRightDown (wxMouseEvent& event);
  void OnBPGoto (wxCommandEvent& event);
  void OnBPEnable (wxCommandEvent& event);
  void OnBPDisable (wxCommandEvent& event);
  void OnBPDelete (wxCommandEvent& event);

  void TellAddedBreakpoint(Breakpoint * bp);
  void TellDeletedBreakpoint(Breakpoint * bp);
  void TellEnabledBreakpoint(Breakpoint * bp);
  void TellDisabledBreakpoint(Breakpoint * bp);

private:
  int cwidth;
  int cheight;
  int topline;
  int selected;

  bool initial;

  wxMenu * bpMenuEnable;
  wxMenu * bpMenuDisable;

  BreakpointManager * bpm;

  DECLARE_EVENT_TABLE()
};

#endif // INCLUDED_BREAKPOINTWINDOW_H
