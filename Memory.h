//
// Memory.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H

#include "AddressPeripheral.h"
#include "MemoryListener.h"

#define MAX_NUM_LISTENERS 8

class Memory: public AddressPeripheral
{
public:
  Memory ();
  ~Memory ();

  void SetRunning (bool runf) { running = runf; }

  bool IsRom (unsigned short addr);
  bool IsReadable (unsigned short addr);
  void LoadToRom (unsigned short addr, unsigned char byte);

  unsigned char Read (unsigned short addr);
  void Write (unsigned short addr, unsigned char byte);

  bool AddListener (MemoryListener * new_listener);

  unsigned int NumReaders () { return 1; }
  unsigned int NumWriters () { return 1; }

  ap_reader_t GetReader (unsigned int idx);
  ap_writer_t GetWriter (unsigned int idx);

  void init_ram_memory_read (unsigned short cpu_address_begin,
			     unsigned short cpu_address_end,
			     unsigned short mem_address);

  void init_ram_memory_write (unsigned short cpu_address_begin,
			      unsigned short cpu_address_end,
			      unsigned short mem_address);

  void init_rom_memory (unsigned short cpu_address_begin,
			unsigned short cpu_address_end,
			unsigned short mem_address);

private:
  void init_mem_r (unsigned short cpu_address_begin,
		   unsigned short cpu_address_end,
		   unsigned short mem_address,
		   unsigned char * mem);

  void init_mem_w (unsigned short cpu_address_begin,
		   unsigned short cpu_address_end,
		   unsigned short mem_address,
		   unsigned char * mem);

  unsigned char * r_ptr [ADDRESS_SPACE_SIZE];
  unsigned char * w_ptr [ADDRESS_SPACE_SIZE];

  unsigned char ram [ADDRESS_SPACE_SIZE];
  unsigned char rom [ADDRESS_SPACE_SIZE];

  bool running;

  int num_listeners;
  MemoryListener * listeners[MAX_NUM_LISTENERS];
};

#endif // INCLUDED_MEMORY_H
