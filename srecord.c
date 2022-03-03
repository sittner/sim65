/*
 * srecord.c
 * Copyright 2003 William Sheldon Simms III
 */

#include <ctype.h>
#include <stdlib.h>

#include "srecord.h"

static int my_ungetc_char = -1;

/*
 * my_fgetc()
 */

static int my_fgetc (FILE * f)
{
  if (f == NULL) return EOF;

  if (my_ungetc_char >= 0)
    {
      int ch;
      ch = my_ungetc_char;
      my_ungetc_char = -1;
      return ch;
    }

  return fgetc(f);
}

/*
 * find_srecord()
 *
 * Reads characters until end-of-line (CR/NL/CRNL) looking for the start of
 * an S-Record. Returns after reading only the type field of the S-Record.
 *
 * return:
 * -7 : No S-Record found before EOF/EOL
 * -3 : Bad arguments
 * -1 : EOF
 *  N >= 0 : S-Record of type N found
 */

static int find_srecord (FILE * f, int * remarks)
{
  int ch;
  int s = 0;

  if (f == NULL) return srec_result_bad_arg;

  if (remarks != NULL) *remarks = 0;

  ch = my_fgetc(f);

  while (ch != EOF)
    {
      if (ch == '\n') return srec_result_no_srec;
      if (ch == '\r')
	{
	  ch = my_fgetc(f);
	  if (ch == EOF) return srec_result_eof;
	  if (ch != '\n')
	    {
	      if (EOF == ungetc(ch, f))
		my_ungetc_char = ch;
	    }

	  return srec_result_no_srec;
	}

      if (ch == 'S')
	{
	  s = 1;
	}
      else if (s == 1)
	{
	  if (isxdigit(ch))
	    {
	      if (isdigit(ch))
		{
		  if (ch == '4' || ch =='6')
		    {
		      if (remarks != NULL)
			*remarks = *remarks | remark_unknown_srecord;
		    }
		  else
		    return ch - '0';
		}
	      else
		{
		  if (remarks != NULL)
		    *remarks = *remarks | remark_unknown_srecord;
		}
	    }
	  else
	    {
	      if (remarks != NULL)
		*remarks = *remarks | remark_spurious_character;
	    }
	}

      ch = my_fgetc(f);
    }

  return srec_result_eof;
}

/*
 * srecord_length()
 *
 * To be called after find_srecord() returns a result >= 0. Reads the
 * two hexadecimal digit length field of the S-Record.
 *
 * return:
 * -3 : Bad arguments
 * -2 : No length field found before EOF/EOL
 * -1 : EOF
 *  N >= 0 : S-Record length N
 */

static int srecord_length (FILE * f, int * remarks)
{
  int ch;
  int one = 0;
  int length = 0;

  if (f == NULL) return srec_result_bad_arg;

  if (remarks != NULL) *remarks = 0;

  ch = my_fgetc(f);

  while (ch != EOF)
    {
      if (ch == '\n') return srec_result_eol;
      if (ch == '\r')
	{
	  ch = my_fgetc(f);
	  if (ch == EOF) return srec_result_eof;
	  if (ch != '\n')
	    {
	      if (EOF == ungetc(ch, f))
		my_ungetc_char = ch;
	    }

	  return srec_result_eol;
	}

      if (isxdigit(ch))
	{
	  length *= 16;

	  if (isdigit(ch))
	    length += (ch - '0');
	  else
	    length += (toupper(ch) - 'A' + 10);

	  if (one)
	    return length;
	  else
	    one = 1;
	}
      else
	{
	  if (remarks != NULL)
	    *remarks = *remarks | remark_spurious_character;
	}
      
      ch = my_fgetc(f);
    }

  return srec_result_eof;
}

/*
 * read_srecord()
 *
 * Given the S-Record length and a buffer for the S-Record data, reads
 * the S-Record data into the buffer.
 *
 * return:
 * -4 : More bytes read than passed length
 * -3 : Bad arguments
 * -2 : Unexpected EOL (before all S-Record data could be read)
 * -1 : EOF
 *  0 : No Error - Read the expected number of bytes
 */

