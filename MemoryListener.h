//
// MemoryListener.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_MEMORYLISTENER_H
#define INCLUDED_MEMORYLISTENER_H

class MemoryListener
{
public:
  virtual void TellNewValue(unsigned short a, unsigned char b) = 0;
};

#endif // INCLUDED_MEMORYLISTENER_H
