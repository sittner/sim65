//
// Memory.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cassert>
#include "Memory.h"

Memory::Memory ()
{
  for (int idx = 0; idx < ADDRESS_SPACE_SIZE; idx++)
    {
      r_ptr[idx] = 0;
      w_ptr[idx] = 0;
    }

  running = false;
  num_listeners = 0;
}

bool Memory::IsReadable (unsigned short addr)
{
  if (r_ptr[addr] == 0) return false;
  return true;
}

bool Memory::IsRom (unsigned short addr)
{
  unsigned char * ptr = r_ptr[addr];

  // if it's not NULL, and not ram, it must be rom

  if (ptr == 0) return false;
  if (ptr < ram) return true;
  if (ptr < (ram + ADDRESS_SPACE_SIZE)) return false;
  return true;
}

void Memory::LoadToRom (unsigned short addr, unsigned char byte)
{
  // if the address really is mapped as rom
  if (IsRom(addr))
    {
      // then go in through the out door
      *(r_ptr[addr]) = byte;
    }
}

unsigned char Memory::Read (unsigned short addr)
{
  return *(r_ptr[addr]);
}

void Memory::Write (unsigned short addr, unsigned char byte)
{
  if (running)
    {
      *(w_ptr[addr]) = byte;
    }
  else
    {
      unsigned char old = *(r_ptr[addr]);
      *(w_ptr[addr]) = byte;

      if (old != byte)
	{
	  for (int idx = 0; idx < num_listeners; ++idx)
	    listeners[idx]->TellNewValue(addr, byte);
	}
    }
}

ap_reader_t Memory::GetReader (unsigned int idx)
{
  assert(idx < 1);

  ap_reader_t reader;
  reader.ap = this;
  reader.read = (ap_rfunc_t)&Memory::Read;

  return reader;
}

ap_writer_t Memory::GetWriter (unsigned int idx)
{
  assert(idx < 1);

  ap_writer_t writer;
  writer.ap = this;
  writer.write = (ap_wfunc_t)&Memory::Write;

  return writer;
}

Memory::~Memory ()
{
  num_listeners = 0;
}

bool Memory::AddListener (MemoryListener * new_listener)
{
  if (num_listeners == MAX_NUM_LISTENERS)
    return false;

  listeners[num_listeners++] = new_listener;
  return true;
}

void Memory::init_ram_memory_read (unsigned short cpu_address_begin,
				   unsigned short cpu_address_end,
				   unsigned short mem_address)
{
  init_mem_r(cpu_address_begin, cpu_address_end, mem_address, ram);
}

void Memory::init_ram_memory_write (unsigned short cpu_address_begin,
				    unsigned short cpu_address_end,
				    unsigned short mem_address)
{
  init_mem_w(cpu_address_begin, cpu_address_end, mem_address, ram);
}

void Memory::init_rom_memory (unsigned short cpu_address_begin,
			      unsigned short cpu_address_end,
			      unsigned short mem_address)
{
  init_mem_r(cpu_address_begin, cpu_address_end, mem_address, rom);
  init_mem_w(cpu_address_begin, cpu_address_end, mem_address, 0);
}

void Memory::init_mem_r (unsigned short cpu_address_begin,
			 unsigned short cpu_address_end,
			 unsigned short mem_address,
			 unsigned char * mem)
{
  if (mem == 0) return;

  unsigned int count = 0;
  for (unsigned int idx = cpu_address_begin; idx <= cpu_address_end; ++idx)
    {
      r_ptr[mem_address + count] = mem + idx;
      ++count;
    }
}

void Memory::init_mem_w (unsigned short cpu_address_begin,
			 unsigned short cpu_address_end,
			 unsigned short mem_address,
			 unsigned char * mem)
{
  if (mem == 0) return;

  unsigned int count = 0;
  for (unsigned int idx = cpu_address_begin; idx <= cpu_address_end; ++idx)
    {
      w_ptr[mem_address + count] = mem + idx;
      ++count;
    }
}

// end
