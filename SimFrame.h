//
// SimFrame.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_SIMFRAME_H
#define INCLUDED_SIMFRAME_H

#include <wx/wx.h>
#include "ByteRegister.h"
#include "WordRegister.h"
#include "RegisterWindow.h"
#include "StatusWindow.h"
#include "Label.h"
#include "StackWindow.h"
#include "DisasmWindow.h"
#include "MemWindow.h"
#include "Core65c02.h"
#include "BreakpointWindow.h"
#include "BreakpointManager.h"
#include "srecord.h"
#include "AddressPeripheral.h"

#define MAX_NUM_WMITEMS 8
#define MAX_NUM_ADDRESS_PERIPHERALS 32

class SimFrame: public wxFrame
{
public:
  SimFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  ~SimFrame ();

  void OnNew (wxCommandEvent& event);
  void OnOpen (wxCommandEvent& event);
  void OnLoadBinary (wxCommandEvent& event);
  void OnLoadSRecords (wxCommandEvent& event);
  void OnExit (wxCommandEvent& event);
  void OnClose (wxCloseEvent& event);

  void OnHelp (wxCommandEvent& event);
  void OnAbout (wxCommandEvent& event);

  void OnRun (wxCommandEvent& event);
  void OnStop (wxCommandEvent& event);
  void OnStep (wxCommandEvent& event);
  void OnMStep (wxCommandEvent& event);
  void OnTrace (wxCommandEvent& event);
  void OnStepOver (wxCommandEvent& event);
  void OnReset (wxCommandEvent& event);

  void OnBPEnableAll (wxCommandEvent& event);
  void OnBPDeleteAll (wxCommandEvent& event);
  void OnBPDisableAll (wxCommandEvent& event);

  void OnWinMenu (wxCommandEvent& event);

  void OnIdle (wxIdleEvent& event);
  void OnPaint (wxPaintEvent& event);

private:
  bool running;
  bool stepping;

  wxMenu *winmenu;
  unsigned int numwmitems;
  wxFrame * frame[MAX_NUM_WMITEMS];
  bool wmchecked[MAX_NUM_WMITEMS];  // shouldn't need this. wx seems not to work (at least on GTK2)

  BreakpointManager * bpm;

  ByteRegister * areg;
  ByteRegister * xreg;
  ByteRegister * yreg;
  ByteRegister * sreg;
  ByteRegister * preg;
  WordRegister * pcreg;

  ByteRegisterWindow * awin;
  ByteRegisterWindow * xwin;
  ByteRegisterWindow * ywin;
  ByteRegisterWindow * swin;
  ByteRegisterWindow * pwin;
  WordRegisterWindow * pcwin;

  Memory * mem;
  NullPeripheral * nul;

  unsigned int num_aps;
  AddressPeripheral * ap[MAX_NUM_ADDRESS_PERIPHERALS];

  StatusWindow * stwin;
  StackWindow * stkwin;
  DisasmWindow * diswin;
  MemWindow * memwin;
  BreakpointWindow * bpwin;

  wxButton * btrun;
  wxButton * btstop;
  wxButton * btstep;
  wxButton * btmstep;
  wxButton * bttrace;
  wxButton * btstepover;

  Core65c02 cpu;

  void load_srecord (struct srecord * srec, unsigned long * total);

  DECLARE_EVENT_TABLE()
};

#endif // INCLUDED_SIMFRAME_H
