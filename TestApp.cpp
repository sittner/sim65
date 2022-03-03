// TestApp

#include "wx/wx.h"

class Label: public wxWindow
{
public:
  Label (wxWindow * parent, char * text);
  void OnPaint (wxPaintEvent& event);

private:
  int cwidth;
  int cheight;
  char * text;

  DECLARE_EVENT_TABLE()
};

class ByteWindow: public wxWindow
{
public:
  ByteWindow (wxWindow * parent, unsigned char * valptr);

  void OnPaint (wxPaintEvent& event);

private:
  int cwidth;
  int cheight;
  unsigned char * myval;

  DECLARE_EVENT_TABLE()
};

class LabeledByteWindow: public wxBoxSizer
{
public:
  LabeledByteWindow (wxWindow * parent, char * label, unsigned char * valptr);

private:
  ByteWindow * bwin;
};

class TestFrame: public wxFrame
{
public:
  TestFrame();
  void OnExit (wxCommandEvent& event);

private:
  unsigned char val_a;
  unsigned char val_b;

  DECLARE_EVENT_TABLE()
};

class TestApp: public wxApp
{
  virtual bool OnInit();

  TestFrame * frame;
};

// Label

BEGIN_EVENT_TABLE(Label, wxWindow)
EVT_PAINT(Label::OnPaint)
END_EVENT_TABLE()

Label::Label (wxWindow * parent, char * text)
  : wxWindow(parent, -1)
{
  this->text = text;

  SetFont(wxFont(10, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

  int w, h, d;
  dc.GetTextExtent(text, &w, &h, &d);

  wxSize csz(w, h+d);
  SetClientSize(csz);
  SetMinClientSize(csz);
}

void Label::OnPaint (wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  dc.SetFont(GetFont());
  dc.DrawText(text, 0, 0);
}

// Byte Window

BEGIN_EVENT_TABLE(ByteWindow, wxWindow)
EVT_PAINT(ByteWindow::OnPaint)
END_EVENT_TABLE()

ByteWindow::ByteWindow (wxWindow * parent, unsigned char * valptr)
  : wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
  SetThemeEnabled(FALSE);
  SetBackgroundColour(*wxWHITE);

  SetFont(wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  myval = valptr;

  wxClientDC dc(this);
  dc.SetFont(GetFont());

  cwidth = dc.GetCharWidth();
  cheight = dc.GetCharHeight();

  SetClientSize(4 * cwidth, 3 * cheight / 2);
}

void ByteWindow::OnPaint (wxPaintEvent& WXUNUSED(event))
{
  char valtext[4];

  sprintf(valtext, "%2.2x", *myval);

  wxPaintDC dc(this);
  dc.SetFont(GetFont());
  dc.DrawText(valtext, cwidth, cheight / 4);
}

// Labeled Byte Window

LabeledByteWindow::LabeledByteWindow (wxWindow * parent, char * label, unsigned char * valptr)
  : wxBoxSizer(wxHORIZONTAL)
{
  bwin = new ByteWindow(parent, valptr);

  Add(new Label(parent, label), 0, wxCENTRE, 5);
  Add(bwin, 0, wxLEFT, 5);
}

// TestFrame

#define File_Exit 1

BEGIN_EVENT_TABLE(TestFrame, wxFrame)
EVT_MENU(File_Exit, TestFrame::OnExit)
END_EVENT_TABLE()

#define NORESIZE_FRAME (wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))

TestFrame::TestFrame ()
  : wxFrame((wxFrame *)NULL, -1, "TestFrame", wxPoint(-1,-1), wxSize(-1,-1), NORESIZE_FRAME)
{
  val_a = 0x55;
  val_b = 0xAA;

  wxMenu *menuFile = new wxMenu();
  menuFile->Append(File_Exit, "E&xit");
  wxMenuBar *menuBar = new wxMenuBar();
  menuBar->Append(menuFile, "&File");
  SetMenuBar(menuBar);

  wxPanel * panel = new wxPanel(this);

  LabeledByteWindow * awin = new LabeledByteWindow(panel, "A", &val_a);
  LabeledByteWindow * bwin = new LabeledByteWindow(panel, "B", &val_b);

  wxStaticBox * box = new wxStaticBox(panel, -1, "Info");
  wxSizer * info_sizer = new wxStaticBoxSizer(box, wxVERTICAL);
  info_sizer->Add(awin, 1, wxALL, 5);
  info_sizer->Add(bwin, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);

  panel->SetSizer(info_sizer);
  info_sizer->SetSizeHints(panel);

  wxBoxSizer * panel_sizer = new wxBoxSizer(wxVERTICAL);
  panel_sizer->Add(panel, 0, wxEXPAND, 0);

  this->SetSizer(panel_sizer);
  panel_sizer->SetSizeHints(this);
}

void TestFrame::OnExit (wxCommandEvent& WXUNUSED(event))
{
  Close(TRUE);
}

// Test App

IMPLEMENT_APP(TestApp)

bool TestApp::OnInit()
{
  frame = new TestFrame();
  frame->Show(TRUE);
  SetTopWindow(frame);
  return TRUE;
}
