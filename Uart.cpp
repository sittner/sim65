//
// Uart.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cstdlib>
#include "Uart.h"

// public member functions

Uart::Uart ()
{
  rts = LOW;
  cts = LOW;
  tx_idx = 0;
  tx_shift = 0;
  tx_length = 0;
  time_index = 0;
  current_time = 0;
  num_listeners = 0;

  txing = false;
  irq_enable = false;
    
  tx.buf = NULL;
  tx.enable = DISABLED;
  tx.parity = PARITY_EVEN;

  rx.buf = NULL;
  rx.enable = DISABLED;
  rx.parity = PARITY_EVEN;

  par_error = false;
  frm_error = false;
  ovr_error = false;
}

Uart::~Uart ()
{
  if (tx.newbuf) delete[] tx.buf;
  if (rx.newbuf) delete[] rx.buf;
}

void Uart::AddUartListener (UartListener * nlistener)
{
  if (num_listeners < MAX_NUM_LISTENERS)
    {
      listener[num_listeners++] = nlistener;

      nlistener->TellBaud(tx.bits_per_ksec, rx.bits_per_ksec);
      nlistener->TellParity(tx.parity, rx.parity);
      nlistener->TellDataBits(tx.data_bits, rx.data_bits);
      nlistener->TellStopBits(tx.twice_stop_bits, rx.twice_stop_bits);

      nlistener->TellIrq(tx.irq_flag, rx.irq_flag);
      nlistener->TellEnabled(tx.enable, rx.enable);
      nlistener->TellIrqEnabled(irq_enable, tx.irq_enable, rx.irq_enable);

      nlistener->TellTxShift(tx_shift);
      nlistener->TellRxShift(0);
    }
}

void Uart::UpdateAll ()
{
  for (unsigned int idx = 0; idx < num_listeners; ++idx)
    {
      listener[idx]->TellBaud(tx.bits_per_ksec, rx.bits_per_ksec);
      listener[idx]->TellParity(tx.parity, rx.parity);
      listener[idx]->TellDataBits(tx.data_bits, rx.data_bits);
      listener[idx]->TellStopBits(tx.twice_stop_bits, rx.twice_stop_bits);

      listener[idx]->TellIrq(tx.irq_flag, rx.irq_flag);
      listener[idx]->TellEnabled(tx.enable, rx.enable);
      listener[idx]->TellIrqEnabled(irq_enable, tx.irq_enable, rx.irq_enable);

      listener[idx]->TellTxShift(tx_shift);
      listener[idx]->TellRxShift(0);
    }
}

unsigned char Uart::read_rx_data (unsigned short dummy)
{
  if (rx.bufidx == 0) return 0;

  rx.bufidx = rx.bufidx - 1;

  return rx.buf[rx.bufidx];
}

void Uart::write_tx_data (unsigned short dummy, unsigned char byte)
{
  if (tx.enable == ENABLED)
    {
      tx.buf[tx.bufidx] = byte;
      tx.bufidx = tx.bufidx + 1;

      if (txing == false)
	{
	  /* start shifting all high bits immediately */

	  txing = true;
	  tx_idx = 0;
	  tx_shift = ~0;

	  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
	    listener[idx]->TellTxShift(tx_shift);

	  tx_length = (2 * tx.data_bits) + tx.twice_stop_bits + 2;
	  if (tx.parity != PARITY_NONE)
	    tx_length += 2;
	}
    }
}

bool Uart::update (unsigned long ns)
{
  /* need to be able to handle the case where at some point in the middle of 'ns'
     nanoseconds, the CPU disabled transmitting */

  current_time += ns;

  while (txing == true && tx.bit_time != 0)
    {
      while (tx_idx < tx_length)
	{
	  if ((current_time - time_index) >= tx.bit_time)
	    {
	      unsigned char tx_level = tx_shift & 1;
	      tx_shift >>= 1;

	      for (unsigned int idx  = 0; idx < num_listeners; ++idx)
		{
		  listener[idx]->TxBit(time_index,  tx_level);
		  listener[idx]->TellTxShift(tx_shift);
		}

	      time_index += tx.bit_time;
	      ++tx_idx;
	    }
	  else
	    {
	      return true; /* need to be called again */
	    }
	}

      if (tx.bufidx > 0)
	{
	  tx_idx = 0;
	  tx.bufidx = tx.bufidx - 1;
	  tx_shift = build_tx_shift(tx.buf[tx.bufidx]);

	  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
	    {
	      listener[idx]->TxByte(tx.buf[tx.bufidx]);
	      listener[idx]->TellTxShift(tx_shift);
	    }
	}
      else
	{
	  txing = false;
	  if (tx.irq_enable == ENABLED)
	    {
	      tx.irq_flag = true;
	      for (unsigned int idx  = 0; idx < num_listeners; ++idx)
		listener[idx]->TellIrq(tx.irq_flag, rx.irq_flag);
	    }
	}
    }

  time_index = current_time;

  return false; /* don't need to be called again */
}

