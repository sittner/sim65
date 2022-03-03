//
// AppleTextFrame.cpp
// Copyright © 2020 William Sheldon Simms
//

#include "AppleTextFrame.h"
#include "Prefs.h"

wxBEGIN_EVENT_TABLE(ATPanel, wxPanel)
EVT_CHAR(ATPanel::OnChar)
wxEND_EVENT_TABLE()

ATPanel::ATPanel(wxWindow *parent) : wxPanel(parent)
{
}

ATPanel::~ATPanel()
{
}

void ATPanel::OnChar (wxKeyEvent& event)
{
  int kcode = event.GetKeyCode();

  keybit  = 0x80;
  lastkey = kcode & 0x7F;
  //printf("key: %2.2x\n", lastkey);
}

wxBEGIN_EVENT_TABLE(AppleTextFrame, wxFrame)
EVT_LEFT_DOWN(AppleTextFrame::OnMouseLeftDown)
EVT_PAINT(AppleTextFrame::OnPaint)
EVT_CLOSE(AppleTextFrame::OnClose)
wxEND_EVENT_TABLE()

#define NORESIZE_FRAME (wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))

AppleTextFrame::~AppleTextFrame ()
{
}

AppleTextFrame::AppleTextFrame(const wxString& title)
  : wxFrame((wxFrame *)NULL, -1, title, wxDefaultPosition, wxDefaultSize, NORESIZE_FRAME)
{
  Prefs * prefs = Prefs::GetPrefs();
  wxFont font(prefs->WindowFontSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  //wxTextAttr attr(*wxBLACK, *wxWHITE, font);
  SetFont(font);

  wxSize dsize;
  dsize.Set(40 * GetCharWidth(), 24 * GetCharHeight());
  SetClientSize(dsize);
  SetMinClientSize(dsize);

  page = 0;
  for (int i = 0; i < 0x800; i++) mem[i] = 0x20;

  panel = new ATPanel(this);
}

void AppleTextFrame::OnMouseLeftDown (wxMouseEvent& event)
{
  this->SetFocus();
}

void AppleTextFrame::OnPaint(wxPaintEvent&  WXUNUSED(event))
{
  char rowtext[100];
  wxPaintDC dc(this);
  //PrepareDC(dc);
  dc.SetFont(GetFont());

  unsigned idx = 0;
  unsigned base = page ? 0x400 : 0;
  for (unsigned a = 0; a < 0x400; a++)
    {
      unsigned g = a & 0x7f;
      if (g >= 0x78) continue;            // ignore screen holes
      
      unsigned col = g % 40;
      if (col == 0) idx = 0;
      g = g / 40;                         // get the group # (0,1,2)
      
      char b = mem[base + a];
      b &= 0x7f;
      
      if (b < 0x20) rowtext[idx++] = b + 0x40;
      else if (b != 0x60) rowtext[idx++] = b;
      else {
	rowtext[idx++] = 0xe2;
	rowtext[idx++] = 0x96; /* medium stipple */
	rowtext[idx++] = 0x92;
      }
      
      unsigned row = 8*g + a/0x80;
      if (col == 39) {
	rowtext[idx] = 0;
	dc.SetTextForeground(*wxBLACK);
	dc.DrawText(wxString::FromUTF8(rowtext), 0, row * GetCharHeight());
      }	
    }
}

void AppleTextFrame::OnExit (wxCommandEvent& event)
{
  Close(true);
}

void AppleTextFrame::OnClose (wxCloseEvent& event)
{
  /*
  if (event.CanVeto())
    event.Veto();
  else
  */
    Destroy();
}


ap_reader_t AppleTextFrame::GetReader (unsigned int idx)
{
  assert(idx < 3);

  ap_reader_t reader;
  reader.ap = this;

  switch (idx) {
  case 0: reader.read = (ap_rfunc_t)&AppleTextFrame::read_key; break;
  case 1: reader.read = (ap_rfunc_t)&AppleTextFrame::read_strobe; break;
  case 2: reader.read = (ap_rfunc_t)&AppleTextFrame::read_appletext; break;
  }

  return reader;
}

ap_writer_t AppleTextFrame::GetWriter (unsigned int idx)
{
  assert(idx < 3);

  ap_writer_t writer;
  writer.ap = this;

  switch (idx) {
  case 0: writer.write = (ap_wfunc_t)&AppleTextFrame::write_key; break;
  case 1: writer.write = (ap_wfunc_t)&AppleTextFrame::write_strobe; break;
  case 2: writer.write = (ap_wfunc_t)&AppleTextFrame::write_appletext; break;
  }

  return writer;
}

unsigned char AppleTextFrame::read_appletext (unsigned short addr)
{
  addr -= 0x400;
  if (addr >= 0x800) return 0;
  return mem[addr];
}

void AppleTextFrame::write_appletext (unsigned short addr, unsigned char byte)
{
  addr = addr - 0x400;
  if (addr >= 0x800) return;          // text screen is from 0x400 to 0xC00
  
  mem[addr] = byte;                   // remember the byte for redraws, etc.
  
  if ((addr & 0x7F) >= 0x78) return;  // ignore screen holes
  
  int cw, ch;
  GetClientSize(&cw, &ch);
  Refresh();                 // refresh everything (for now)
}

unsigned char AppleTextFrame::read_key (unsigned short addr)
{
  return panel->keybit | panel->lastkey;
}

void AppleTextFrame::write_key (unsigned short addr, unsigned char byte)
{
}
  
unsigned char AppleTextFrame::read_strobe (unsigned short addr)
{
  panel->keybit = 0;
  return panel->keybit;
}

void AppleTextFrame::write_strobe (unsigned short addr, unsigned char byte)
{
  panel->keybit = 0;
}
  
