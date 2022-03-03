//
// SimFrame.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cctype>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <wx/file.h>
#include "SimFrame.h"
#include "instructions.h"
#include "AddressDialog.h"
#include "Prefs.h"
#include "Acia6551.h"
#include "Acia6551Frame.h"
#include "AppleTextFrame.h"

#define NORESIZE_FRAME (wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))

enum
  {
    File_New = 1,
    File_Open,
    File_LoadBinary,
    File_LoadSRecords,
    File_Exit,
    Help_Help,
    Help_About,
    Run_Run,
    Run_Stop,
    Run_Step,
    Run_StepOver,
    Run_MStep,
    Run_Trace,
    Run_Reset,
    BP_EnableAll,
    BP_DisableAll,
    BP_DeleteAll,
    Butt_Run,
    Butt_Stop,
    Butt_Step,
    Butt_StepOver,
    Butt_MStep,
    Butt_Trace,
    Win_First // this should go last
  };

BEGIN_EVENT_TABLE(SimFrame, wxFrame)
EVT_MENU(File_New, SimFrame::OnNew)
EVT_MENU(File_Open, SimFrame::OnOpen)
EVT_MENU(File_LoadBinary, SimFrame::OnLoadBinary)
EVT_MENU(File_LoadSRecords, SimFrame::OnLoadSRecords)
EVT_MENU(File_Exit, SimFrame::OnExit)
EVT_MENU(Help_Help, SimFrame::OnHelp)
EVT_MENU(Help_About, SimFrame::OnAbout)
EVT_MENU(Run_Run, SimFrame::OnRun)
EVT_MENU(Run_Stop, SimFrame::OnStop)
EVT_MENU(Run_Step, SimFrame::OnStep)
EVT_MENU(Run_StepOver, SimFrame::OnStepOver)
EVT_MENU(Run_MStep, SimFrame::OnMStep)
EVT_MENU(Run_Trace, SimFrame::OnTrace)
EVT_MENU(Run_Reset, SimFrame::OnReset)
EVT_MENU(BP_EnableAll, SimFrame::OnBPEnableAll)
EVT_MENU(BP_DisableAll, SimFrame::OnBPDisableAll)
EVT_MENU(BP_DeleteAll, SimFrame::OnBPDeleteAll)
EVT_MENU_RANGE(Win_First, Win_First+7, SimFrame::OnWinMenu)
EVT_BUTTON(Butt_Run, SimFrame::OnRun)
EVT_BUTTON(Butt_Stop, SimFrame::OnStop)
EVT_BUTTON(Butt_Step, SimFrame::OnStep)
EVT_BUTTON(Butt_StepOver, SimFrame::OnStepOver)
EVT_BUTTON(Butt_MStep, SimFrame::OnMStep)
EVT_BUTTON(Butt_Trace, SimFrame::OnTrace)
EVT_IDLE(SimFrame::OnIdle)
EVT_CLOSE(SimFrame::OnClose)
END_EVENT_TABLE()

