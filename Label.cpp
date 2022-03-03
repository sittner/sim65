//
// Label.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cstring>
#include "Label.h"
#include "Prefs.h"

BEGIN_EVENT_TABLE(Label, wxWindow)
EVT_PAINT(Label::OnPaint)
END_EVENT_TABLE()

Label::Label (wxWindow * parent, const char * ltxt, wxFont * font)
  : wxWindow(parent, -1)
{
  text = ltxt;
  SetThemeEnabled(TRUE);

  Prefs * prefs = Prefs::GetPrefs();

  if (font == 0)
    SetFont(wxFont(prefs->LabelFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  else
    SetFont(*font);

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

  int w, h, d;
  dc.GetTextExtent(wxString::FromAscii(ltxt), &w, &h, &d);
  wxSize csz;
  csz.Set(w, h+d);
  SetClientSize(csz);
  SetMinClientSize(csz);
}

void Label::OnPaint (wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  dc.SetFont(GetFont());
  dc.DrawText(wxString::FromAscii(text), 0, 0);
}
