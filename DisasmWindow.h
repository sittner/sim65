//
// DisasmWindow.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_DISASMWINDOW_H
#define INCLUDED_DISASMINDOW_H

#include "wx/wx.h"
#include "Memory.h"
#include "MemoryListener.h"
#include "WordRegisterListener.h"
#include "BreakpointManager.h"

class DisasmWindow: public wxScrolledWindow,
		    public MemoryListener,
		    public WordRegisterListener,
		    public BreakpointListener
{
public:
  DisasmWindow (wxWindow * parent, Memory * nmem, BreakpointManager * nbpm);

  void OnPaint (wxPaintEvent& event);
  void OnMouseLeftDown (wxMouseEvent& event);

  void ForceRefresh (void);

  void TellNewValue (unsigned short w);
  void TellNewValue (unsigned short addr, unsigned char byte);

  void TellAddedBreakpoint (Breakpoint * bp);
  void TellDeletedBreakpoint (Breakpoint * bp);
  void TellEnabledBreakpoint (Breakpoint * bp);
  void TellDisabledBreakpoint (Breakpoint * bp);

private:
  Memory * mem;
  BreakpointManager * bpm;

  wxBitmap * bkpt_enabled;
  wxBitmap * bkpt_disabled;

  int bkpt_bmp_width;
  int bkpt_bmp_height;

  int bkpt_bmp_xoffs;
  int bkpt_bmp_yoffs;

  int cwidth;
  int cheight;

  int firstaddr;
  int lastaddr;

  int addridx;
  unsigned short addrvec[65536];

  bool running;
  bool initial;
  bool keep_pc_in_window;

  int num_lines_backward (int addr); // should be called before num_lines_forward()
  int num_lines_forward (int addr);  // should be called after num_lines_backward()

  void disassemble (unsigned short addr, char * buf);

  DECLARE_EVENT_TABLE()
};

#endif // INCLUDED_DISASMWINDOW_H
