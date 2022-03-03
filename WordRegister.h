//
// WordRegister.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_WORDREGISTER_H
#define INCLUDED_WORDREGISTER_H

#define MAX_NUM_LISTENERS 8

#include "WordRegisterListener.h"

class WordRegister
{
public:
  WordRegister (unsigned short * rptr);
  unsigned short Read (void);
  void Write (unsigned short w);
  bool AddListener (WordRegisterListener * new_listener);

private:
  unsigned short * ptr;
  int num_listeners;
  WordRegisterListener * listeners[MAX_NUM_LISTENERS];
};

#endif // INCLUDED_WORDREGISTER_H
