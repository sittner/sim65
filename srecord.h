//
// srecord.h
// Copyright © 2003 William Sheldon Simms III
//

#ifndef INCLUDED_SRECORD_H
#define INCLUDED_SRECORD_H

#include <stdio.h>

struct srecord
{
  int type;
  int length;
  unsigned long address;
  unsigned char * buf;
  unsigned char * data;
};

enum
  {
    srec_result_no_srec  = -7,
    srec_result_checksum = -6,
    srec_result_malloc   = -5,
    srec_result_overflow = -4,
    srec_result_bad_arg  = -3,
    srec_result_eol      = -2,
    srec_result_eof      = -1,
    srec_result_ok       =  0,
    srec_type_s0         =  0,
    srec_type_s1         =  1,
    srec_type_s2         =  2,
    srec_type_s3         =  3,
    srec_type_s4         =  4,
    srec_type_s5         =  5,
    srec_type_s7         =  7,
    srec_type_s8         =  8,
    srec_type_s9         =  9,

    remark_unknown_srecord    = 0x1,
    remark_spurious_character = 0x2
  };

int read_srecord (FILE * f, struct srecord * srec, int * remarks);

#endif // INCLUDED_SRECORD_H
