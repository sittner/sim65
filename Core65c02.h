//
// Core65c02.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_CORE65C02_H
#define INCLUDED_CORE65C02_H

#include "BreakpointManager.h"
#include "ClockCustomer.h"

#define MAX_NUM_CLOCK_CUSTOMERS 8

class Core65c02
{
public:
  Core65c02 ();

  void SetFrequency (unsigned long freq) {
    frequency = freq;
  }

  void SetBreakpointManager (BreakpointManager * nbpm) {
    bpm = nbpm;
  }

  bool Execute (unsigned int count);

  void ResetNumExecuted ();
  unsigned long long GetNumExecuted ();

  void AddClockCustomer (ClockCustomer * customer);

private:
  unsigned char opcode;
  unsigned long frequency; // Hz
  unsigned long long num_instructions;
  BreakpointManager * bpm;

  int num_clockusers;
  ClockCustomer * clockuser[MAX_NUM_CLOCK_CUSTOMERS];
};

#endif // INCLUDED_CORE65C02_H
