//
// SimApp.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include "SimApp.h"

IMPLEMENT_APP(SimApp)

bool SimApp::OnInit()
{
  wxString t = wxString::FromAscii("Sim65 (development)");
  // sim_frame = new SimFrame(t, wxPoint(-1,-1), wxSize(16,16));
  sim_frame = new SimFrame(t, wxPoint(-1,-1), wxDefaultSize);
  sim_frame->Show(TRUE);
  SetTopWindow(sim_frame);

  return TRUE;
}
