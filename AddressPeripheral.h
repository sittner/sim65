//
// AddressPeripheral.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_ADDRESSPERIPHERAL_H
#define INCLUDED_ADDRESSPERIPHERAL_H

#include <cassert>
#include <cstdio>
#include <cstdint>

class AddressPeripheral;

#define ADDRESS_SPACE_SIZE 65536

typedef unsigned char (AddressPeripheral::* ap_rfunc_t) (unsigned short addr);
typedef void (AddressPeripheral::* ap_wfunc_t) (unsigned short addr, unsigned char byte);

struct ap_reader_t
{
  AddressPeripheral * ap;
  ap_rfunc_t read;
};

struct ap_writer_t
{
  AddressPeripheral * ap;
  ap_wfunc_t write;
};

class AddressPeripheral
{
public:
  virtual ~AddressPeripheral () {};
  virtual unsigned int NumReaders () = 0;
  virtual unsigned int NumWriters () = 0;
  virtual ap_reader_t GetReader (unsigned int idx) = 0;
  virtual ap_writer_t GetWriter (unsigned int idx) = 0;
};

class NullPeripheral: public AddressPeripheral
{
public:
  unsigned int NumReaders () { return 1; }
  unsigned int NumWriters () { return 1; }

  ap_reader_t GetReader (unsigned int);
  ap_writer_t GetWriter (unsigned int);

  unsigned char Read (unsigned short);
  void Write (unsigned short, unsigned char);
};

extern ap_reader_t ap_reader [];
extern ap_writer_t ap_writer [];

inline uint8_t READ (uint16_t addr)
{
  ap_reader_t reader = ap_reader[addr];

#if 0
  if (reader.ap == 0) printf("ap_reader[%4.4x].ap == NULL\n", addr);
  assert(reader.ap);
  if (reader.read == 0) printf("ap_reader[%4.4x].read == NULL\n", addr);
  assert(reader.read);
#endif

  return ((reader.ap)->*(reader.read))(addr);
}

inline void WRITE (uint16_t addr, uint8_t byte)
{
  ap_writer_t writer = ap_writer[addr];

#if 0
  assert(writer.ap);
  assert(writer.write);
#endif

  ((writer.ap)->*(writer.write))(addr, byte);
}

#define READ_ZP(a)    READ(a)
#define WRITE_ZP(a,b) WRITE(a,b)

#define READ_STACK(a)    READ(0x100+a)
#define WRITE_STACK(a,b) WRITE(0x100+a,b)

#endif // INCLUDED_ADDRESSPERIPHERAL_H
