//
// StatusWindow.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cassert>
#include "instructions.h"
#include "StatusWindow.h"
#include "ByteRegisterListener.h"
#include "Prefs.h"

class FlagsLabel: public wxWindow
{
public:
  FlagsLabel (wxWindow * parent, int offs);
  void OnPaint (wxPaintEvent& event);

private:
  int offset;
  int cwidth;
  int cheight;

  DECLARE_EVENT_TABLE()
};

class FlagsWindow: public wxWindow
{
public:
  FlagsWindow (wxWindow * parent, unsigned char * N, unsigned char * V, unsigned char * B,
		unsigned char * D, unsigned char * I, unsigned char * Z, unsigned char * C);

  bool AddListener (ByteRegisterListener * new_listener);

  int GetOffset (void);
  void OnPaint (wxPaintEvent& event);
  void OnMouseLeftDown (wxMouseEvent& event);
  void NewFlags (unsigned char p);

private:
  char foo[9];

  unsigned char * pN;
  unsigned char * pV;
  unsigned char * pB;
  unsigned char * pD;
  unsigned char * pI;
  unsigned char * pZ;
  unsigned char * pC;

  int cwidth;
  int cheight;
  int num_listeners;
  ByteRegisterListener * listeners[MAX_NUM_LISTENERS];

  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(FlagsLabel, wxWindow)
EVT_PAINT(FlagsLabel::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(FlagsWindow, wxWindow)
EVT_PAINT(FlagsWindow::OnPaint)
EVT_LEFT_DOWN(FlagsWindow::OnMouseLeftDown)
END_EVENT_TABLE()

FlagsLabel::FlagsLabel (wxWindow * parent, int offs)
  : wxWindow(parent, -1)
{
  int w,h;
  int cw,ch;

  GetSize(&w,&h);
  GetClientSize(&cw,&ch);

  Prefs * prefs = Prefs::GetPrefs();
  offset = offs - ((h - ch) / 2);
  SetFont(wxFont(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

  dc.GetTextExtent(wxString::FromAscii("NV-BDIZC"), &w, &h);

  wxSize csz;
  csz.Set(w, cheight);
  SetClientSize(csz);
  SetMinClientSize(csz);
}

void FlagsLabel::OnPaint (wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);

  dc.SetFont(GetFont());
  dc.DrawText(wxString::FromAscii("NV-BDIZC"), offset, 0);
}

FlagsWindow::FlagsWindow (wxWindow * parent, unsigned char * N, unsigned char * V, unsigned char * B,
			  unsigned char * D, unsigned char * I, unsigned char * Z, unsigned char * C)
  : wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
  pN = N;
  pV = V;
  pB = B;
  pD = D;
  pI = I;
  pZ = Z;
  pC = C;

  foo[2] = '1';
  foo[8] = 0;

  num_listeners = 0;
  SetThemeEnabled(FALSE);
  SetBackgroundColour(*wxWHITE);

  Prefs * prefs = Prefs::GetPrefs();
  SetFont(wxFont(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

  int w, h;
  dc.GetTextExtent(wxString::FromAscii("NV-BDIZC"), &w, &h);
  wxSize csz;
  csz.Set(w + (2 * cwidth), 3 * cheight / 2);
  SetClientSize(csz);
  SetMinClientSize(csz);
}

bool FlagsWindow::AddListener (ByteRegisterListener * new_listener)
{
  if (num_listeners >= MAX_NUM_LISTENERS)
    return false;

  listeners[num_listeners++] = new_listener;
  return true;
}

int FlagsWindow::GetOffset (void)
{
  int w,h;
  int cw,ch;

  GetSize(&w,&h);
  GetClientSize(&cw,&ch);

  return (h - ch) / 2 + cwidth;
}

void FlagsWindow::OnPaint (wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  dc.SetFont(GetFont());

  foo[0] = (*pN) ? '1' : '0';
  foo[1] = (*pV) ? '1' : '0';
  foo[3] = (*pB) ? '1' : '0';
  foo[4] = (*pD) ? '1' : '0';
  foo[5] = (*pI) ? '1' : '0';
  foo[6] = (*pZ) ? '1' : '0';
  foo[7] = (*pC) ? '1' : '0';

  // dc.Clear();
  dc.DrawText(wxString::FromAscii(foo), cwidth, cheight / 4);
}

void FlagsWindow::OnMouseLeftDown (wxMouseEvent& event)
{
  int x = event.GetX();

  unsigned char op = build_P();

  switch (x / cwidth)
    {
    case 1:
      *pN = 1 - *pN;
      break;

    case 2:
      *pV = 1 - *pV;
      break;

    case 4:
      *pB = 1 - *pB;
      break;

    case 5:
      if (*pD)
	iD8_CLD();
      else
	iF8_SED();
      break;

    case 6:
      *pI = 1 - *pI;
      break;

    case 7:
      *pZ = 1 - *pZ;
      break;

    case 8:
      *pC = 1 - *pC;
      break;

    default:
      return;
    }

  unsigned char np = build_P();

  if (op != np)
    {
      for (int idx = 0; idx < num_listeners; ++idx)
	listeners[idx]->TellNewValue(np);
    }

  Refresh();
}

void FlagsWindow::NewFlags (unsigned char p)
{
  *pC = p & 1; p >>= 1;
  *pZ = p & 1; p >>= 1;
  *pI = p & 1; p >>= 1;

  if (p & 1)
    iF8_SED();
  else
    iD8_CLD();
  p >>= 1;

  *pB = p & 1; p >>= 2;
  *pV = p & 1; p >>= 1;
  *pN = p & 1;

  Refresh();
}

StatusWindow::StatusWindow (wxWindow * parent, unsigned char * N, unsigned char * V, unsigned char * B,
			    unsigned char * D, unsigned char * I, unsigned char * Z, unsigned char * C)
  : wxBoxSizer(wxVERTICAL)
{
  fw = new FlagsWindow(parent, N, V, B, D, I, Z, C);
  FlagsLabel * fl = new FlagsLabel(parent, fw->GetOffset());

  Prefs * prefs = Prefs::GetPrefs();
  Add(fl, 0, wxEXPAND, prefs->BorderWidth());
  Add(fw, 0, wxTOP, prefs->BorderWidth());
}

void StatusWindow::Refresh ()
{
  fw->Refresh();
}

void StatusWindow::TellNewValue (unsigned char b)
{
  fw->NewFlags(b);
}

bool StatusWindow::AddListener (ByteRegisterListener * new_listener)
{
  return fw->AddListener(new_listener);
}

// end
