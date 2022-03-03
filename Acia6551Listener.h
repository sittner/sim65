//
// Acia6551Listener.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_ACIA6551LISTENER_H
#define INCLUDED_ACIA6551LISTENER_H

class Acia6551Listener
{
public:
  virtual void TellCmd (unsigned char ncmd) = 0;
  virtual void TellCtl (unsigned char nctl) = 0;
  virtual void TellSta (unsigned char nsta) = 0;
  virtual void TellTxd (unsigned char ntxd) = 0;
  virtual void TellRxd (unsigned char nrxd) = 0;
};

#endif // INCLUDED_ACIA6551LISTENER_H
