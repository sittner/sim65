//
// Label.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_LABEL_H
#define INCLUDED_LABEL_H

#include "wx/wx.h"

class Label: public wxWindow
{
public:
  Label (wxWindow * parent, const char * ltxt, wxFont * font);
  void OnPaint (wxPaintEvent& event);

private:
  int cwidth;
  int cheight;
  const char * text;

  DECLARE_EVENT_TABLE()
};

#endif // INCLUDED_LABEL_H
