//
// Core65c02.cpp
// Copyright © 2000-2003 William Sheldon Simms III
//

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Core65c02.h"
#include "disasm.h"
#include "instructions.h"
#include "tables.h"
#include "AddressPeripheral.h"

/* 65c02 execution */

Core65c02::Core65c02 ()
{
  num_instructions = 0;
  num_clockusers = 0;
}

void Core65c02::AddClockCustomer (ClockCustomer * customer)
{
  if (num_clockusers < MAX_NUM_CLOCK_CUSTOMERS)
    clockuser[num_clockusers++] = customer;
}

/* Main CPU fetch/decode/execute loop */

bool Core65c02::Execute (unsigned int count)
{
  bool hit_breakpoint = false;
  Breakpoint * bp = 0;

  unbuild_P(P);

  while (count && !hit_breakpoint)
    {
      --count;

      /* check for interrupts */
      if (interrupt_flags)
	{
	  if ((I == 0) && (interrupt_flags & F_IRQ))
	    interrupt(IRQ_VECTOR, F_IRQ);
	  else if (interrupt_flags & F_NMI)
	    interrupt(NMI_VECTOR, F_NMI);
	  else if (interrupt_flags & F_RESET)
	    interrupt(RES_VECTOR, F_RESET);
	}

      /* fetch, decode, and execute and instruction */
      opcode = READ(emPC); ++emPC;
      EXECUTE(opcode);
      ++num_instructions;

      /* tick the clock for interested parties */
      if (num_clockusers != 0)
	{
	  unsigned long long time = 1000000000;
	  time = (time * cycle_clock) / frequency;

	  for (int idx = 0; idx < num_clockusers; ++idx)
	    clockuser[idx]->TellTime(time);
	}

      /* check for encountered breakpoints */
      int nbkpts = bpm->NumBreakpoints();

      if (nbkpts != 0)
	{
	  /* must count down (not up) because of automatic breakpoint deletion */
	  while (nbkpts--)
	    {
	      bp = bpm->GetBreakpointAtIndex(nbkpts);

	      if (bp->IsEnabled() == false)
		continue;

	      if (bp->GetOperandFlags() == bp_PC)
		{
		  if (bp->GetConditionFlags() == bp_equal)
		    if (bp->GetAddress() == emPC)
		      {
			hit_breakpoint = true;
			if (bp->IsAutomatic())
			  bpm->DeleteBreakpoint(bp);
		      }
		}
	      else if (bp->GetOperandFlags() == bp_S)
		{
		  if (bp->GetConditionFlags() == bp_equal)
		    if (bp->GetValue() == S)
		      {
			hit_breakpoint = true;
			if (bp->IsAutomatic())
			  bpm->DeleteBreakpoint(bp);
		      }
		}
	    }
	}
    }

  P = build_P();

  return hit_breakpoint;
}

unsigned long long Core65c02::GetNumExecuted ()
{
  return num_instructions;
}

void Core65c02::ResetNumExecuted ()
{
  num_instructions = 0;
}

// end
