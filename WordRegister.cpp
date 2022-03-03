//
// WordRegister.cpp
// Copyright © 2003 William Sheldon Simms III
//

#include "WordRegister.h"

WordRegister::WordRegister (unsigned short * rptr)
{
  ptr = rptr;
  num_listeners = 0;
}

unsigned short WordRegister::Read (void)
{
  return *ptr;
}

void WordRegister::Write (unsigned short w)
{
  *ptr = w;

  for (int idx = 0; idx < num_listeners; ++idx)
    listeners[idx]->TellNewValue(w);
}

bool WordRegister::AddListener (WordRegisterListener * new_listener)
{
  if (num_listeners >= MAX_NUM_LISTENERS)
    return false;

  listeners[num_listeners++] = new_listener;
  return true;
}

// end
