//
// RegisterWindow.h
// Copyright © 2003 William Sheldon Simms III
//

#include <cstdio>
#include <cstring>
#include "RegisterWindow.h"
#include "Label.h"
#include "ByteRegister.h"
#include "WordRegister.h"
#include "Prefs.h"

#define ALTER_START 0
#define ALTER_ONE_DIGIT 1
#define ALTER_TWO_DIGITS 2
#define ALTER_THREE_DIGITS 3

class ByteWindow: public wxWindow
{
public:
  ByteWindow (wxWindow * parent, int width, ByteRegister * ptr);

  void OnChar (wxKeyEvent& event);
  void OnPaint (wxPaintEvent& event);
  void OnMouseLeftDown (wxMouseEvent& event);
  void OnSetFocus (wxFocusEvent &);
  void OnKillFocus (wxFocusEvent &);
  void Constrain (unsigned char sbits, unsigned char cbits);
  void NewValue (unsigned char b);

private:
  int focus;
  int alter;
  int cwidth;
  int cheight;
  unsigned char setbits;
  unsigned char clearbits;
  ByteRegister * reg;
  DECLARE_EVENT_TABLE()
};

class WordWindow: public wxWindow
{
public:
  WordWindow (wxWindow * parent, int width, WordRegister * ptr);

  void OnChar (wxKeyEvent& event);
  void OnPaint (wxPaintEvent& event);
  void OnMouseLeftDown (wxMouseEvent& event);
  void OnSetFocus (wxFocusEvent &);
  void OnKillFocus (wxFocusEvent &);
  void Constrain (unsigned short sbits, unsigned short cbits);
  void NewValue (unsigned short w);

private:
  int focus;
  int alter;
  int cwidth;
  int cheight;
  unsigned short setbits;
  unsigned short clearbits;
  WordRegister * reg;
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ByteWindow, wxWindow)
EVT_CHAR(ByteWindow::OnChar)
EVT_PAINT(ByteWindow::OnPaint)
EVT_LEFT_DOWN(ByteWindow::OnMouseLeftDown)
EVT_SET_FOCUS(ByteWindow::OnSetFocus)
EVT_KILL_FOCUS(ByteWindow::OnKillFocus)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(WordWindow, wxWindow)
EVT_CHAR(WordWindow::OnChar)
EVT_PAINT(WordWindow::OnPaint)
EVT_LEFT_DOWN(WordWindow::OnMouseLeftDown)
EVT_SET_FOCUS(WordWindow::OnSetFocus)
EVT_KILL_FOCUS(WordWindow::OnKillFocus)
END_EVENT_TABLE()

