//
// MemWindow.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include "MemWindow.h"
#include "instructions.h"
#include "Prefs.h"

#define ALTER_START     0
#define ALTER_ONE_DIGIT 1

BEGIN_EVENT_TABLE(MemWindow, wxScrolledWindow)
EVT_CHAR(MemWindow::OnChar)
EVT_PAINT(MemWindow::OnPaint)
EVT_KEY_DOWN(MemWindow::OnKeyDown)
EVT_LEFT_DOWN(MemWindow::OnMouseLeftDown)
EVT_SET_FOCUS(MemWindow::OnSetFocus)
EVT_KILL_FOCUS(MemWindow::OnKillFocus)
END_EVENT_TABLE()

MemWindow::MemWindow (wxWindow * parent, Memory * mem)
  : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxSUNKEN_BORDER)
{
  memory = mem;

#if 0
  SetRunning(FALSE);
#else
  adjust_view = true;
#endif

  SetThemeEnabled(FALSE);
  SetBackgroundColour(*wxWHITE);

  Prefs * prefs = Prefs::GetPrefs();
  SetFont(wxFont(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  caddr = -1;
  memaddr = 0;
  cx = cy = -1;
  alter = ALTER_START;

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

#define MEMWINDOW_WIDTH (2 + 5 + 2 + (8*3) + 1 + (8*3) + 2 + 8 + 1 + 8 + 2)

  SetScrollbars(cwidth, cheight, MEMWINDOW_WIDTH, 4096);

  int client_width  = MEMWINDOW_WIDTH * cwidth;
  int client_height = 12 * cheight;

  int cw, ch;
  wxSize csz;
  csz.Set(client_width, client_height);
  SetClientSize(csz);
  SetMinClientSize(csz);
  GetClientSize(&cw, &ch);

  // printf("1: set w=%d, h=%d   got w=%d, h=%d\n", client_width, client_height, cw, ch);

  // GTK2 in particular does some weird stuff here. After the first
  // SetClientSize/GetClientSize pair, ch > client_width. It seems like
  // SetClientSite is adding space for a horizontal scrollbar (which will
  // never be there). But after the second !identical! pair of calls,
  // ch == client_width. I can't explain this.

  if (cw != client_width || ch != client_height)
    {
      // printf("diff = %d\n", ch - client_height);

      SetClientSize(client_width, client_height);
      GetClientSize(&cw, &ch);

      //printf("2: set w=%d, h=%d   got w=%d, h=%d\n", client_width, client_height, cw, ch);
    }

  // fflush(stdout);
}

#if 0
void MemWindow::SetRunning (bool isRunning)
{
  running = isRunning;
  if (isRunning == false)
    adjust_view = true;
}
#endif

void MemWindow::select_byte (int addr, int cx, int cy, int onoff)
{
  char foo[8];

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  sprintf(foo, "%2.2x", READ(addr));

  if (cx < 33)
    cx -= (cx % 3);
  else
    cx = (cx+2) - ((cx+2)%3) - 2;

  if (onoff)
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

  dc.DrawRectangle(cx*cwidth, cy*cheight, 2*cwidth, cheight);
  dc.DrawText(wxString::FromAscii(foo), cx*cwidth, cy*cheight);
  dc.SetTextForeground(*wxBLACK);
}

void MemWindow::OnPaint (wxPaintEvent&  WXUNUSED(event))
{
  char foo[MEMWINDOW_WIDTH + 8];
  char bar[32];

  wxPaintDC dc(this);
  PrepareDC(dc);

  dc.SetFont(GetFont());

  int vx, vy;
  int cw, ch;

  GetViewStart(&vx, &vy);
  GetClientSize(&cw, &ch);
  ch /= cheight;

  //printf("2: got w=%d, h=%d\n", cw, ch);

#if 0
  if (running || adjust_view)
#else
  if (adjust_view)
#endif
    {
      adjust_view = false;

      int spos = memaddr / 16;
      if (spos < vy || spos >= (vy + ch))
	{
	  int vw, vh;
	  GetVirtualSize(&vw, &vh);
	  vh /= cheight;

	  if ((vh - spos) < ch)
	    spos = vh - ch;
	  else if (spos >= (ch / 2))
	    spos -= (ch / 2);

	  Scroll(0, spos);
	  GetViewStart(&vx, &vy);
	}
    }

  memaddr = vy * 16;

  for (int idx = vy; idx <= (vy + ch); ++idx)
    {
      int foonum = 0;
      int barnum = 0;
      int addr = idx * 16;

      if (addr < 65536)
	{
	  foonum = sprintf(foo, "%4.4x:  ", addr);
	  barnum = sprintf(bar, "  ");
	  
	  for (int jdx = addr; jdx < (addr+16); ++jdx)
	    {
	      if (memory->IsReadable(jdx))
		{
		  unsigned char membyte = READ(jdx);
		  foonum += sprintf(foo + foonum, "%2.2x ", membyte);
		  if (isprint(membyte))
		    barnum += sprintf(bar + barnum, "%c", membyte);
		  else
		    barnum += sprintf(bar + barnum, ".");
		}
	      else
		{
		  foonum += sprintf(foo + foonum, "-- ");
		  barnum += sprintf(bar + barnum, " ");
		}

	      if (jdx == (addr+7))
		{
		  foonum += sprintf(foo + foonum, " ");
		  barnum += sprintf(bar + barnum, " ");
		}
	    }

	  sprintf(foo + foonum, "%s", bar);
	  dc.SetTextForeground(*wxBLACK);
	  dc.DrawText(wxString::FromAscii(foo), 2*cwidth, idx*cheight);
	}
    }

  if ((caddr >= 0) && (caddr < 65536) && memory->IsReadable(caddr))
    {
      int ocx, ocy;
      if (address_to_coord(caddr, &ocx, &ocy))
	{
	  // hilite the selected byte
	  select_byte(caddr, ocx, ocy, 1);
	}
    }
}

void MemWindow::OnChar (wxKeyEvent& event)
{
  int kcode = event.GetKeyCode();

  if (!isxdigit(kcode))
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
      WRITE(caddr, kcode);
    }
  else
    {
      alter = ALTER_START;
      kcode = 16 * READ(caddr) + kcode;
      WRITE(caddr, kcode);
    }

  int cw, ch;
  GetClientSize(&cw, &ch);

  int cx, cy;
  bool caddr_visible = address_to_coord(caddr, &cx, &cy);

  if (caddr_visible == false)
    {
      scrollto(caddr);
      address_to_coord(caddr, &cx, &cy);
    }

  // refresh the entire line
  RefreshRect(wxRect(0, cy * cheight, cw, cheight));
}

void MemWindow::OnKeyDown (wxKeyEvent& event)
{
  if (!address_in_window(caddr))
    scrollto(caddr);

  switch (event.GetKeyCode())
    {
    case WXK_UP:
      up_arrow();
      break;

    case WXK_DOWN:
      down_arrow();
      break;

    case WXK_LEFT:
      left_arrow();
      break;

    case WXK_RIGHT:
      right_arrow();
      break;

    default:
      event.Skip();
      break;
    }
}

void MemWindow::scrollto (int addr)
{
  if (addr < 0 || addr >= 0x10000) return;

  int addrline = addr / 16;

  int cw, ch;
  GetClientSize(&cw, &ch);

  int visiblelines = ch / cheight;
  int halflines = visiblelines / 2;

  int vw, vh;
  GetVirtualSize(&vw, &vh);

  int totallines = vh / cheight;

  int vx, vy;
  GetViewStart(&vx, &vy);

  if (addrline < halflines)
    {
      memaddr = 0;
      Scroll(vx, 0);
    }
  else if ((totallines - addrline) < halflines)
    {
      memaddr = 16 * (totallines - halflines);
      Scroll(vx, totallines - halflines);
    }
  else
    {
      memaddr = 16 * (addrline - halflines);
      Scroll(vx, addrline - halflines);
    }
}

void MemWindow::change_caddr_noscroll (unsigned short naddr)
{
  int cx, cy;
  bool caddr_visible = address_to_coord(caddr, &cx, &cy);

  assert(caddr_visible);      // at this point caddr should always be visible

  int nx, ny;
  bool naddr_visible = address_to_coord(naddr, &nx, &ny);

  assert(naddr_visible);      // since no scroll, naddr must be visible

  // the rectangle needing refresh is the union of the rectangles
  // enclosing the current and next addresses
  int x = (cx < nx) ? cx : nx;
  int y = (cy < ny) ? cy : ny;
  int w = abs(cx - nx) + 2;
  int h = abs(cy - ny) + 1;

  RefreshRect(wxRect(x * cwidth, y * cheight, w * cwidth, h * cheight));

  caddr = naddr;
}

void MemWindow::change_caddr_scrollup (unsigned short naddr)
{
  int lines = (memaddr / 16) - (naddr / 16);
  memaddr -= (16*lines);

  int cw, ch;
  GetClientSize(&cw, &ch);

  int vx, vy;
  GetViewStart(&vx, &vy);

  Scroll(vx, vy - lines);

  lines += 1;
  RefreshRect(wxRect(0, 0, cw, lines * cheight));

  caddr = naddr;
}

void MemWindow::change_caddr_scrolldn (unsigned short naddr, unsigned short lastaddr)
{
  int lines = 1 + (naddr / 16) - (lastaddr / 16);
  memaddr += (16*lines);

  int cw, ch;
  GetClientSize(&cw, &ch);

  int vx, vy;
  GetViewStart(&vx, &vy);

  Scroll(vx, vy + lines);

  lines += 1;
  RefreshRect(wxRect(0, ch - (lines * cheight + 1), cw, lines * cheight + 1));

  caddr = naddr;
}

void MemWindow::up_arrow ()
{
  if (caddr < 0) return;          // no current address
  if (caddr < 16) return;         // can't move up a line

  // find up-one-or-more-lines address, skipping all addresses that don't map to RAM or ROM
  int naddr;
  for (naddr = caddr - 16; !memory->IsReadable(naddr); naddr -= 16)
    if (naddr < 0) break;

  if (naddr < 0) return;         // can't go up one or more lines, so WKX_UP does nothing

  // have to scroll?
  if (naddr < memaddr)
    change_caddr_scrollup(naddr);
  else
    change_caddr_noscroll(naddr);

  alter = ALTER_START;
}

void MemWindow::down_arrow ()
{
  if (caddr < 0) return;          // no current address
  if (caddr >= 0xFFF0) return;    // can't move down a line

  // find down-one-or-more-lines address, skipping all addresses that don't map to RAM or ROM
  int naddr;
  for (naddr = caddr + 16; !memory->IsReadable(naddr); naddr += 16)
    if (naddr > 0xFFFF) break;

  if (naddr > 0xFFFF) return;    // can't go down one or more lines, so WKX_DOWN does nothing

  // find out if scrolling is necessary
  int cw, ch;
  GetClientSize(&cw, &ch);
  int lastaddr = memaddr + (16 * (ch / cheight));

  // have to scroll?
  if (naddr >= lastaddr)
    change_caddr_scrolldn(naddr, lastaddr);
  else
    change_caddr_noscroll(naddr);

  alter = ALTER_START;
}

void MemWindow::left_arrow ()
{
  if (caddr <= 0) return;         // no current address OR can't move to previous address

  // find 'previous' address, skipping all addresses that don't map to RAM or ROM
  int naddr;
  for (naddr = caddr - 1; !memory->IsReadable(naddr); --naddr)
    if (naddr < 0) break;

  if (naddr < 0) return;         // can't go back, so WKX_LEFT does nothing

  // have to scroll?
  if (naddr < memaddr)
    change_caddr_scrollup(naddr);
  else
    change_caddr_noscroll(naddr);

  alter = ALTER_START;
}

void MemWindow::right_arrow ()
{
  if (caddr < 0) return;          // no current address
  if (caddr >= 0xFFFF) return;    // can't move to next address

  // find 'next' address, skipping all addresses that don't map to RAM or ROM
  int naddr;
  for (naddr = caddr + 1; !memory->IsReadable(naddr); ++naddr)
    if (naddr > 0xFFFF) break;

  if (naddr > 0xFFFF) return;    // can't advance, so WKX_RIGHT does nothing

  // find out if scrolling is necessary
  int cw, ch;
  GetClientSize(&cw, &ch);
  int lastaddr = memaddr + (16 * (ch / cheight));

  // have to scroll?
  if (naddr >= lastaddr)
    change_caddr_scrolldn(naddr, lastaddr);
  else
    change_caddr_noscroll(naddr);

  alter = ALTER_START;
}

// for reference
// #define MEMWINDOW_WIDTH (2 + 5 + 2 + (8*3) + 1 + (8*3) + 2 + 8 + 1 + 8 + 2)

int MemWindow::coord_to_address (int cx, int cy)
{
  if (cx < 9 || cx > 58) return -1;
  if (cx == 33) return -1;

  if (cx < 33)
    return memaddr + (16 * cy) + ((cx - 9) / 3);

  return memaddr + (16 * cy) + ((cx - 34) / 3) + 8;
}

bool MemWindow::address_in_window (int addr)
{
  int cw, ch;
  GetClientSize(&cw, &ch);
  ch /= cheight;

  if (addr < 0 || addr >= 0x10000) return false;
  if (addr < memaddr) return false;
  if (addr > (memaddr + ((ch+1) * 16))) return false;

  return true;
}

bool MemWindow::address_to_coord (int addr, int * cx, int * cy)
{
  if (!address_in_window(addr)) return false;

  int x = 3 * ((addr - memaddr) % 16);
  x += 9;

  if (x > 30) ++x;

  *cy = (addr - memaddr) / 16;
  *cx = x;

  return true;
}

void MemWindow::OnMouseLeftDown (wxMouseEvent& event)
{
  //wxClientDC dc(this);
  //dc.SetFont(GetFont());

  cx = event.GetX() / cwidth;
  cy = event.GetY() / cheight;

  int addr = coord_to_address(cx, cy);
  if (addr < 0) return;

  if (memory->IsReadable(addr))
    {
      if ((caddr >= 0) && (caddr < 0x10000) && memory->IsReadable(caddr))
	{
	  int ocx, ocy;
	  if (address_to_coord(caddr, &ocx, &ocy))
	    select_byte(caddr, ocx, ocy, 0);
	}

      select_byte(addr, cx, cy, 1);
      caddr = addr;
    }

  this->SetFocus();
}

void MemWindow::OnSetFocus (wxFocusEvent& evt)
{
}

void MemWindow::OnKillFocus (wxFocusEvent& evt)
{
  cx = cy = -1;
  if ((caddr >= 0) && (caddr < 0x10000) && memory->IsReadable(caddr))
    {
      int ocx, ocy;
      if (address_to_coord(caddr, &ocx, &ocy))
	select_byte(caddr, ocx, ocy, 0);
    }
}

void MemWindow::ForceRefresh (void)
{
  adjust_view = true;
  Refresh();
}

// end
