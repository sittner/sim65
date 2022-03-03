//
// ByteRegister.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include <cstdio>
#include "ByteRegister.h"

ByteRegister::ByteRegister (unsigned char * rptr)
{
  ptr = rptr;
  num_listeners = 0;
}

unsigned char ByteRegister::Read (void)
{
  return *ptr;
}

void ByteRegister::Write (unsigned char b)
{
  unsigned char old = *ptr;

  *ptr = b;

  if (old != b)
    {
      for (int idx = 0; idx < num_listeners; ++idx)
	listeners[idx]->TellNewValue(b);
    }
}

bool ByteRegister::AddListener (ByteRegisterListener * new_listener)
{
  if (num_listeners >= MAX_NUM_LISTENERS)
    return false;

  listeners[num_listeners++] = new_listener;
  return true;
}

// end
