//
// ByteRegister.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_BYTEREGISTER_H
#define INCLUDED_BYTEREGISTER_H

#include "ByteRegisterListener.h"

#define MAX_NUM_LISTENERS 8

class ByteRegister
{
public:
  ByteRegister (unsigned char * rptr);
  unsigned char Read (void);
  void Write (unsigned char b);
  bool AddListener (ByteRegisterListener * new_listener);

private:
  unsigned char * ptr;
  int num_listeners;
  ByteRegisterListener * listeners[MAX_NUM_LISTENERS];
};

#endif // INCLUDED_BYTEREGISTER_H
