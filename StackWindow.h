//
// StackWindow.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_STACKWINDOW_H
#define INCLUDED_STACKWINDOW_H

#include "wx/wx.h"
#include "MemoryListener.h"
#include "ByteRegisterListener.h"

class StackWindow: public wxScrolledWindow, public MemoryListener, public ByteRegisterListener
{
public:
  StackWindow (wxWindow * parent);
  void SetRunning (bool isRunning);
  void OnPaint (wxPaintEvent& event);
  void ForceRefresh (void);

  void TellNewValue (unsigned char b);
  void TellNewValue (unsigned short addr, unsigned char byte);

private:
  int cwidth;
  int cheight;

  unsigned short firstaddr;
  unsigned short lastaddr;

  bool running;
  bool initial;

  DECLARE_EVENT_TABLE()
};

#endif // INCLUDED_STACKWINDOW_H
