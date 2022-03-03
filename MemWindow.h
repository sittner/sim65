//
// MemWindow.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_MEMWINDOW_H
#define INCLUDED_MEMWINDOW_H

#include "wx/wx.h"
#include "Memory.h"

class MemWindow: public wxScrolledWindow
{
public:
  MemWindow (wxWindow * parent, Memory * mem);
  void SetRunning (bool isRunning);
  void OnChar (wxKeyEvent& event);
  void OnPaint (wxPaintEvent& event);
  void OnKeyDown (wxKeyEvent& event);
  void OnSetFocus (wxFocusEvent &);
  void OnKillFocus (wxFocusEvent &);
  void OnMouseLeftDown (wxMouseEvent& event);
  void ForceRefresh (void);

private:
  int cwidth;
  int cheight;
  int cx, cy;

  bool running;
  bool adjust_view;

  int caddr;
  unsigned short memaddr;

  int alter;

  Memory * memory;

  void scrollto (int addr);

  void change_caddr_noscroll (unsigned short naddr);
  void change_caddr_scrollup (unsigned short naddr);
  void change_caddr_scrolldn (unsigned short naddr, unsigned short lastaddr);

  void up_arrow ();
  void down_arrow ();
  void left_arrow ();
  void right_arrow ();

  void select_byte (int addr, int cx, int cy, int onoff);
  int coord_to_address (int cx, int cy);

  bool address_in_window (int addr);
  bool address_to_coord (int addr, int * cx, int * cy);

  DECLARE_EVENT_TABLE()
};

#endif // INCLUDED_MEMWINDOW_H
