//
// AddressPeripheral.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include "AddressPeripheral.h"

ap_reader_t ap_reader [ADDRESS_SPACE_SIZE];
ap_writer_t ap_writer [ADDRESS_SPACE_SIZE];

ap_reader_t NullPeripheral::GetReader (unsigned int idx)
{
  assert(idx < 1);

  ap_reader_t reader;
  reader.ap = this;
  reader.read = (ap_rfunc_t)&NullPeripheral::Read;

  return reader;
}

ap_writer_t NullPeripheral::GetWriter (unsigned int idx)
{
  assert(idx < 1);

  ap_writer_t writer;
  writer.ap = this;
  writer.write = (ap_wfunc_t)&NullPeripheral::Write;

  return writer;
}

unsigned char NullPeripheral::Read (unsigned short addr)
{
  return 0;
}

void NullPeripheral::Write (unsigned short addr, unsigned char byte)
{
}

// end
