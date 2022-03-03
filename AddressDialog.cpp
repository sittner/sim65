/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.cpp
// Purpose:     wxTextEntryDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: textdlgg.cpp,v 1.20 2002/06/23 13:51:32 JS Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "AddressDialog.h"
#include "wx/statline.h"

BEGIN_EVENT_TABLE(AddressDialog, wxDialog)
EVT_BUTTON(wxID_OK, AddressDialog::OnOK)
END_EVENT_TABLE()

AddressDialog::AddressDialog(wxWindow *parent)
  : wxDialog(parent, -1, wxString::FromAscii("Enter Load Address"), wxDefaultPosition,
	     wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
  m_value = wxString::FromAscii("");
  wxString message = wxString::FromAscii("At what address do you want to load the image?");

  wxBeginBusyCursor();

  m_textctrl = new wxTextCtrl(this, -1, m_value, wxDefaultPosition, wxDefaultSize, 0);
  m_textctrl->SetMaxLength(4);

#if 0 // doesn't seem to work anyway
  wxFont wfont = m_textctrl->GetDefaultStyle().GetFont();
  wxFont mfont(wfont.GetPointSize(), wxFONTFAMILY_MODERN, wfont.GetStyle(), wfont.GetWeight());
  m_textctrl->SetDefaultStyle(wxTextAttr(*wxBLACK, wxNullColour, mfont));
#endif

  wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

  topsizer->Add(CreateTextSizer(message), 0, wxALL, 10);
  topsizer->Add(m_textctrl, 1, wxLEFT | wxRIGHT | wxCENTRE, 15);
  topsizer->Add(new wxStaticLine(this, -1), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
  topsizer->Add(CreateButtonSizer(wxOK | wxCANCEL | wxCENTRE), 0, wxCENTRE | wxALL, 10);

#if 1
  wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST, &m_value);
  wxArrayString okchars;
  okchars.Add(wxT("0")); okchars.Add(wxT("1")); okchars.Add(wxT("2")); okchars.Add(wxT("3"));
  okchars.Add(wxT("4")); okchars.Add(wxT("5")); okchars.Add(wxT("6")); okchars.Add(wxT("7"));
  okchars.Add(wxT("8")); okchars.Add(wxT("9")); okchars.Add(wxT("A")); okchars.Add(wxT("B"));
  okchars.Add(wxT("C")); okchars.Add(wxT("D")); okchars.Add(wxT("E")); okchars.Add(wxT("F"));
  okchars.Add(wxT("a")); okchars.Add(wxT("b")); okchars.Add(wxT("c")); okchars.Add(wxT("d"));
  okchars.Add(wxT("e")); okchars.Add(wxT("f"));
  validator.SetIncludes(okchars);
  m_textctrl->SetValidator(validator);
#endif

  SetSizer(topsizer);

  topsizer->SetSizeHints(this);
  topsizer->Fit(this);

  Centre(wxBOTH);
  m_textctrl->SetFocus();

  wxEndBusyCursor();
}

void AddressDialog::OnOK (wxCommandEvent& WXUNUSED(event))
{
#if 1
  if (Validate() && TransferDataFromWindow()) 
    {
      EndModal( wxID_OK );
    }
#else
  m_value = m_textctrl->GetValue();
  EndModal(wxID_OK);
#endif
}
