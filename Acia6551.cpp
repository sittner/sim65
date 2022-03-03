//
// Acia6551.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include "Acia6551.h"
#include <cassert>

Acia6551::Acia6551 () : Uart()
{
  num_listeners = 0;
  
  status = 0x10;
  txbuf  = 0;

  write_control(0, 0);
  write_command(0, 0);

  SetXcvrBuf(tx, 1, &txbuf);

  echo = false;
  external_rx_clock = true;

  SetIrqEnable(false);
}

void Acia6551::AddAcia6551Listener (Acia6551Listener * nlistener)
{
  if (num_listeners < MAX_NUM_LISTENERS)
    {
      listener[num_listeners++] = nlistener;

      nlistener->TellCmd(command);
      nlistener->TellCtl(control);
      nlistener->TellSta(status);
      nlistener->TellTxd(txbuf);
      nlistener->TellRxd(0);
    }
}

void Acia6551::UpdateAll ()
{
  for (unsigned int idx = 0; idx < num_listeners; ++idx)
    {
      listener[idx]->TellCmd(command);
      listener[idx]->TellCtl(control);
      listener[idx]->TellSta(status);
      listener[idx]->TellTxd(txbuf);
      listener[idx]->TellRxd(0);
    }

  Uart::UpdateAll();
}

ap_reader_t Acia6551::GetReader (unsigned int idx)
{
  static ap_rfunc_t r_funcs [] = {
    (ap_rfunc_t)&Acia6551::read_data,
    (ap_rfunc_t)&Acia6551::read_status,
    (ap_rfunc_t)&Acia6551::read_command,
    (ap_rfunc_t)&Acia6551::read_control
  };

  assert(idx < 4);

  ap_reader_t reader;
  reader.ap = this;
  reader.read = r_funcs[idx];

  return reader;
}

ap_writer_t Acia6551::GetWriter (unsigned int idx)
{
  static ap_wfunc_t w_funcs [] = {
    (ap_wfunc_t)&Acia6551::write_data,
    (ap_wfunc_t)&Acia6551::write_status,
    (ap_wfunc_t)&Acia6551::write_command,
    (ap_wfunc_t)&Acia6551::write_control
  };

  assert(idx < 4);

  ap_writer_t writer;
  writer.ap = this;
  writer.write = w_funcs[idx];

  return writer;
}

unsigned char Acia6551::read_data (unsigned short dummy)
{
  return read_rx_data(dummy);
}

unsigned char Acia6551::read_status (unsigned short dummy)
{
  unsigned char status = 0x00;

  status |= (GetParityError() ? 0x01 : 0x00);
  status |= (GetFramingError() ? 0x02 : 0x00);
  status |= (GetOverrunError() ? 0x04 : 0x00);

  status |= (GetXcvrNumBuffered(rx) ? 0x08 : 0x00);
  status |= (GetXcvrNumBuffered(tx) ? 0x00 : 0x10);

  status |= ((GetXcvrIrqFlag(tx) || GetXcvrIrqFlag(rx)) ? 0x80 : 0x00);

  SetXcvrIrqFlag(rx, false);

  return status;
}

unsigned char Acia6551::read_command (unsigned short dummy)
{
  return command;
}

unsigned char Acia6551::read_control (unsigned short dummy)
{
  return control;
}

void Acia6551::write_data (unsigned short dummy, unsigned char byte)
{
  SetXcvrIrqFlag(tx, false);
  write_tx_data(dummy, byte);

  for (unsigned int idx = 0; idx < num_listeners; ++idx)
    listener[idx]->TellTxd(byte);
}

void Acia6551::write_status (unsigned short dummy, unsigned char byte)
{
  write_command(0, command & 0xE0);
  SetOverrunError(false);

  for (unsigned int idx = 0; idx < num_listeners; ++idx)
    listener[idx]->TellSta(byte);
}

