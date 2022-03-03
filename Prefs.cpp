//
// Prefs.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cstdlib>
#include "wx/wx.h"
#include "Prefs.h"

Prefs * Prefs::instance = NULL;

Prefs::Prefs ()
{
  border_width = 5;
  label_font_size = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT).GetPointSize();
  window_font_size = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT).GetPointSize();

  label_font_size = (label_font_size + window_font_size) / 2;
  window_font_size = label_font_size;
}

Prefs * Prefs::GetPrefs ()
{
  if (instance == NULL)
    instance = new Prefs();

  return instance;
}

// end
