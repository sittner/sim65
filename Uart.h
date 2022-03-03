//
// Uart.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_UART_H
#define INCLUDED_UART_H

#include "AddressPeripheral.h"
#include "ClockCustomer.h"
#include "UartListener.h"
#include "UartTypes.h"

#define MAX_NUM_LISTENERS 8

class Uart; // forward for xcvr

class xcvr
{
  friend class Uart;

  unsigned char * buf;
  unsigned int bufidx;
  unsigned int buflen;

  unsigned int data_bits;
  unsigned int twice_stop_bits;

  parity_t parity;

  enable_t enable;
  enable_t irq_enable;

  bool irq_flag;

  unsigned long bits_per_ksec;
  unsigned long bit_time;

  bool newbuf;
};

class Uart : public ClockCustomer, public AddressPeripheral
{
public:
  Uart ();
  virtual ~Uart ();

  unsigned char read_rx_data (unsigned short dummy);
  void write_tx_data (unsigned short dummy, unsigned char byte);

  bool update (unsigned long ns);

  void TellTime (unsigned long long current_time);

  void AddUartListener (UartListener *);
  void UpdateAll ();

protected:
  int SetXcvrBuf (xcvr & x, unsigned int len, unsigned char * ptr);

  void SetXcvrIrqFlag     (xcvr & x, bool irq_flag);
  void SetXcvrIrqEnable   (xcvr & x, enable_t irq_enable);
  void SetXcvrDataBits    (xcvr & x, unsigned int data_bits);
  void SetXcvrStopBits    (xcvr & x, unsigned int stop_bits);
  void SetXcvrEnable      (xcvr & x, enable_t enable);
  void SetXcvrParity      (xcvr & x, parity_t parity);
  void SetXcvrBitsPerKsec (xcvr & x, unsigned long bits_per_ksec);

  int          GetXcvrNumBuffered (xcvr & x) { return x.bufidx; }
  bool         GetXcvrIrqFlag     (xcvr & x) { return x.irq_flag; }
  enable_t     GetXcvrEnable      (xcvr & x) { return x.enable; }
  enable_t     GetXcvrIrqEnable   (xcvr & x) { return x.irq_enable; }
  parity_t     GetXcvrParity      (xcvr & x) { return x.parity; }
  unsigned int GetXcvrDataBits    (xcvr & x) { return x.data_bits; }
  unsigned int GetXcvrStopBits    (xcvr & x) { return x.twice_stop_bits; }

  void SetIrqEnable (bool flag);
  void SetParityError (bool flag) { par_error = flag; }
  void SetFramingError (bool flag) { frm_error = flag; }
  void SetOverrunError (bool flag) { ovr_error = flag; }

  bool GetIrqEnable () { return irq_enable; }
  bool GetParityError () { return par_error; }
  bool GetFramingError () { return frm_error; }
  bool GetOverrunError () { return ovr_error; }

  // protected data members

  xcvr tx;
  xcvr rx;

  bool txing;

  unsigned int tx_idx;
  unsigned int tx_length;
  unsigned long tx_shift;
 
  unsigned int rts;
  unsigned int cts;

  unsigned long time_index;
  unsigned long current_time;

private:
  bool par_error;
  bool frm_error;
  bool ovr_error;

  bool irq_enable; // all irqs - separate from rx and tx irq enables

  unsigned int num_listeners;
  UartListener * listener [MAX_NUM_LISTENERS];

  unsigned int num_one_bits (unsigned char byte);
  unsigned long double_bits (unsigned char byte);
  unsigned long build_tx_shift (unsigned char byte);
  unsigned long calc_bit_time (unsigned long bits_per_ksec);
};

#endif // INCLUDED_UART_H
