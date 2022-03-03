//
// BreakpointWindow.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include "BreakpointWindow.h"
#include "Prefs.h"

#define BPWIN_WIDTH 20

enum {
  BP_Goto = 1,
  BP_Enable,
  BP_Disable,
  BP_Delete
};

BEGIN_EVENT_TABLE(BreakpointWindow, wxScrolledWindow)
EVT_PAINT(BreakpointWindow::OnPaint)
EVT_LEFT_DOWN(BreakpointWindow::OnMouseLeftDown)
EVT_RIGHT_DOWN(BreakpointWindow::OnMouseRightDown)
EVT_MENU(BP_Goto, BreakpointWindow::OnBPGoto)
EVT_MENU(BP_Enable, BreakpointWindow::OnBPEnable)
EVT_MENU(BP_Disable, BreakpointWindow::OnBPDisable)
EVT_MENU(BP_Delete, BreakpointWindow::OnBPDelete)
END_EVENT_TABLE()

BreakpointWindow::BreakpointWindow (wxWindow * parent, BreakpointManager * nbpm)
  : wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxSUNKEN_BORDER)
{
  bpm      = nbpm;
  initial  = true;
  topline  = 0;
  selected = -1;

  bpMenuDisable = new wxMenu();
  // bpMenuDisable->Append(BP_Goto, "Go to");
  bpMenuDisable->Append(BP_Disable, wxString::FromAscii("Disable"));
  bpMenuDisable->Append(BP_Delete, wxString::FromAscii("Delete"));

  bpMenuEnable = new wxMenu();
  // bpMenuEnable->Append(BP_Goto, "Go to");
  bpMenuEnable->Append(BP_Enable, wxString::FromAscii("Enable"));
  bpMenuEnable->Append(BP_Delete, wxString::FromAscii("Delete"));

  SetThemeEnabled(FALSE);
  SetBackgroundColour(*wxWHITE);

  Prefs * prefs = Prefs::GetPrefs();
  SetFont(wxFont(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  cwidth = GetCharWidth();
  cheight = GetCharHeight();

  wxSize csz;
  csz.Set(BPWIN_WIDTH * cwidth, cheight);
  SetClientSize(csz);
  SetMinClientSize(csz);
  
  SetScrollbars(cwidth, cheight, BPWIN_WIDTH, 0);
}

void BreakpointWindow::OnPaint (wxPaintEvent&  WXUNUSED(event))
{
  char foo[32];

  wxPaintDC dc(this);
  PrepareDC(dc);

  dc.SetFont(GetFont());

  int vx;
  int cw, ch;

  GetViewStart(&vx, &topline);
  GetClientSize(&cw, &ch);
  ch /= cheight;

  for (int idx = topline; idx <= (topline + ch); ++idx)
    {
      if (idx == bpm->NumBreakpoints())
	break;

      Breakpoint * bp = bpm->GetBreakpointAtIndex(idx);

      if (idx == selected)
	{
	  dc.SetBrush(*wxBLACK_BRUSH);
	  dc.DrawRectangle(0, idx*cheight, cw, cheight);
	  if (bp->IsEnabled())
	    dc.SetTextForeground(*wxWHITE);
	  else
	    dc.SetTextForeground(*wxLIGHT_GREY);
	}
      else
	{
	  if (bp->IsEnabled())
	    dc.SetTextForeground(*wxBLACK);
	  else
	    dc.SetTextForeground(*wxLIGHT_GREY);
	}

      const char * condstr = 0;
      bpflags_t cond = bp->GetConditionFlags();

      if (cond & bp_equal)
	if (cond & bp_less)
	  condstr = "<=";
	else if (cond & bp_greater)
	  condstr = ">=";
	else
	  condstr = "=";
      else if (cond & bp_less)
	condstr = "<";
      else if (cond & bp_greater)
	condstr = ">";
      else if (cond & bp_notequal)
	condstr = "!=";
      else
	condstr = "???";

      bpflags_t op = bp->GetOperandFlags();

      if (op == bp_PC)
	sprintf(foo, "%2d) PC %s $%4.4x", idx+1, condstr, bp->GetAddress());
      else if (op == bp_S)
	sprintf(foo, "%2d) S %s $%2.2x", idx+1, condstr, bp->GetValue());

      dc.DrawText(wxString::FromAscii(foo), 2*cwidth, idx*cheight);
    }
}

void BreakpointWindow::OnMouseLeftDown (wxMouseEvent& event)
{
  int cy, vx, vy;

  GetViewStart(&vx, &vy);
  cy = event.GetY() / cheight;

  if ((vy + cy) < bpm->NumBreakpoints())
    {
      if (selected == (vy + cy))
	selected = -1;
      else
	selected = vy + cy;

      Refresh();
    }
}

void BreakpointWindow::OnMouseRightDown (wxMouseEvent& event)
{
  int cy, vx, vy;
  int old_selected = selected;

  GetViewStart(&vx, &vy);
  cy = event.GetY() / cheight;

  if ((vy + cy) < bpm->NumBreakpoints())
    {
      selected = vy + cy;

      if (bpm->GetBreakpointAtIndex(selected)->IsEnabled())
	PopupMenu(bpMenuDisable, event.GetX(), event.GetY());
      else
	PopupMenu(bpMenuEnable, event.GetX(), event.GetY());

      if (selected != old_selected)
	Refresh();
    }
}

void BreakpointWindow::OnBPGoto (wxCommandEvent& event)
{
}

void BreakpointWindow::OnBPEnable (wxCommandEvent& event)
{
  bpm->EnableBreakpointAtIndex(selected);
}

void BreakpointWindow::OnBPDisable (wxCommandEvent& event)
{
  bpm->DisableBreakpointAtIndex(selected);
}

void BreakpointWindow::OnBPDelete (wxCommandEvent& event)
{
  bpm->DeleteBreakpointAtIndex(selected);
}

void BreakpointWindow::TellEnabledBreakpoint(Breakpoint * bp)
{
  Refresh();
}

void BreakpointWindow::TellDisabledBreakpoint(Breakpoint * bp)
{
  Refresh();
}

void BreakpointWindow::TellAddedBreakpoint (Breakpoint * bp)
{
  SetScrollbars(cwidth, cheight, BPWIN_WIDTH, bpm->NumBreakpoints());
  Scroll(0, topline);
  Refresh();
}

void BreakpointWindow::TellDeletedBreakpoint (Breakpoint * bp)
{
  Refresh();
}

// end