ByteWindow::ByteWindow (wxWindow * parent, int width, ByteRegister * ptr)
  : wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
  focus = 0;
  reg = ptr;

  setbits = clearbits = 0;

  SetThemeEnabled(FALSE);
  SetBackgroundColour(*wxWHITE);

  Prefs * prefs = Prefs::GetPrefs();
  SetFont(wxFont(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

  wxSize csz;
  csz.Set(width * cwidth, 3 * cheight / 2);
  SetClientSize(csz);
  SetMinClientSize(csz);
}

void ByteWindow::Constrain (unsigned char sbits, unsigned char cbits)
{
  setbits = sbits;
  clearbits = cbits;
}

void ByteWindow::OnMouseLeftDown (wxMouseEvent& event)
{
  this->SetFocus();
}

void ByteWindow::OnSetFocus (wxFocusEvent& evt)
{
  focus = 1;
  alter = ALTER_START;
  Refresh();
}

void ByteWindow::OnKillFocus (wxFocusEvent& evt)
{
  focus = 0;
  Refresh();
}

void ByteWindow::OnChar (wxKeyEvent& event)
{
  int kcode = event.GetKeyCode();

  if (!focus || !isxdigit(kcode))
    {
      event.Skip();
      return;
    }

  if (isdigit(kcode))
    kcode -= '0';
  else
    kcode = toupper(kcode) - 'A' + 10;

  if (alter == ALTER_START)
    {
      alter = ALTER_ONE_DIGIT;
      reg->Write((kcode | setbits) & (~clearbits));
    }
  else
    {
      alter = ALTER_START;
      kcode = 16 * reg->Read() + kcode;
      reg->Write((kcode | setbits) & (~clearbits));
    }

  Refresh();
}

void ByteWindow::OnPaint (wxPaintEvent& WXUNUSED(event))
{
  char foo[8];

  wxPaintDC dc(this);
  dc.SetFont(GetFont());

  sprintf(foo, "%2.2x", reg->Read());

  if (focus)
    {
      dc.SetPen(*wxBLACK_PEN);
      dc.SetBrush(*wxBLACK_BRUSH);
      dc.SetTextForeground(*wxWHITE);
    }
  else
    {
      dc.SetPen(*wxWHITE_PEN);
      dc.SetBrush(*wxWHITE_BRUSH);
      dc.SetTextForeground(*wxBLACK);
    }

  dc.DrawRectangle(cwidth, cheight / 4, 2*cwidth, cheight);
  dc.DrawText(wxString::FromAscii(foo), cwidth, cheight / 4);
  dc.SetTextForeground(*wxBLACK);
}

void ByteWindow::NewValue (unsigned char b)
{
  reg->Write(b);
  Refresh();
}

WordWindow::WordWindow (wxWindow * parent, int width, WordRegister * ptr)
  : wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
  focus = 0;
  reg = ptr;

  setbits = clearbits = 0;

  SetThemeEnabled(FALSE);
  SetBackgroundColour(*wxWHITE);

  Prefs * prefs = Prefs::GetPrefs();
  SetFont(wxFont(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

  wxSize csz;
  csz.Set(width * cwidth, 3 * cheight / 2);
  SetClientSize(csz);
  SetMinClientSize(csz);
}

void WordWindow::Constrain (unsigned short sbits, unsigned short cbits)
{
  setbits = sbits;
  clearbits = cbits;
}

void WordWindow::OnMouseLeftDown (wxMouseEvent& event)
{
  this->SetFocus();
}

void WordWindow::OnSetFocus (wxFocusEvent& evt)
{
  focus = 1;
  alter = ALTER_START;
  Refresh();
}

void WordWindow::OnKillFocus (wxFocusEvent& evt)
{
  focus = 0;
  Refresh();
}

void WordWindow::OnChar (wxKeyEvent& event)
{
  int kcode = event.GetKeyCode();

  if (!focus || !isxdigit(kcode))
    {
      event.Skip();
      return;
    }

  if (isdigit(kcode))
    kcode -= '0';
  else
    kcode = toupper(kcode) - 'A' + 10;

  if (alter == ALTER_START)
    {
      alter = ALTER_ONE_DIGIT;
      reg->Write((kcode | setbits) & (~clearbits));
    }
  else if (alter == ALTER_ONE_DIGIT)
    {
      alter = ALTER_TWO_DIGITS;
      kcode = 16 * reg->Read() + kcode;
      reg->Write((kcode | setbits) & (~clearbits));
    }
  else if (alter == ALTER_TWO_DIGITS)
    {
      alter = ALTER_THREE_DIGITS;
      kcode = 16 * reg->Read() + kcode;
      reg->Write((kcode | setbits) & (~clearbits));
    }
  else
    {
      alter = ALTER_START;
      kcode = 16 * reg->Read() + kcode;
      reg->Write((kcode | setbits) & (~clearbits));
    }

  Refresh();
}

void WordWindow::OnPaint (wxPaintEvent& WXUNUSED(event))
{
  char foo[8];

  wxPaintDC dc(this);
  dc.SetFont(GetFont());

  sprintf(foo, "%4.4x", reg->Read());

  if (focus)
    {
      dc.SetPen(*wxBLACK_PEN);
      dc.SetBrush(*wxBLACK_BRUSH);
      dc.SetTextForeground(*wxWHITE);
    }
  else
    {
      dc.SetPen(*wxWHITE_PEN);
      dc.SetBrush(*wxWHITE_BRUSH);
      dc.SetTextForeground(*wxBLACK);
    }

  dc.DrawRectangle(cwidth, cheight / 4, 4*cwidth, cheight);
  dc.DrawText(wxString::FromAscii(foo), cwidth, cheight / 4);
  dc.SetTextForeground(*wxBLACK);
}

void WordWindow::NewValue (unsigned short w)
{
  reg->Write(w);
  Refresh();
}

ByteRegisterWindow::ByteRegisterWindow (wxWindow * parent, const char * label, ByteRegister * ptr)
  : wxBoxSizer(wxHORIZONTAL)
{
  bwin = new ByteWindow(parent, 4, ptr);

  Prefs * prefs = Prefs::GetPrefs();
  Add(new Label(parent, label, 0), 0, wxCENTRE, prefs->BorderWidth());
  Add(bwin, 0, wxLEFT, prefs->BorderWidth());
}

void ByteRegisterWindow::Constrain (unsigned char sbits, unsigned char cbits)
{
  bwin->Constrain(sbits, cbits);
}

void ByteRegisterWindow::Refresh ()
{
  bwin->Refresh();
}

void ByteRegisterWindow::TellNewValue (unsigned char b)
{
  bwin->NewValue(b);
}

WordRegisterWindow::WordRegisterWindow (wxWindow * parent, const char * label, WordRegister * ptr)
  : wxBoxSizer(wxHORIZONTAL)
{
  wwin = new WordWindow(parent, 6, ptr);

  Prefs * prefs = Prefs::GetPrefs();
  Add(new Label(parent, label, 0), 0, wxCENTRE, prefs->BorderWidth());
  Add(wwin, 0, wxLEFT, prefs->BorderWidth());
}

void WordRegisterWindow::Constrain (unsigned short sbits, unsigned short cbits)
{
  wwin->Constrain(sbits, cbits);
}

void WordRegisterWindow::Refresh ()
{
  wwin->Refresh();
}

void WordRegisterWindow::TellNewValue (unsigned short w)
{
  wwin->NewValue(w);
}

// end
