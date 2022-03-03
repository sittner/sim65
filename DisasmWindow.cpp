//
// DisasmWindow.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include "DisasmWindow.h"
#include "instructions.h"
#include "AddressPeripheral.h"
#include "disasm.h"
#include "tables.h"
#include "Prefs.h"

#include <cstdio>
#include <cassert>

#include "bkpt_enabled.xpm"
#include "bkpt_disabled.xpm"

BEGIN_EVENT_TABLE(DisasmWindow, wxScrolledWindow)
EVT_PAINT(DisasmWindow::OnPaint)
EVT_LEFT_DOWN(DisasmWindow::OnMouseLeftDown)
END_EVENT_TABLE()

#define DISASM_WIDTH 46

DisasmWindow::DisasmWindow (wxWindow * parent, Memory * nmem, BreakpointManager * nbpm)
  : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxSUNKEN_BORDER)
{
  initial = true;

  keep_pc_in_window = false;

  mem = nmem;
  bpm = nbpm;

  SetThemeEnabled(FALSE);
  SetBackgroundColour(*wxWHITE);

  Prefs * prefs = Prefs::GetPrefs();
  SetFont(wxFont(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  firstaddr = lastaddr = 0;

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

  bkpt_enabled = new wxBitmap(bkpt_enabled_xpm);
  bkpt_disabled = new wxBitmap(bkpt_disabled_xpm);

  bkpt_bmp_width = bkpt_enabled->GetWidth();
  bkpt_bmp_height = bkpt_enabled->GetHeight();

  bkpt_bmp_xoffs = cwidth + (cwidth - bkpt_bmp_width) / 2;
  bkpt_bmp_yoffs = (cheight - bkpt_bmp_height) / 2;

  int client_width = (DISASM_WIDTH * cwidth);
  int scrollbar_width = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, this);

  // this is here to set the width of the virtual window
  // the height is bogus, but more than the height of the client area
  SetScrollbars(cwidth, cheight, DISASM_WIDTH, 1000);

  wxSize csz;
  csz.Set(client_width + scrollbar_width + 1, cheight);
  SetClientSize(csz);
  SetMinClientSize(csz);
}

/* Count the # of lines at and after addr */
int DisasmWindow::num_lines_forward (int addr)
{
  int count = 0;
  int numbytes;

  while (addr <= 0xFFFF)
    {
      if (mem->IsReadable(addr))
	{
	  /* address is readable - see how big the instruction there is */
	  numbytes = instruction_size[READ(addr)];

	  if (numbytes == 0)
	    goto not_instruction;

	  /* all instructions bytes must be readable, otherwise the
	     readable bytes will be considered data */
	  for (int idx = 1; idx < numbytes; ++idx)
	    {
	      if ((addr + idx) > 0xFFFF)
		goto not_instruction;

	      if (mem->IsReadable(addr + idx) == 0)
		goto not_instruction;
	    }
	}
      else
	{
	not_instruction:
	  /* unreadable address - could be I/O, not implemented, etc. */
	  numbytes = 1;
	}

      ++count;
      addrvec[addridx++] = addr;
      addr += numbytes;
    }

  return count;
}

/* Count the # of lines before addr */
int DisasmWindow::num_lines_backward (int addr)
{
  int count = 0;
  int ok_idx;

  addridx = 65536;

  while (addr > 0)
    {
      ok_idx = 0;

      for (int idx = 1; idx < 4; ++idx)
	{
	  if ((addr-idx) < 0)
	    break;

	  if (mem->IsReadable(addr-idx) == 0)
	    continue;

	  /* address is readable - see how big the instruction there is */
	  int numbytes = instruction_size[READ(addr-idx)];

	  /* see if the instruction 'fits' before the last valid addr */
	  if (numbytes != idx)
	    continue;

	  ok_idx = idx;
	}

      if (ok_idx > 0)
	{
	  ++count;
	  addr -= ok_idx;
	}
      else
	{
	  /* unreadable address - could be I/O, not implemented, etc. */
	  ++count;
	  --addr;
	}

      addrvec[--addridx] = addr;
    }

  /* copy addresses to top of vector */
  int idx;
  for (idx = 0; addridx < 65536; ++idx, ++addridx)
    addrvec[idx] = addrvec[addridx];

  addridx = idx;

  return count;
}

void DisasmWindow::OnPaint (wxPaintEvent&  WXUNUSED(event))
{
  int vx, vy;
  int cw, ch;
  char foo[DISASM_WIDTH+8];

  wxPaintDC dc(this);
  PrepareDC(dc);

  dc.SetFont(GetFont());

  GetClientSize(&cw, &ch);
  ch /= cheight;

  GetViewStart(&vx, &vy);
  firstaddr = addrvec[vy];

  if ((vy + ch) >= addridx)
    lastaddr = addrvec[addridx - 1];
  else
    lastaddr = addrvec[vy + ch];

  if (initial == false && keep_pc_in_window == true)
    {
      initial = true;
      keep_pc_in_window = false;

      if ((emPC >= firstaddr) && (emPC < lastaddr))
	{
	  initial = true;
	  for (int idx = vy; (idx < (vy+ch)) && (idx < (addridx-1)); ++idx)
	    if (emPC == addrvec[idx])
	      initial = false;
	}
    }

  if (initial)
    {
      initial = false;

      int lines_bwd = num_lines_backward(emPC);
      int lines_fwd = num_lines_forward(emPC);

      SetScrollbars(cwidth, cheight, DISASM_WIDTH, lines_fwd + lines_bwd);
      GetViewStart(&vx, &vy);

      int spos = lines_bwd;
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
	}
    }

  for (int idx = vy; idx <= (vy + ch); ++idx)
    {
      if (idx < addridx)
	{
	  unsigned short addr = addrvec[idx];

	  if (mem->IsReadable(addr))
	    {
	      int next = idx+1;
	      int len = 1;

	      if (next < addridx)
		len = addrvec[next] - addrvec[idx];
	      else
		len = 0x10000 - addr;

	      disasm(foo, addr, len, 0);

	      if (addr == emPC)
		{
		  dc.SetBrush(*wxBLACK_BRUSH);
		  dc.DrawRectangle(3*cwidth, idx*cheight, cw, cheight);
		  dc.SetTextForeground(*wxWHITE);
		  dc.DrawText(wxString::FromAscii(foo), 4*cwidth, idx*cheight);
		  dc.SetTextForeground(*wxBLACK);
		}
	      else
		{
		  dc.DrawText(wxString::FromAscii(foo), 4*cwidth, idx*cheight);
		}

	      Breakpoint * bpp = bpm->GetBreakpoint(addr);
	      if (bpp != NULL)
		{
		  wxMemoryDC mdc;

		  if (bpp->IsEnabled())
		    {
		      mdc.SelectObject(*bkpt_enabled);
		      dc.Blit(bkpt_bmp_xoffs, (idx * cheight) + bkpt_bmp_yoffs,
			      bkpt_bmp_width, bkpt_bmp_height, &mdc, 0, 0);
		    }
		  else
		    {
		      mdc.SelectObject(*bkpt_disabled);
		      dc.Blit(bkpt_bmp_xoffs, (idx * cheight) + bkpt_bmp_yoffs,
			      bkpt_bmp_width, bkpt_bmp_height, &mdc, 0, 0);
		    }

		  mdc.SelectObject(wxNullBitmap);
		}
	    }
	  else
	    {
	      sprintf(foo, "%4.4x :   [address isn't memory]", addr);
	      dc.DrawText(wxString::FromAscii(foo), 4*cwidth, idx*cheight);
	    }
	}
    }
}

