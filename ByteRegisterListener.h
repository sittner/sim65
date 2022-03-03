//
// ByteRegisterListener.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_BYTEREGISTERLISTENER_H
#define INCLUDED_BYTEREGISTERLISTENER_H

class ByteRegisterListener
{
public:
  virtual void TellNewValue(unsigned char b) = 0;
};

#endif // INCLUDED_BYTEREGISTERLISTENER_H
