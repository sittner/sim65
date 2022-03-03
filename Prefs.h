//
// Prefs.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_PREFS_H
#define INCLUDED_PREFS_H

class Prefs
{
public:
  static Prefs * GetPrefs ();

  int BorderWidth() { return border_width; }
  int LabelFontSize () { return label_font_size; }
  int WindowFontSize () { return window_font_size; }

private:
  static Prefs * instance;

  int border_width;
  int label_font_size;
  int window_font_size;

  Prefs();
};

#endif // INCLUDED_PREFS_H
