//
// WordRegisterListener.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_WORDREGISTERLISTENER_H
#define INCLUDED_WORDREGISTERLISTENER_H

class WordRegisterListener
{
public:
  virtual void TellNewValue(unsigned short w) = 0;
};

#endif // INCLUDED_WORDREGISTERLISTENER_H
