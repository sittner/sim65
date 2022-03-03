//
// Acia6551Frame.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_ACIA6551FRAME_H
#define INCLUDED_ACIA6551FRAME_H

#include "wx/wx.h"
#include "ByteRegister.h"
#include "RegisterWindow.h"
#include "Acia6551.h"
#include "UartListener.h"
#include "Acia6551Listener.h"

class Acia6551Frame: public wxFrame, public UartListener, public Acia6551Listener
{
public:
  Acia6551Frame (const wxString& title, Acia6551 * acia);
  ~Acia6551Frame();

  void OnExit (wxCommandEvent&);
  void OnClose (wxCloseEvent&);

  // Acia6551Listener interface
  void TellCmd (unsigned char ncmd) { cmd = ncmd; cmdwin->Refresh(); }
  void TellCtl (unsigned char nctl) { ctl = nctl; ctlwin->Refresh(); }
  void TellSta (unsigned char nsta) { sta = nsta; stawin->Refresh(); }
  void TellTxd (unsigned char ntxd) { txd = ntxd; txdwin->Refresh(); }
  void TellRxd (unsigned char nrxd) { rxd = nrxd; rxdwin->Refresh(); }

  // UartListener interface
  void TxBit  (unsigned long long time, unsigned char level) {}
  void TxByte (unsigned char byte);

  void TellBaud     (unsigned long tx, unsigned long rx) { txbps = tx; rxbps = rx; SetParams(); }
  void TellParity   (parity_t tx, parity_t rx)           { txpar = tx; rxpar = rx; SetParams(); }
  void TellDataBits (unsigned int tx, unsigned int rx)   { txdbs = tx; rxdbs = rx; SetParams(); }
  void TellStopBits (unsigned int tx, unsigned int rx)   { txsbs = tx; rxsbs = rx; SetParams(); }

  void TellIrq        (bool tx, bool rx);
  void TellEnabled    (enable_t tx, enable_t rx);
  void TellIrqEnabled (bool irq, enable_t tx, enable_t rx);

  void TellErrors (bool parity, bool framing, bool overrun) {}

  void TellTxShift (unsigned long ntxs) { txs = ntxs; SetParams(); }
  void TellRxShift (unsigned long nrxs) { rxs = nrxs; SetParams(); }

private:
  bool initialized;

  unsigned char cmd, ctl, sta, txd, rxd;
  unsigned int  txdbs, txsbs, rxdbs, rxsbs;
  unsigned long txs, rxs, txbps, rxbps;
  parity_t txpar, rxpar;
  enable_t txen, rxen, txirqen, rxirqen;
  bool txirq, rxirq;

  ByteRegister * cmdreg;
  ByteRegister * ctlreg;
  ByteRegister * stareg;
  ByteRegister * txdreg;
  ByteRegister * rxdreg;

  ByteRegisterWindow * cmdwin;
  ByteRegisterWindow * ctlwin;
  ByteRegisterWindow * stawin;
  ByteRegisterWindow * txdwin;
  ByteRegisterWindow * rxdwin;

  wxTextCtrl * term;
  wxTextCtrl * txpm;
  wxTextCtrl * rxpm;

  wxCheckBox * txenchk;
  wxCheckBox * rxenchk;
  wxCheckBox * txirqchk;
  wxCheckBox * rxirqchk;
  wxCheckBox * txirqenchk;
  wxCheckBox * rxirqenchk;

  void SetParams ();

  DECLARE_EVENT_TABLE()
};

#endif // INCLUDED_ACIA6551FRAME_H