static int read_srecord_data (FILE * f, int length, unsigned char * buf, int * remarks)
{
  int ch;
  int one = 0;
  int byte = 0;
  int bufidx = 0;

  if (f == NULL) return srec_result_bad_arg;
  if (length < 3) return srec_result_bad_arg;
  if (buf == NULL) return srec_result_bad_arg;

  if (remarks != NULL) *remarks = 0;

  ch = my_fgetc(f);

  while (ch != EOF)
    {
      if (ch == '\n')
	{
	  if (bufidx < length) return srec_result_eol;
	  if (bufidx > length) return srec_result_overflow;
	  return srec_result_ok;
	}

      if (ch == '\r')
	{
	  ch = my_fgetc(f);
	  if (ch == EOF) return srec_result_eof;
	  if (ch != '\n')
	    {
	      if (EOF == ungetc(ch, f))
		my_ungetc_char = ch;
	    }

	  if (bufidx < length) return srec_result_eol;
	  if (bufidx > length) return srec_result_overflow;
	  return srec_result_ok;
	}

      if (isxdigit(ch))
	{
	  byte *= 16;

	  if (isdigit(ch))
	    byte += (ch - '0');
	  else
	    byte += (toupper(ch) - 'A' + 10);

	  if (one)
	    {
	      if (bufidx < length)
		buf[bufidx] = byte;

	      ++bufidx;
	      one = byte = 0;
	    }
	  else
	    {
	      one = 1;
	    }
	}
      else
	{
	  if (remarks != NULL)
	    *remarks = *remarks | remark_spurious_character;
	}

      ch = my_fgetc(f);
    }

  return srec_result_eof;
}

/*
 * srecord_checksum()
 *
 * Computes the checksum of the passed buffer and compares it with the in-buffer checksum.
 *
 * return:
 * -6 : checksum wrong
 * -3 : bad arguments
 *  0 : checksum correct
 */

static int srecord_checksum (int length, unsigned char * buf)
{
  int idx;
  unsigned char ck;
  unsigned int cksum;

  if (length < 3) return srec_result_bad_arg;
  if (buf == NULL) return srec_result_bad_arg;

  cksum = length;

  --length;
  for (idx = 0; idx < length; ++idx)
    cksum += buf[idx];

  ck = (cksum & 0xFF) ^ 0xFF;

  // printf("mychk = %2.2x  srecchk[%d] = %2.2x\n", ck, length, buf[length]);

  if (ck != buf[length]) return srec_result_checksum;
  return srec_result_ok;
}

/*
 * read_srecord()
 *
 * Reads a complete S-Record from the given file. The caller must pass a valid pointer
 * to a struct srecord in the parameter srec. This function will fill out the passed
 * struct, including allocating a data buffer accessible through the member buf. The
 * member data is a pointer into the allocated buffer which skips the address field
 * (i.e. data points at the first data byte in the buffer pointed to by buf).
 *
 * return:
 * -5 : malloc failed
 * -4 : more bytes read than record length
 * -3 : Bad arguments
 * -1 : EOF
 *  0 : no error
 */

int read_srecord (FILE * f, struct srecord * srec, int * remarks)
{
  int result;
  int local_remarks;

  if (f == NULL) return srec_result_bad_arg;
  if (srec == NULL) return srec_result_bad_arg;

  if (remarks != NULL) *remarks = 0;

  srec->type = find_srecord(f, &local_remarks);

  if (remarks != NULL) *remarks = *remarks | local_remarks;
  if (srec->type < 0) return srec->type;

  srec->length = srecord_length(f, &local_remarks);

  if (remarks != NULL) *remarks = *remarks | local_remarks;
  if (srec->length < 0) return srec->length;

  srec->buf = (unsigned char *)malloc(srec->length);
  if (srec->buf == NULL) return srec_result_malloc;

  result = read_srecord_data(f, srec->length, srec->buf, &local_remarks);

  if (remarks != NULL) *remarks = *remarks | local_remarks;
  if (result < 0) return result;

  result = srecord_checksum(srec->length, srec->buf);

  if (result < 0) return result;

  switch (srec->type)
    {
    case 0: case 1: case 9:
      srec->address = (256 * srec->buf[0]) + srec->buf[1];
      srec->length = srec->length - 3;
      srec->data = srec->buf + 2;
      break;

    case 2: case 8:
      srec->address = (256 * 256 * srec->buf[0]) + (256 * srec->buf[1]) + srec->buf[2];
      srec->length = srec->length - 4;
      srec->data = srec->buf + 3;
      break;

    case 3: case 7:
      srec->address = (256 * 256 * 256 * srec->buf[0]) + (256 * 256 * srec->buf[1]) + (256 * srec->buf[2]) + srec->buf[3];
      srec->length = srec->length - 5;
      srec->data = srec->buf + 4;
      break;

    case 5:
      /* no data bytes, don't know how many address bytes */
      srec->data = NULL;
      srec->address = 0;
      for (result = srec->length - 2; result >= 0; --result)
	{
	  srec->address = 256 * srec->address;
	  srec->address = srec->address + srec->buf[result];
	}
    }

  return srec_result_ok;
}

/* end */