void Acia6551::write_command (unsigned short dummy, unsigned char byte)
{
  command = byte;

  set_parity();
  set_tx();

  enable_t rx_enable = EXTRACT_RX_ENABLE();
  SetXcvrEnable(rx, rx_enable);

  SetIrqEnable(true);
  SetXcvrIrqEnable(rx, EXTRACT_RX_IRQ());

  if (rx_enable == DISABLED)
    SetIrqEnable(false);

  for (unsigned int idx = 0; idx < num_listeners; ++idx)
    listener[idx]->TellCmd(byte);
}

void Acia6551::write_control (unsigned short dummy, unsigned char byte)
{
  static unsigned long baud_rate_tab [] = {
    0,       50000,   75000,    109920,  134580,  150000,  300000,  600000,
    1200000, 1800000, 2400000,  3600000, 4800000, 7200000, 9600000, 19200000
  };

  static unsigned int word_length_tab [] = {
    8, 7, 6, 5
  };

  control = byte;

  unsigned int data_bits = word_length_tab[EXTRACT_DATA_BITS()];
  SetXcvrDataBits(rx, data_bits);
  SetXcvrDataBits(tx, data_bits);

  set_stop_bits();

  external_rx_clock = EXTRACT_CLOCK_SOURCE();

  unsigned long bits_per_ksec = baud_rate_tab[EXTRACT_BAUD_RATE()];
  SetXcvrBitsPerKsec(tx, bits_per_ksec);

  if (external_rx_clock)
    SetXcvrBitsPerKsec(rx, 0);
  else
    SetXcvrBitsPerKsec(rx, bits_per_ksec);

  for (unsigned int idx = 0; idx < num_listeners; ++idx)
    listener[idx]->TellCtl(byte);
}

// private member functions

/* Thing to find out: if the 6551 is not currently transmitting, and a byte 
   is written to the data register, is that byte transferred immediately to
   the tx shift register and start to shift, or does the 6551 first wait
   one character time before transmission begins? I'm going to implement
   the wait for now. However for correctness this needs to be verified. */

void Acia6551::set_stop_bits (void)
{
  unsigned int stop_bits = 2;

  assert(GetXcvrParity(tx) == GetXcvrParity(rx));
  assert(GetXcvrDataBits(tx) == GetXcvrDataBits(rx));

  parity_t parity = GetXcvrParity(tx);
  unsigned int data_bits = GetXcvrDataBits(tx);

  if (EXTRACT_STOP_BITS() != 0)
    {
      if (data_bits == 8 && parity != PARITY_NONE)
	stop_bits = 2;
      else if (data_bits == 5 && parity == PARITY_NONE)
	stop_bits = 3;
      else
	stop_bits = 4;
    }

  SetXcvrStopBits(tx, stop_bits);
  SetXcvrStopBits(rx, stop_bits);
}

void Acia6551::set_parity (void)
{
  parity_t parity = PARITY_NONE;

  assert(GetXcvrDataBits(tx) == GetXcvrDataBits(rx));

  if (EXTRACT_PARITY_ENABLE() == ENABLED)
    parity = EXTRACT_PARITY_TYPE();

  SetXcvrParity(tx, parity);
  SetXcvrParity(rx, parity);

  set_stop_bits();
}

void Acia6551::set_tx (void)
{
  echo = false;

  switch (EXTRACT_TX_BITS())
    {
    case 0:
      SetXcvrEnable(tx, DISABLED);
      SetXcvrIrqEnable(tx, DISABLED);
      rts = HIGH;
      if (EXTRACT_ECHO_BIT()) echo = true;
      break;

    case 1:
      SetXcvrEnable(tx, ENABLED);
      SetXcvrIrqEnable(tx, ENABLED);
      rts = LOW;
      break;

    case 2:
      SetXcvrEnable(tx, ENABLED);
      SetXcvrIrqEnable(tx, DISABLED);
      rts = LOW;
      break;

    case 3:
      SetXcvrEnable(tx, BREAK_ENABLED);
      SetXcvrIrqEnable(tx, DISABLED);
      rts = LOW;
      break;
    }
}

// end