SimFrame::SimFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
  : wxFrame((wxFrame *)NULL, -1, title, pos, size, NORESIZE_FRAME)
{
  SetThemeEnabled(FALSE);

  CreateStatusBar();

  num_aps = 0;
  running = false;
  stepping = false;
  Prefs * prefs = Prefs::GetPrefs();

  cpu.SetFrequency(1023000);

  numwmitems = 0;
  winmenu  = new wxMenu();

  wxMenu *menuFile = new wxMenu();
  wxMenu *menuHelp = new wxMenu();
  wxMenu *menuRun  = new wxMenu();
  wxMenu *menuBP  = new wxMenu();

  // menuFile->Append(File_New, wxString::FromAscii("&New Project..."));
  // menuFile->Append(File_Open, wxString::FromAscii("&Open Project..."));
  // menuFile->AppendSeparator();
  menuFile->Append(File_LoadBinary, wxString::FromAscii("&Load Binary Image..."));
  menuFile->Append(File_LoadSRecords, wxString::FromAscii("&Load S-Record File..."));
  menuFile->AppendSeparator();
  menuFile->Append(File_Exit, wxString::FromAscii("E&xit"));

  menuRun->Append(Run_Run, wxString::FromAscii("Run"));
  menuRun->Append(Run_Stop, wxString::FromAscii("Stop"));
  menuRun->Append(Run_Step, wxString::FromAscii("Step"));
  menuRun->Append(Run_StepOver, wxString::FromAscii("Step Over"));
  menuRun->Append(Run_MStep, wxString::FromAscii("Multistep"));
  // menuRun->Append(Run_Trace, wxString::FromAscii("Trace"));
  menuRun->AppendSeparator();
  menuRun->Append(Run_Reset, wxString::FromAscii("Reset"));

  menuBP->Append(BP_EnableAll, wxString::FromAscii("Enable All"));
  menuBP->Append(BP_DisableAll, wxString::FromAscii("Disable All"));
  menuBP->AppendSeparator();
  menuBP->Append(BP_DeleteAll, wxString::FromAscii("Delete All"));

  menuHelp->Append(Help_Help, wxString::FromAscii("&Help"));
  menuHelp->Append(Help_About, wxString::FromAscii("&About..."));

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, wxString::FromAscii("&File"));
  menuBar->Append(menuRun, wxString::FromAscii("&Run"));
  menuBar->Append(menuBP, wxString::FromAscii("Breakpoints"));
  menuBar->Append(winmenu, wxString::FromAscii("&Windows"));
  menuBar->Append(menuHelp, wxString::FromAscii("&Help"));

  SetMenuBar(menuBar);

  SetStatusText(wxString::FromAscii("Default configuration loaded."));

  mem = new Memory();
  nul = new NullPeripheral();
  bpm = new BreakpointManager();

  cpu.SetBreakpointManager(bpm);

  wxPanel * panel = new wxPanel(this);

  areg = new ByteRegister(&A);
  xreg = new ByteRegister(&X);
  yreg = new ByteRegister(&Y);
  sreg = new ByteRegister(&S);
  preg = new ByteRegister(&P);
  pcreg = new WordRegister(&emPC);

  awin = new ByteRegisterWindow(panel, "A", areg);
  xwin = new ByteRegisterWindow(panel, "X", xreg);
  ywin = new ByteRegisterWindow(panel, "Y", yreg);
  swin = new ByteRegisterWindow(panel, "S", sreg);
  pwin = new ByteRegisterWindow(panel, "P", preg);
  pcwin = new WordRegisterWindow(panel, "PC", pcreg);

  pwin->Constrain(0x20, 0x00);

  stwin = new StatusWindow(panel, &N, &V, &B, &D, &I, &Z, &C);
  stkwin = new StackWindow(panel);
  diswin = new DisasmWindow(panel, mem, bpm);
  bpwin = new BreakpointWindow(panel, bpm);

  //memwin = new MemWindow(panel, mem);

  btrun = new wxButton(panel, Butt_Run, wxString::FromAscii("Run"), wxDefaultPosition);
  btstop = new wxButton(panel, Butt_Stop, wxString::FromAscii("Stop"), wxDefaultPosition);
  btstep = new wxButton(panel, Butt_Step, wxString::FromAscii("Step"), wxDefaultPosition);
  bttrace = new wxButton(panel, Butt_Trace, wxString::FromAscii("Trace"), wxDefaultPosition);
  btmstep = new wxButton(panel, Butt_MStep, wxString::FromAscii("Multistep"), wxDefaultPosition);
  btstepover = new wxButton(panel, Butt_StepOver, wxString::FromAscii("Step Over"), wxDefaultPosition);

  mem->AddListener(diswin);
  mem->AddListener(stkwin);

  preg->AddListener(stwin);
  sreg->AddListener(stkwin);
  pcreg->AddListener(diswin);
  stwin->AddListener(pwin);

  bpm->AddBreakpointListener(bpwin);
  bpm->AddBreakpointListener(diswin);

  wxBoxSizer * top_sizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * bot_sizer = new wxBoxSizer(wxHORIZONTAL);

  wxBoxSizer * vsizer = new wxBoxSizer(wxVERTICAL);

  wxStaticBox * reg_box = new wxStaticBox(panel, -1, wxString::FromAscii("Registers"));
  wxSizer * reg_sizer = new wxStaticBoxSizer(reg_box, wxVERTICAL);
  reg_sizer->Add(awin, 1, wxALL | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(xwin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(ywin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(swin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(pwin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(pcwin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(stwin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());

  wxStaticBox * stk_box = new wxStaticBox(panel, -1, wxString::FromAscii("Stack"));
  wxSizer * stk_sizer = new wxStaticBoxSizer(stk_box, wxVERTICAL);
  stk_sizer->Add(stkwin, 1, wxALL, prefs->BorderWidth());

  wxStaticBox * bp_box = new wxStaticBox(panel, -1, wxString::FromAscii("Breakpoints"));
  wxSizer * bp_sizer = new wxStaticBoxSizer(bp_box, wxVERTICAL);
  bp_sizer->Add(bpwin, 1, wxALL, prefs->BorderWidth());

  wxStaticBox * dis_box = new wxStaticBox(panel, -1, wxString::FromAscii("Disassembly"));
  wxSizer * dis_sizer = new wxStaticBoxSizer(dis_box, wxVERTICAL);
  dis_sizer->Add(diswin, 1, wxALL, prefs->BorderWidth());

  top_sizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  top_sizer->Add(reg_sizer, 0, 0, prefs->BorderWidth());
  top_sizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  top_sizer->Add(stk_sizer, 0, wxEXPAND, prefs->BorderWidth());
  top_sizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  top_sizer->Add(bp_sizer, 0, wxEXPAND, prefs->BorderWidth());
  top_sizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  top_sizer->Add(dis_sizer, 0, wxEXPAND, prefs->BorderWidth());
  top_sizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer

  //wxStaticBox * mem_box = new wxStaticBox(panel, -1, wxString::FromAscii("Memory"));
  //wxSizer * mem_sizer = new wxStaticBoxSizer(mem_box, wxVERTICAL);
  //mem_sizer->Add(memwin, 0, wxALL, prefs->BorderWidth());
  wxStaticBoxSizer * mem_sizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Memory");
  memwin = new MemWindow(mem_sizer->GetStaticBox(), mem);
  mem_sizer->Add(memwin, 0, wxALL, prefs->BorderWidth());

  wxBoxSizer * butt_sizer = new wxBoxSizer(wxVERTICAL);
  butt_sizer->Add(btrun, 0, wxALL, prefs->BorderWidth());
  butt_sizer->Add(btstep, 0, wxLEFT | wxRIGHT | wxBOTTOM, prefs->BorderWidth());
  butt_sizer->Add(btstepover, 0, wxLEFT | wxRIGHT | wxBOTTOM, prefs->BorderWidth());
  butt_sizer->Add(btmstep, 0, wxLEFT | wxRIGHT | wxBOTTOM, prefs->BorderWidth());
  butt_sizer->Add(bttrace, 0, wxLEFT | wxRIGHT | wxBOTTOM, prefs->BorderWidth());
  butt_sizer->Add(btstop, 0, wxLEFT | wxRIGHT | wxBOTTOM, prefs->BorderWidth());

  bot_sizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  bot_sizer->Add(mem_sizer, 0, 0, 0);
  bot_sizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  bot_sizer->Add(butt_sizer, 0, wxALIGN_CENTRE, 0);
  bot_sizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer

  vsizer->Add(0, prefs->BorderWidth(), 0, 0); // spacer
  vsizer->Add(top_sizer, 0, 0, 0);
  vsizer->Add(0, prefs->BorderWidth(), 0, 0); // spacer
  vsizer->Add(bot_sizer, 0, 0, 0);
  vsizer->Add(0, prefs->BorderWidth(), 0, 0); // spacer

  vsizer->Add(0, wxSystemSettings::GetMetric(wxSYS_CAPTION_Y), 0, 0); // spacer

  panel->SetSizer(vsizer);
  vsizer->SetSizeHints(panel);

  wxBoxSizer * panel_sizer = new wxBoxSizer(wxVERTICAL);
  panel_sizer->Add(panel, 0, 0, 0);

  this->SetSizer(panel_sizer);
  panel_sizer->SetSizeHints(this);

  mem->init_ram_memory_read(0, 0xFFFF, 0);
  mem->init_ram_memory_write(0, 0xFFFF, 0);

  // initialize all of address space to RAM
  ap_reader_t mem_reader = mem->GetReader(0);
  ap_writer_t mem_writer = mem->GetWriter(0);

  for (int idx = 0; idx <= 0xFFFF; ++idx)
    {
      ap_reader[idx] = mem_reader;
      ap_writer[idx] = mem_writer;
    }

#if 1
  // 6551 serial port
  Acia6551 * ser = new Acia6551();
  ap[num_aps++] = ser;

  Acia6551Frame * ser_frame = new Acia6551Frame("Acia 6551", ser);
  frame[numwmitems] = ser_frame;
  wmchecked[numwmitems] = false;
  winmenu->AppendCheckItem(Win_First + numwmitems, "Acia 6551");
  ++numwmitems;

  cpu.AddClockCustomer(ser);

  // data
  ap_reader[0xC080] = ser->GetReader(0);
  ap_writer[0xC080] = ser->GetWriter(0);

  // status
  ap_reader[0xC081] = ser->GetReader(1);
  ap_writer[0xC081] = ser->GetWriter(1);

  // command
  ap_reader[0xC082] = ser->GetReader(2);
  ap_writer[0xC082] = ser->GetWriter(2);

  // control
  ap_reader[0xC083] = ser->GetReader(3);
  ap_writer[0xC083] = ser->GetWriter(3);
#endif

#if 1
  AppleTextFrame * textFrame = new AppleTextFrame("40-Col Text Display");
  frame[numwmitems] = textFrame;
  wmchecked[numwmitems] = false;
  winmenu->AppendCheckItem(Win_First + numwmitems, "40-Col Display");
  ++numwmitems;

  ap_reader[0xC000] = textFrame->GetReader(0);
  ap_writer[0xC000] = textFrame->GetWriter(0);
  
  ap_reader[0xC010] = textFrame->GetReader(1);
  ap_writer[0xC010] = textFrame->GetWriter(1);
  
  for (unsigned short ad = 0x400; ad < 0x800; ad++)
    {
      ap_reader[ad] = textFrame->GetReader(2);
      ap_writer[ad] = textFrame->GetWriter(2);
    }

#endif
}

SimFrame::~SimFrame ()
{
/*
  delete areg;
  delete xreg;
  delete yreg;
  delete sreg;
  delete preg;
  delete pcreg;
  delete nul;
  delete bpm;
  delete mem;

  for (unsigned int idx = 0; idx < num_aps; ++idx)
    {
      delete ap[idx];
      frame[idx]->Destroy();
    }
*/
}

void SimFrame::OnWinMenu (wxCommandEvent& event)
{
  int which = event.GetId();

  bool checked = wmchecked[which - Win_First] ? false : true;
  wmchecked[which - Win_First] = checked;

  winmenu->Check(which, checked);
  frame[which - Win_First]->Show(checked);
}

void SimFrame::OnExit (wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

void SimFrame::OnClose (wxCloseEvent& WXUNUSED(event))
{
  unsigned int i;
  for (i = 0; i < numwmitems; i++) frame[i]->Destroy();
  Destroy();
}

void SimFrame::OnNew (wxCommandEvent& WXUNUSED(event))
{
}

void SimFrame::OnOpen (wxCommandEvent& WXUNUSED(event))
{
}

void SimFrame::OnLoadBinary (wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog * fd = new wxFileDialog(this,
				       wxString::FromAscii("Choose a binary image"),
				       wxString::FromAscii(""),
				       wxString::FromAscii(""),
				       wxString::FromAscii("*.*"),
				       wxFD_OPEN | wxFD_CHANGE_DIR);

  int result = fd->ShowModal();

  if (result == wxID_OK)
    {
      wxString pstr = fd->GetPath();
      const wxCharBuffer mbb = pstr.mb_str();
      const char * name = mbb.data();

      AddressDialog ad(this);

      if (ad.ShowModal() == wxID_OK)
	{
	  wxString astr = ad.GetValue();
	  const wxCharBuffer mba = astr.mb_str();
	  const char * atxt = mba.data();

	  int addr;
	  sscanf(atxt, "%x", &addr);

	  unsigned char buf[256];
	  int fd = open(name, O_RDONLY);
	  wxFile ifile(fd);

	  if (ifile.IsOpened() == false)
	    {
	      int nlen = strlen(name);
	      char * msg = new char[nlen + 80];
	      if (msg != NULL)
		{
		  sprintf(msg, "Couldn't open file\n%s\nfor reading.", name);
		  wxMessageDialog dlg(this,
				      wxString::FromAscii(msg),
				      wxString::FromAscii("Load Binary Image"));
		  dlg.ShowModal();
		  delete[] msg;
		}
	    }
	  else
	    {
	      int idx = 0;
	      int loc = addr;
	      off_t count = 1;
	      off_t total = 0;

	      while (count > 0)
		{
		  if (addr == 0x10000)
		    break;

		  count = ifile.Read(buf, 256);
		  if (count > 0)
		    total += count;

		  idx = 0;
		  while (idx < count)
		    {
		      if (mem->IsReadable(addr))
			{
			  if (mem->IsRom(addr))
			    mem->LoadToRom(addr, buf[idx]);
			  else
			    WRITE(addr, buf[idx]);
			}

		      ++idx;
		      if (++addr == 0x10000)
			break;
		    }
		}

	      memwin->ForceRefresh();
	      stkwin->ForceRefresh();
	      diswin->ForceRefresh();

	      if ((addr == 0x10000) && ((idx != count) || (ifile.Read(buf, 1) == 1)))
		{
		  wxMessageDialog dlg(this,
				      wxString::FromAscii("File was too long for memory.\n"
							  "Loading stopped at 0xFFFF."),
				      wxString::FromAscii("Load Binary Image"));
		  dlg.ShowModal();
		}
	      else
		{
		  char foo[64];
		  sprintf(foo, "Loaded %d bytes at %4.4x", (int)total, loc);
		  SetStatusText(wxString::FromAscii(foo));
		}
	    }
	}
    }

  fd->Destroy();
}

void SimFrame::load_srecord (struct srecord * srec, unsigned long * total)
{
  int idx = 0;
  int addr = srec->address;
  int count = srec->length;
  unsigned long mytotal = 0;

  if (addr >= 0x10000)
    {
      /*
      wxLogError("S%d record specifies an out-of-range address of 0x%8.8lx\n",
		 srec->type, srec->address);
      */
      return;
    }

  for (idx = 0; (idx < count && addr < 0x10000); ++idx)
    {
      if (mem->IsReadable(addr))
	{
	  ++mytotal;

	  if (mem->IsRom(addr))
	    mem->LoadToRom(addr, srec->data[idx]);
	  else
	    WRITE(addr, srec->data[idx]);

	  ++addr;
	}
    }

  *total = *total + mytotal;
}

void SimFrame::OnLoadSRecords (wxCommandEvent& WXUNUSED(event))
{
  wxFileDialog * fd = new wxFileDialog(this,
					wxString::FromAscii("Choose an S-Record file"),
					wxString::FromAscii(""),
					wxString::FromAscii(""),
					wxString::FromAscii("*.*"),
				       wxFD_OPEN | wxFD_CHANGE_DIR);

  int result = fd->ShowModal();

  if (result == wxID_OK)
    {
      wxString pstr = fd->GetPath();
      const wxCharBuffer mbb = pstr.mb_str();
      const char * name = mbb.data();

      FILE * f = fopen(name, "rb");
      if (f == NULL)
	{
	  int nlen = strlen(name);
	  char * msg = new char[nlen + 80];
	  if (msg != NULL)
	    {
	      sprintf(msg, "Couldn't open file\n%s\nfor reading.", name);
	      wxMessageDialog dlg(this,
				  wxString::FromAscii(msg),
				  wxString::FromAscii("Load S-Records"));
	      dlg.ShowModal();
	      delete[] msg;
	    }	  
	}
      else
	{
	  int line = 1;
	  int type = 0;
	  int result;
	  int remarks;
	  unsigned numrecords = 0;
	  unsigned long total = 0;
	  unsigned short entry;
	  struct srecord srec;

	  result = read_srecord(f, &srec, &remarks);

	  while (result != srec_result_eof)
	    {
		/*
	      if (remarks != 0)
		{
		  if (remarks & remark_unknown_srecord)
		    wxLogWarning("Unknown type of S-Record encountered at line %d\n", line);

		  if (remarks & remark_spurious_character)
		    wxLogWarning("Spurious characters encountered in S-Record at line %d\n", line);
		}
		*/

	      if (result == srec_result_ok)
		{
		  switch (srec.type)
		    {
		    case srec_type_s0:
		      numrecords = 0;
		      break;

		    case srec_type_s1:
		    case srec_type_s2:
		    case srec_type_s3:
			/*
		      if (type > 0 && type != srec.type)
			{
			  wxLogWarning("S%d record follows S%d record without "
					"intervening termination record.\n",
				       srec.type, type);
			}
			*/

		      load_srecord(&srec, &total);
		      type = srec.type;
		      ++numrecords;
		      break;

		    case srec_type_s5:
			/*
		      if (numrecords != srec.address)
			{
			  wxLogWarning("S5 record specifies %lu preceding records, but there were actually %u records.\n",
				       srec.address, numrecords);
			}
			*/
		      break;

		    case srec_type_s7:
			/*
		      if (type != srec_type_s3)
			{
			  wxLogWarning("S7 record encountered after S%d record(s)\n", type);
			}
			*/

		      entry = srec.address;
		      type = 0;
		      break;
		      
		    case srec_type_s8:
			/*
		      if (type != srec_type_s2)
			{
			  wxLogWarning("S8 record encountered after S%d record(s)\n", type);
			}
			*/

		      entry = srec.address;
		      type = 0;
		      break;
		      
		    case srec_type_s9:
			/*
		      if (type != srec_type_s1)
			{
			  wxLogWarning("S9 record encountered after S%d record(s)\n", type);
			}
			*/

		      entry = srec.address;
		      type = 0;
		      break;
		    }

		  free(srec.buf);
		}
	      else
		{
		/*
		  switch (result)
		    {
		    case srec_result_eol:
		      wxLogWarning("Unexpected end of line encountered at line %d\n", line);
		      break;

		    case srec_result_bad_arg:
		      wxLogError("Internal error (bad argument)\n");
		      break;

		    case srec_result_overflow:
		      wxLogWarning("Malformed S-Record encountered at line %d (more data than length)\n", line);
		      break;

		    case srec_result_malloc:
		      wxLogError("Internal error (can't allocate memory)\n");
		      break;

		    case srec_result_checksum:
		      wxLogWarning("Malformed S-Record encountered at line %d (bad checksum)\n", line);
		      break;

		    default:
		      break;
		    }
		*/
		}

	      ++line;
	      result = read_srecord(f, &srec, &remarks);
	    }

	  if (total > 0)
	    {
	      char foo[64];

	      memwin->ForceRefresh();
	      stkwin->ForceRefresh();
	      diswin->ForceRefresh();

	      sprintf(foo, "Loaded %lu bytes", total);
	      SetStatusText(wxString::FromAscii(foo));
	    }
	}
    }

  fd->Destroy();
}

void SimFrame::OnHelp (wxCommandEvent& WXUNUSED(event))
{
}

void SimFrame::OnAbout (wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(wxString::FromAscii("Sim65 (development)\n"
				   "Copyright 2000-2003 William Sheldon Simms III"),
               wxString::FromAscii("About Sim65"),
	       wxOK | wxICON_INFORMATION);
}

void SimFrame::OnBPEnableAll (wxCommandEvent& event)
{
  bpm->EnableAll();
}

void SimFrame::OnBPDisableAll (wxCommandEvent& event)
{
  bpm->DisableAll();
}

void SimFrame::OnBPDeleteAll (wxCommandEvent& event)
{
  bpm->DeleteAll();
}

void SimFrame::OnRun (wxCommandEvent& WXUNUSED(event))
{
  if (running == false && stepping == false)
    {
      running = true;
      mem->SetRunning(true);
      cpu.ResetNumExecuted();
      SetStatusText(wxString::FromAscii("Running."));
    }
}

void SimFrame::OnStop (wxCommandEvent& WXUNUSED(event))
{
  running = false;
  stepping = false;
  mem->SetRunning(false);
}

void SimFrame::OnStep (wxCommandEvent& WXUNUSED(event))
{
  if (running == false && stepping == false)
    {
      // printf("PC = %4.4x\n", emPC);

      cpu.Execute(1);

      awin->Refresh();
      xwin->Refresh();
      ywin->Refresh();
      swin->Refresh();
      pwin->Refresh();
      pcwin->Refresh();
      stwin->Refresh();
      memwin->ForceRefresh();
      stkwin->ForceRefresh();
      diswin->ForceRefresh();

      SetStatusText(wxString::FromAscii("Executed one instruction."));
    }
}

void SimFrame::OnStepOver (wxCommandEvent& event)
{
  if (running == false && stepping == false)
    {
      unsigned char opcode;
      opcode = READ(emPC);

      if (opcode == 0x20) /* JSR */
	{
	  running = true;
	  mem->SetRunning(true);
	  bpm->AddBreakpoint(Breakpoint(bp_S | bp_equal | bp_automatic, S));
	  cpu.ResetNumExecuted();
	  SetStatusText(wxString::FromAscii("Stepping Over..."));
	}
      else
	{
	  OnStep(event);
	}
    }
}

void SimFrame::OnMStep (wxCommandEvent& WXUNUSED(event))
{
  if (running == false && stepping == false)
    {
      stepping = true;
      cpu.ResetNumExecuted();
      SetStatusText(wxString::FromAscii("Stepping."));
    }
}

void SimFrame::OnTrace (wxCommandEvent& WXUNUSED(event))
{
  SetStatusText(wxString::FromAscii("Tracing not implemented yet."));
}

void SimFrame::OnIdle (wxIdleEvent& event)
{
  static bool last_running = false;
  static bool last_stepping = false;

#define RUN_STEP 16384

  if (running || stepping)
    {
      bool hit_breakpoint = false;

      if (running)
	{
	  hit_breakpoint = cpu.Execute(RUN_STEP);

	  if (hit_breakpoint)
	    {
	      last_running = true;
	      running = false;
	      mem->SetRunning(false);
	    }
	}
      else if (stepping)
	{
	  hit_breakpoint = cpu.Execute(1);

	  if (hit_breakpoint)
	    {
	      last_stepping = true;
	      stepping = false;
	    }

	  awin->Refresh();
	  xwin->Refresh();
	  ywin->Refresh();
	  swin->Refresh();
	  pwin->Refresh();
	  pcwin->Refresh();
	  stwin->Refresh();
	  memwin->ForceRefresh();
	  stkwin->ForceRefresh();
	  diswin->ForceRefresh();
	}

      event.RequestMore();
    }

  if (!running && !stepping)
    {
      if (last_running || last_stepping)
	{
	  char foo [80];

	  awin->Refresh();
	  xwin->Refresh();
	  ywin->Refresh();
	  swin->Refresh();
	  pwin->Refresh();
	  pcwin->Refresh();
	  stwin->Refresh();
	  memwin->ForceRefresh();
	  stkwin->ForceRefresh();
	  diswin->ForceRefresh();

	  sprintf(foo, "Stopped. Executed %lld instructions.", cpu.GetNumExecuted());
	  SetStatusText(wxString::FromAscii(foo));
	}
    }

  last_running = running;
  last_stepping = stepping;
}

void SimFrame::OnReset (wxCommandEvent& WXUNUSED(event))
{
  interrupt(RES_VECTOR, F_RESET);

  awin->Refresh();
  xwin->Refresh();
  ywin->Refresh();
  swin->Refresh();
  pwin->Refresh();
  pcwin->Refresh();
  stwin->Refresh();
  memwin->ForceRefresh();
  stkwin->ForceRefresh();
  diswin->ForceRefresh();

  SetStatusText(wxString::FromAscii("Reset CPU."));
}

#if 0
void SimFrame::OnPaint (wxPaintEvent& event)
{
  wxPaintDC dc(this);
  dc.SetBrush(*wxMEDIUM_GREY_BRUSH);
  dc.BeginDrawing();
  dc.Clear();
  dc.EndDrawing();
}
#endif

// end