void DisasmWindow::OnMouseLeftDown (wxMouseEvent& event)
{
  int cx = event.GetX() / cwidth;
  int cy = event.GetY() / cheight;

  int vx, vy;
  GetViewStart(&vx, &vy);

  if (cx >= 0 && cx <= 3)
    {
      if ((vy + cy) < addridx)
	{
	  unsigned short addr = addrvec[vy+cy];
	  Breakpoint * bpp = bpm->GetBreakpoint(addr);

	  if (bpp != NULL)
	    bpm->DeleteBreakpoint(bpp);
	  else
	    bpm->AddBreakpoint(Breakpoint(addr));

	  Refresh();
	}
    }
}

void DisasmWindow::TellNewValue (unsigned short w)
{
  initial = true;
  Refresh();
}

void DisasmWindow::TellNewValue (unsigned short addr, unsigned char byte)
{
  if (firstaddr >= lastaddr) return;
  if (addr < firstaddr) return;
  if (addr > lastaddr) return;

  initial = true;
  Refresh();
}

void DisasmWindow::TellAddedBreakpoint (Breakpoint * bp)
{
  Refresh();
}

void DisasmWindow::TellDeletedBreakpoint (Breakpoint * bp)
{
  Refresh();
}

void DisasmWindow::TellEnabledBreakpoint (Breakpoint * bp)
{
  Refresh();
}

void DisasmWindow::TellDisabledBreakpoint (Breakpoint * bp)
{
  Refresh();
}

void DisasmWindow::ForceRefresh (void)
{
  keep_pc_in_window = true;
  Refresh();
}

// end
