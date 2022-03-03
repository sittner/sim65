//
// AppleTextFrame.h
// Copyright © 2020 William Sheldon Simms
//

#ifndef INCLUDED_APPLETEXTFRAME_H
#define INCLUDED_APPLETEXTFRAME_H

#include "wx/wx.h"
#include "AddressPeripheral.h"

class ATPanel: public wxPanel {
public:
  ATPanel(wxWindow *parent);
  ~ATPanel();
  void OnChar (wxKeyEvent& event);

  unsigned char keybit;
  unsigned char lastkey;
  
  DECLARE_EVENT_TABLE()  
};

class AppleTextFrame: public wxFrame, public AddressPeripheral
{
public:
  AppleTextFrame(const wxString& title);
  ~AppleTextFrame();

  void OnMouseLeftDown (wxMouseEvent& event);
  void OnExit (wxCommandEvent&);
  void OnClose (wxCloseEvent&);
  void OnPaint (wxPaintEvent& event);

  unsigned int NumReaders () { return 3; }
  unsigned int NumWriters () { return 3; }

  ap_reader_t GetReader (unsigned int idx);
  ap_writer_t GetWriter (unsigned int idx);
  
  unsigned char read_key (unsigned short addr);
  unsigned char read_strobe (unsigned short addr);
  unsigned char read_appletext (unsigned short addr);
  
  void write_key (unsigned short addr, unsigned char byte);
  void write_strobe (unsigned short addr, unsigned char byte);
  void write_appletext (unsigned short addr, unsigned char byte);
  
private:
  ATPanel * panel;
  unsigned short page;
  char mem[0x800];
  wxTextCtrl * display;

  DECLARE_EVENT_TABLE()  
};

#endif /* INCLUDED_APPLETEXTFRAME_H */
