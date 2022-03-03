//
// StackWindow.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cstdio>
#include "StackWindow.h"
#include "instructions.h"
#include "AddressPeripheral.h"
#include "Prefs.h"

BEGIN_EVENT_TABLE(StackWindow, wxScrolledWindow)
EVT_PAINT(StackWindow::OnPaint)
END_EVENT_TABLE()

StackWindow::StackWindow (wxWindow * parent)
  : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxSUNKEN_BORDER)
{
#if 0
  SetRunning(FALSE);
#else
  initial = TRUE;
#endif

  SetThemeEnabled(FALSE);
  SetBackgroundColour(*wxWHITE);

  Prefs * prefs = Prefs::GetPrefs();
  SetFont(wxFont(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  firstaddr = lastaddr = 0;

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

#define STACKWIN_WIDTH (2 + 5 + 1 + 2 + 2)

  int client_width = (STACKWIN_WIDTH * cwidth);
  int scrollbar_width = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, this);
  SetScrollbars(cwidth, cheight, STACKWIN_WIDTH, 256);

  wxSize csz;
  csz.Set(client_width + scrollbar_width + 1, cheight);
  SetClientSize(csz);
  SetMinClientSize(csz);
}

#if 0
void StackWindow::SetRunning (bool isRunning)
{
  running = isRunning;
  if (isRunning == false)
    initial = true;
}
#endif

void StackWindow::OnPaint (wxPaintEvent&  WXUNUSED(event))
{
  char foo[16];

  wxPaintDC dc(this);
  PrepareDC(dc);

  dc.SetFont(GetFont());

  int cw, ch;
  GetClientSize(&cw, &ch);
  ch /= cheight;

  int vx, vy;
  GetViewStart(&vx, &vy);

#if 0
  if (running || initial)
#else
  if (initial)
#endif
    {
      initial = false;

      // position (line number) of top-of-stack in virtual window
      int spos = 0xFF - S;

      // if top-of-stack is not (completely) in the visible window
      if (spos < vy || spos >= (vy + ch))
	{
	  // vh = number of lines in virtual window
	  int vw, vh;
	  GetVirtualSize(&vw, &vh);
	  vh /= cheight;

	  // if, when scrolled all the way down, top-of-stack is in window
	  //    then scroll all the way down
	  // else if it is possible to scroll such that t-o-s is centered
	  //    then scroll such that t-o-s is centered.

	  if ((vh - spos) < ch)
	    Scroll(0, vh - ch);
	  else if (spos >= (ch / 2))
	    Scroll(0, spos - (ch/2));

	  GetViewStart(&vx, &vy);
	}

      // set firstaddr & lastaddr
      lastaddr = 0x1FF - vy;
      firstaddr = 0x1FF - (vy + ch);
      if (firstaddr < 0x100) firstaddr = 0x100;
    }
  
  for (int idx = vy; idx <= (vy + ch); ++idx)
    {
      int addr = 0x1FF - idx;

      if (addr >= 0x100)
	{
	  sprintf(foo, "%4.4x: %2.2x", addr, READ(addr));

	  if ((addr & 0xff) == S)
	    {
	      dc.SetBrush(*wxBLACK_BRUSH);
	      dc.DrawRectangle(0, idx*cheight, cw, cheight);
	      dc.SetTextForeground(*wxWHITE);
	      dc.DrawText(wxString::FromAscii(foo), 2*cwidth, idx*cheight);
	      dc.SetTextForeground(*wxBLACK);
	    }
	  else
	    {
	      dc.DrawText(wxString::FromAscii(foo), 2*cwidth, idx*cheight);
	    }
	}
    }
}

void StackWindow::TellNewValue (unsigned char b)
{
  initial = true;
  Refresh(FALSE);
}

void StackWindow::TellNewValue (unsigned short addr, unsigned char byte)
{
  if (firstaddr >= lastaddr) return;
  if (addr < firstaddr) return;
  if (addr > lastaddr) return;

  int cw, ch;
  GetClientSize(&cw, &ch);

  int y = cheight * (lastaddr - addr);

  initial = true;
  RefreshRect(wxRect(0, y, cw, cheight));
}

void StackWindow::ForceRefresh (void)
{
  initial = true;
  Refresh();
}

// end
