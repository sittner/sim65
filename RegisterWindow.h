//
// RegisterWindow.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_REGISTERWINDOW_H
#define INCLUDED_REGISTERWINDOW_H

#include "wx/wx.h"
#include "ByteRegister.h"
#include "ByteRegisterListener.h"
#include "WordRegister.h"
#include "WordRegisterListener.h"

class ByteWindow;
class WordWindow;

class ByteRegisterWindow: public wxBoxSizer, public ByteRegisterListener
{
public:
  ByteRegisterWindow (wxWindow * parent, const char * label, ByteRegister * ptr);
  void Constrain (unsigned char sbits, unsigned char cbits);
  void Refresh ();

  void TellNewValue (unsigned char b);

private:
  ByteWindow * bwin;
};

class WordRegisterWindow: public wxBoxSizer, public WordRegisterListener
{
public:
  WordRegisterWindow (wxWindow * parent, const char * label, WordRegister * ptr);
  void Constrain (unsigned short sbits, unsigned short cbits);
  void Refresh ();

  void TellNewValue (unsigned short b);

private:
  WordWindow * wwin;
};

#endif // INCLUDED_REGISTERWINDOW_H