void Uart::TellTime (unsigned long long current_time)
{
  static unsigned long long last_time = 0;

  unsigned long time_diff = current_time - last_time;
  last_time = current_time;

  update(time_diff);
}

// protected member functions

int Uart::SetXcvrBuf (xcvr & x, unsigned int len, unsigned char * ptr)
{
  if (len < 0) return -1;

  x.buflen = 0;
  x.bufidx = 0;

  if (ptr)
    {
      x.buf = ptr;
      x.newbuf = false;
    }
  else
    {
      x.buf = new unsigned char [len];
      x.newbuf = true;
    }

  if (x.buf == NULL) return -2;

  x.buflen = len;
  return 0;
}

void Uart::SetXcvrIrqFlag (xcvr & x, bool irq_flag)
{
  x.irq_flag = irq_flag;
  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
    listener[idx]->TellIrq(tx.irq_flag, rx.irq_flag);
}

void Uart::SetXcvrIrqEnable (xcvr & x, enable_t irq_enable)
{
  x.irq_enable = irq_enable;
  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
    listener[idx]->TellIrqEnabled(irq_enable, tx.irq_enable, rx.irq_enable);
}

void Uart::SetXcvrDataBits (xcvr & x, unsigned int data_bits)
{
  x.data_bits = data_bits;
  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
    listener[idx]->TellDataBits(tx.data_bits, rx.data_bits);
}

void Uart::SetXcvrStopBits (xcvr & x, unsigned int stop_bits)
{
  x.twice_stop_bits = stop_bits;
  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
    listener[idx]->TellDataBits(tx.twice_stop_bits, rx.twice_stop_bits);
}

void Uart::SetXcvrEnable (xcvr & x, enable_t enable)
{
  x.enable = enable;
  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
    listener[idx]->TellEnabled(tx.enable, rx.enable);
}

void Uart::SetXcvrParity (xcvr & x, parity_t parity)
{
  x.parity = parity;
  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
    listener[idx]->TellParity(tx.parity, rx.parity);
}

void Uart::SetXcvrBitsPerKsec (xcvr & x, unsigned long bits_per_ksec)
{
  x.bits_per_ksec = bits_per_ksec;
  x.bit_time = calc_bit_time(bits_per_ksec);
  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
    listener[idx]->TellBaud(tx.bits_per_ksec, rx.bits_per_ksec);
}

void Uart::SetIrqEnable (bool flag)
{
  irq_enable = flag;
  for (unsigned int idx  = 0; idx < num_listeners; ++idx)
    listener[idx]->TellIrqEnabled(irq_enable, tx.irq_enable, rx.irq_enable);
}

// private member functions

unsigned int Uart::num_one_bits (unsigned char byte)
{
  static int bcnt[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

  return bcnt[byte & 0xF] + bcnt[byte / 16];
}

unsigned long Uart::double_bits (unsigned char byte)
{
  static unsigned long dbit[16] = { 0x00, 0x03, 0x0C, 0x0F, 0x30, 0x33, 0x3C, 0x3F,
                                    0xC0, 0xC3, 0xCC, 0xCF, 0xF0, 0xF3, 0xFC, 0xFF };

  return dbit[byte & 0xF] + (256 * dbit[byte / 16]);
}

/* builds the values that will actually be transmitted at a
   rate of 2*baud in order to handle 1.5 stop bits - i.e.
   builds a 20 bit value for an 8N1 character. */

unsigned long Uart::build_tx_shift (unsigned char tx_data)
{
  int twice_data_bits;
  unsigned long tx_shift;

  /* initialize to stop bits and prepare for data & parity */
  tx_shift = ~(0ul);

  twice_data_bits = 2 * tx.data_bits;

  if (tx.parity == PARITY_NONE)
    tx_length = twice_data_bits;
  else
    tx_length = 2 + twice_data_bits;

  tx_shift <<= tx_length;

  tx_length += 2;
  tx_length += tx.twice_stop_bits;

  /* data bits */
  tx_shift |= double_bits(tx_data) & (0xFFFFul >> (16 - twice_data_bits));

  /* parity bit */
  if (tx.parity == PARITY_ODD)
    {
      if ((num_one_bits(tx_data) & 1) == 0)
	tx_shift |= (0x3 << twice_data_bits);
    }
  else if (tx.parity == PARITY_EVEN)
    {
      if ((num_one_bits(tx_data) & 1) == 1)
	tx_shift |= (0x3 << twice_data_bits);
    }
  else if (tx.parity == PARITY_MARK)
    {
      tx_shift |= (0x3 << twice_data_bits);
    }

  /* start bits */
  tx_shift <<= 2;

  return tx_shift;
}

unsigned long Uart::calc_bit_time (unsigned long bits_per_ksec)
{
  if (bits_per_ksec == 0) return 0;

  /* this returns half the actual number of nanoseconds per bit
     since bits are shifted out at twice the simulated baud rate */

  return (unsigned long)((5e11 / (double)bits_per_ksec) + 0.5);
}

// end
