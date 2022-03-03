//
// UartTypes.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_UARTTYPES_H
#define INCLUDED_UARTTYPES_H

enum parity_t {
  PARITY_NONE,
  PARITY_ODD,
  PARITY_EVEN,
  PARITY_MARK,
  PARITY_SPACE
};

enum enable_t {
  DISABLED,
  ENABLED,
  BREAK_ENABLED
};

#ifdef LOW
#undef LOW
#endif

#ifdef HIGH
#undef HIGH
#endif

#define LOW  0
#define HIGH 1

#endif // INCLUDED_UARTTYPES_H
