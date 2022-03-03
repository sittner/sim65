//
// Acia6551.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_ACIA6551_H
#define INCLUDED_ACIA6551_H

#include "Uart.h"
#include "Acia6551Listener.h"

class Acia6551 : public Uart
{
public:
  Acia6551 ();

  unsigned int NumReaders () { return 4; }
  unsigned int NumWriters () { return 4; }

  ap_reader_t GetReader (unsigned int idx);
  ap_writer_t GetWriter (unsigned int idx);

  unsigned char read_data    (unsigned short dummy);
  unsigned char read_status  (unsigned short dummy);
  unsigned char read_command (unsigned short dummy);
  unsigned char read_control (unsigned short dummy);

  void write_data    (unsigned short dummy, unsigned char byte);
  void write_status  (unsigned short dummy, unsigned char byte);
  void write_command (unsigned short dummy, unsigned char byte);
  void write_control (unsigned short dummy, unsigned char byte);

  void AddAcia6551Listener (Acia6551Listener *);
  void UpdateAll ();

private:
  void set_stop_bits (void);
  void set_parity (void);
  void set_tx (void);

  unsigned int EXTRACT_TX_BITS   () { return (0x0C & command) >> 2; }
  unsigned int EXTRACT_DATA_BITS () { return (0x60 & control) >> 5; }
  unsigned int EXTRACT_STOP_BITS () { return (0x80 & control) >> 7; }
  unsigned int EXTRACT_BAUD_RATE () { return 0x08 & control; }

  parity_t EXTRACT_PARITY_TYPE   () { return (parity_t)(((0xC0 & command) >> 6) + 1); }
  enable_t EXTRACT_PARITY_ENABLE () { return ((0x20 & command) >> 5) ? ENABLED : DISABLED; }

  enable_t EXTRACT_RX_IRQ ()    { return ((0x02 & command) >> 1) ? DISABLED : ENABLED; }
  enable_t EXTRACT_RX_ENABLE () { return (0x01 & command) ? ENABLED : DISABLED; }

  bool EXTRACT_ECHO_BIT     () { return ((0x10 & command) >> 4) ? true : false; }
  bool EXTRACT_CLOCK_SOURCE () { return ((0x10 & control) >> 4) ? false : true; }

  unsigned char txbuf;
  unsigned char status;
  unsigned char command;
  unsigned char control;

  bool echo;
  bool external_rx_clock;

  unsigned int num_listeners;
  Acia6551Listener * listener [MAX_NUM_LISTENERS];
};

#endif // INCLUDED_ACIA6551_H
