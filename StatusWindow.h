//
// StatusWindow.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_STATUSWINDOW_H
#define INCLUDED_STATUSWINDOW_H

#include "wx/wx.h"
#include "ByteRegisterListener.h"

#define MAX_NUM_LISTENERS 8

class FlagsWindow;

class StatusWindow: public wxBoxSizer, public ByteRegisterListener
{
public:
  StatusWindow (wxWindow * parent, unsigned char * N, unsigned char * V, unsigned char * B,
	       unsigned char * D, unsigned char * I, unsigned char * Z, unsigned char * C);

  void Refresh ();

  bool AddListener (ByteRegisterListener * new_listener);
  void TellNewValue (unsigned char b);

private:
  FlagsWindow * fw;
};

#endif // INCLUDED_STATUSWINDOW_H
