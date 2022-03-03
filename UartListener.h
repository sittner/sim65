//
// UartListener.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_UARTLISTENER_H
#define INCLUDED_UARTLISTENER_H

#include "UartTypes.h"

class UartListener
{
public:
  virtual void TxBit (unsigned long long time, unsigned char level) = 0;
  virtual void TxByte (unsigned char) = 0;

  virtual void TellBaud (unsigned long, unsigned long) = 0;
  virtual void TellParity (parity_t, parity_t) = 0;
  virtual void TellDataBits (unsigned int, unsigned int) = 0;
  virtual void TellStopBits (unsigned int, unsigned int) = 0;

  virtual void TellIrq (bool, bool) = 0;
  virtual void TellEnabled (enable_t, enable_t) = 0;
  virtual void TellIrqEnabled (bool, enable_t, enable_t) = 0;

  virtual void TellErrors (bool parity, bool framing, bool overrun) = 0;

  virtual void TellTxShift (unsigned long) = 0;
  virtual void TellRxShift (unsigned long) = 0;
};

#endif // INCLUDED_UARTLISTENER_H
