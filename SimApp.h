//
// SimApp.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_SIMAPP_H
#define INCLUDED_SIMAPP_H

#include "wx/wx.h"
#include "SimFrame.h"

class SimApp: public wxApp
{
  virtual bool OnInit();

  SimFrame * sim_frame;
};

#endif // INCLUDED_SIMAPP_H
