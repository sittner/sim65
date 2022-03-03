//
// Acia6551Frame.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cstdio>
#include "Acia6551Frame.h"
#include "Prefs.h"

BEGIN_EVENT_TABLE(Acia6551Frame, wxFrame)
EVT_CLOSE(Acia6551Frame::OnClose)
END_EVENT_TABLE()

#define NORESIZE_FRAME (wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))

Acia6551Frame::~Acia6551Frame ()
{
  delete cmdreg;
  delete ctlreg;
  delete stareg;
  delete txdreg;
  delete rxdreg;
}

Acia6551Frame::Acia6551Frame (const wxString& title, Acia6551 * acia)
  : wxFrame((wxFrame *)NULL, -1, title, wxDefaultPosition, wxDefaultSize, NORESIZE_FRAME)
{
  initialized = false;

  Prefs * prefs = Prefs::GetPrefs();
  wxFont font(prefs->WindowFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  wxTextAttr attr(*wxBLACK, *wxWHITE, font);

  wxPanel * panel = new wxPanel(this);

  cmd = ctl = sta = txd = rxd = 0;

  cmdreg = new ByteRegister(&cmd);
  ctlreg = new ByteRegister(&ctl);
  stareg = new ByteRegister(&sta);
  txdreg = new ByteRegister(&txd);
  rxdreg = new ByteRegister(&rxd);

  cmdwin = new ByteRegisterWindow(panel, "Command", cmdreg);
  ctlwin = new ByteRegisterWindow(panel, "Control", ctlreg);
  stawin = new ByteRegisterWindow(panel,  "Status", stareg);
  txdwin = new ByteRegisterWindow(panel, "Tx Data", txdreg);
  rxdwin = new ByteRegisterWindow(panel, "Rx Data", rxdreg);

  txenchk = new wxCheckBox(panel, -1, wxString::FromAscii("Tx Enabled"));
  rxenchk = new wxCheckBox(panel, -1, wxString::FromAscii("Rx Enabled"));
  txirqchk = new wxCheckBox(panel, -1, wxString::FromAscii("Tx Interrupt"));
  rxirqchk = new wxCheckBox(panel, -1, wxString::FromAscii("Rx Interrupt"));
  txirqenchk = new wxCheckBox(panel, -1, wxString::FromAscii("Tx Int. Enabled"));
  rxirqenchk = new wxCheckBox(panel, -1, wxString::FromAscii("Rx Int. Enabled"));

  wxBoxSizer * pmsizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * hsizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * vsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer * rsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer * cksizer = new wxBoxSizer(wxVERTICAL);

  wxStaticBox * txchk_box = new wxStaticBox(panel, -1, wxString::FromAscii("Tx Status"));
  wxSizer * txchk_sizer = new wxStaticBoxSizer(txchk_box, wxVERTICAL);

  txchk_sizer->Add(txenchk, 0, 0, 0);
  txchk_sizer->Add(txirqenchk, 0, 0, 0);
  txchk_sizer->Add(txirqchk, 0, 0, 0);

  wxStaticBox * rxchk_box = new wxStaticBox(panel, -1, wxString::FromAscii("Rx Status"));
  wxSizer * rxchk_sizer = new wxStaticBoxSizer(rxchk_box, wxVERTICAL);

  rxchk_sizer->Add(rxenchk, 0, 0, 0);
  rxchk_sizer->Add(rxirqenchk, 0, 0, 0);
  rxchk_sizer->Add(rxirqchk, 0, 0, 0);

  cksizer->Add(txchk_sizer, 0, wxBOTTOM, prefs->BorderWidth());
  cksizer->Add(rxchk_sizer, 0, 0, 0);

  wxStaticBox * reg_box = new wxStaticBox(panel, -1, wxString::FromAscii("ACIA Registers"));
  wxSizer * reg_sizer = new wxStaticBoxSizer(reg_box, wxVERTICAL);

  reg_sizer->Add(cmdwin, 0, wxALL | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(ctlwin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(stawin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(txdwin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());
  reg_sizer->Add(rxdwin, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, prefs->BorderWidth());

  term = new wxTextCtrl(panel, -1, wxString::FromAscii(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_CHARWRAP | wxTE_READONLY);
  term->SetFont(font);
  term->SetDefaultStyle(attr);
  term->SetSize(40 * term->GetCharWidth(), 2 * term->GetCharHeight());

  wxStaticBox * term_box = new wxStaticBox(panel, -1, wxString::FromAscii("Output"));
  wxSizer * term_sizer = new wxStaticBoxSizer(term_box, wxVERTICAL);
  term_sizer->Add(term, 1, wxALL | wxEXPAND, prefs->BorderWidth());

  txpm = new wxTextCtrl(panel, -1, wxString::FromAscii(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  txpm->SetFont(font);
  txpm->SetDefaultStyle(attr);

  wxSize txsz;
  txsz.Set(20 * txpm->GetCharWidth(), 3 * txpm->GetCharHeight() / 2);
  txpm->SetClientSize(txsz);
  txpm->SetMinClientSize(txsz);

  wxStaticBox * txpm_box = new wxStaticBox(panel, -1, wxString::FromAscii("Tx Params"));
  wxSizer * txpm_sizer = new wxStaticBoxSizer(txpm_box, wxVERTICAL);
  txpm_sizer->Add(txpm, 1, wxALL, prefs->BorderWidth());

  rxpm = new wxTextCtrl(panel, -1, wxString::FromAscii(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  rxpm->SetFont(font);
  rxpm->SetDefaultStyle(attr);

  wxSize rxsz;
  rxsz.Set(20 * rxpm->GetCharWidth(), 3 * rxpm->GetCharHeight() / 2);
  rxpm->SetClientSize(rxsz);
  rxpm->SetMinClientSize(rxsz);

  wxStaticBox * rxpm_box = new wxStaticBox(panel, -1, wxString::FromAscii("Rx Params"));
  wxSizer * rxpm_sizer = new wxStaticBoxSizer(rxpm_box, wxVERTICAL);
  rxpm_sizer->Add(rxpm, 1, wxALL, prefs->BorderWidth());

  pmsizer->Add(txpm_sizer, 0, wxBOTTOM, prefs->BorderWidth());
  pmsizer->Add(prefs->BorderWidth(), 0 ,0 ,0);
  pmsizer->Add(rxpm_sizer, 0, wxBOTTOM, prefs->BorderWidth());

  rsizer->Add(prefs->BorderWidth(), 0 ,0 ,0);
  rsizer->Add(pmsizer, 0, 0, 0);
  rsizer->Add(prefs->BorderWidth(), 0 ,0 ,0);
  rsizer->Add(term_sizer, 1, wxEXPAND, 0);
  rsizer->Add(prefs->BorderWidth(), 0 ,0 ,0);

  hsizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  hsizer->Add(reg_sizer, 0, wxEXPAND, 0);
  hsizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  hsizer->Add(rsizer, 0, wxEXPAND, 0);
  hsizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer
  hsizer->Add(cksizer, 0, wxEXPAND, 0);
  hsizer->Add(2*prefs->BorderWidth(), 0, 0, 0); // spacer

  vsizer->Add(0, prefs->BorderWidth(), 0, 0); // spacer
  vsizer->Add(hsizer, 0, 0, 0);
  vsizer->Add(0, prefs->BorderWidth(), 0, 0); // spacer

  panel->SetSizer(vsizer);
  vsizer->SetSizeHints(panel);

  wxBoxSizer * panel_sizer = new wxBoxSizer(wxVERTICAL);
  panel_sizer->Add(panel, 0, 0, 0);

  this->SetSizer(panel_sizer);
  panel_sizer->SetSizeHints(this);

  acia->AddAcia6551Listener(this);
  acia->AddUartListener(this);

  initialized = true;
}

void Acia6551Frame::TellIrq (bool tx, bool rx)
{
  txirqchk->SetValue(tx);
  rxirqchk->SetValue(rx);
}

void Acia6551Frame::TellEnabled (enable_t tx, enable_t rx)
{
  txenchk->SetValue(tx == ENABLED ? true : false);
  rxenchk->SetValue(rx == ENABLED ? true : false);
}

void Acia6551Frame::TellIrqEnabled (bool irq, enable_t tx, enable_t rx)
{
  txirqenchk->SetValue((irq && tx == ENABLED) ? true : false);
  rxirqenchk->SetValue((irq && rx == ENABLED) ? true : false);
}

void Acia6551Frame::OnExit (wxCommandEvent& event)
{
  Close(true);
}

void Acia6551Frame::OnClose (wxCloseEvent& event)
{
  /*
  if (event.CanVeto())
    event.Veto();
  else
  */
    Destroy();
}

void Acia6551Frame::SetParams ()
{
  char txt[40];
  char pchar[] = "NOEMS";

  if (txsbs & 1)
    if (txbps % 1000)
      sprintf(txt, " %d%c%d.d / %d.%d bps", txdbs, pchar[txpar], txsbs/2, 5*(txsbs&1), (txsbs%1000)/10);
    else
      sprintf(txt, " %d%c%d.d / %d bps", txdbs, pchar[txpar], txsbs/2, 5*(txsbs&1));
  else
    if (txbps % 1000)
      sprintf(txt, " %d%c%d / %lu.%lu bps", txdbs, pchar[txpar], txsbs/2, txbps/1000, (txbps%1000)/10);
    else
      sprintf(txt, " %d%c%d / %lu bps", txdbs, pchar[txpar], txsbs/2, txbps/1000);

  txpm->SetValue(wxString::FromAscii(txt));

  if (rxsbs & 1)
    if (rxbps % 1000)
      sprintf(txt, " %d%c%d.d / %d.%d bps", rxdbs, pchar[rxpar], rxsbs/2, 5*(rxsbs&1), (rxsbs%1000)/10);
    else
      sprintf(txt, " %d%c%d.d / %d bps", rxdbs, pchar[rxpar], rxsbs/2, 5*(rxsbs&1));
  else
    if (rxbps % 1000)
      sprintf(txt, " %d%c%d / %lu.%lu bps", rxdbs, pchar[rxpar], rxsbs/2, rxbps/1000, (rxbps%1000)/10);
    else
      sprintf(txt, " %d%c%d / %lu bps", rxdbs, pchar[rxpar], rxsbs/2, rxbps/1000);

  rxpm->SetValue(wxString::FromAscii(txt));
}

void Acia6551Frame::TxByte (unsigned char byte)
{
  char txt[16];
  sprintf(txt, "[%c/0x%2.2x]", byte, byte);
  *term << wxString::FromAscii(txt);
}
