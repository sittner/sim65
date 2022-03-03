//
// ClockCustomer.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_CLOCKCUSTOMER_H
#define INCLUDED_CLOCKCUSTOMER_H

class ClockCustomer
{
public:
  virtual ~ClockCustomer () {};
  virtual void TellTime (unsigned long long current_time) = 0;
};

#endif // INCLUDED_CLOCKCUSTOMER_H
