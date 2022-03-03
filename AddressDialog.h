/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.h
// Purpose:     wxStatusBar class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: textdlgg.h,v 1.14 2002/08/31 11:29:12 GD Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_ADDRESSDIALOG_H
#define INCLUDED_ADDRESSDIALOG_H

#include "wx/wx.h"

// ----------------------------------------------------------------------------
// wxTextEntryDialog: a dialog with text control, [ok] and [cancel] buttons
// ----------------------------------------------------------------------------

class AddressDialog : public wxDialog
{
public:
  AddressDialog(wxWindow *parent);

  wxString GetValue() const { return m_value; }
  void OnOK (wxCommandEvent& event);

private:
  wxTextCtrl * m_textctrl;
  wxString m_value;

  DECLARE_EVENT_TABLE()
};

#endif // INCLUDED_ADDRESSDIALOG_H
