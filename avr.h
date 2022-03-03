/*
  avr.h
  Copyright © 2003 by William Sheldon Simms III
*/

#ifndef INCLUDED_AVR_H
#define INCLUDED_AVR_H

#define IDXREG_X 26
#define IDXREG_Y 28
#define IDXREG_Z 30

/* READ/WRITE prototypes */

unsigned char READ (unsigned long addr);
unsigned short READ_PROGRAM (unsigned long addr);
unsigned char READ_PROGRAM_BYTE (unsigned long waddr, unsigned int baddr);

void WRITE (unsigned long addr, unsigned char byte);

/* Macros to push bytes to and pull bytes from the stack */

#define PUSH(b) WRITE(sp--,(b))
#define POP()   READ(++sp)

extern unsigned char sbit[8];

#define C sbit[0]
#define Z sbit[1]
#define N sbit[2]
#define V sbit[3]
#define S sbit[4]
#define H sbit[5]
#define T sbit[6]
#define I sbit[7]

/* io r/w function pointer types */

typedef unsigned char (*iorfunc) (void);
typedef void (*iowfunc) (unsigned char);

/* the objects declared here are defined in avr.c */

extern unsigned char R[32];        /* AVR register file */
extern unsigned char sram[65536];  /* internal and external SRAM */

extern iorfunc ior[256];
extern iowfunc iow[256];

extern unsigned char EIND;
extern unsigned char RAMPX;
extern unsigned char RAMPY;
extern unsigned char RAMPZ;
extern unsigned char RAMPD;

extern long pc;
extern unsigned short sp;

enum dpsize
{
  DPSIZE_8,
  DPSIZE_16,
  DPSIZE_24
};

extern enum dpsize avr_datasize;
extern enum dpsize avr_pgmsize;

extern unsigned int breakok;
extern unsigned int dobreak;

extern unsigned int skip;
extern unsigned int sleeping;

extern unsigned int watchdog;
extern unsigned int watchdog_start;

extern unsigned int interrupt_flags;
extern unsigned long long cycle_clock;

/* device specific instruction hooks */

extern void (*ld_offset) (int srcreg, int idxreg, int offset);
extern void (*ld_predec) (int srcreg, int idxreg);
extern void (*ld_postinc) (int srcreg, int idxreg);

extern void (*st_offset) (int srcreg, int idxreg, int offset);
extern void (*st_predec) (int srcreg, int idxreg);
extern void (*st_postinc) (int srcreg, int idxreg);

extern void (*spm) (void);

/* avr instructions */

void ld_offset_8 (int dstreg, int idxreg, int offset);
void ld_offset_16 (int dstreg, int idxreg, int offset);
void ld_offset_24 (int dstreg, int idxreg, int offset);

void ld_predec_8 (int dstreg, int idxreg);
void ld_predec_16 (int dstreg, int idxreg);
void ld_predec_24 (int dstreg, int idxreg);

void ld_postinc_8 (int dstreg, int idxreg);
void ld_postinc_16 (int dstreg, int idxreg);
void ld_postinc_24 (int dstreg, int idxreg);

void lds (int dstreg);
void lpm (int dstreg);
void elpm (int dstreg);
void lpm_postinc (int dstreg);
void elpm_postinc (int dstreg);

void st_offset_8 (int dstreg, int idxreg, int offset);
void st_offset_16 (int dstreg, int idxreg, int offset);
void st_offset_24 (int dstreg, int idxreg, int offset);

void st_predec_8 (int dstreg, int idxreg);
void st_predec_16 (int dstreg, int idxreg);
void st_predec_24 (int dstreg, int idxreg);

void st_postinc_8 (int dstreg, int idxreg);
void st_postinc_16 (int dstreg, int idxreg);
void st_postinc_24 (int dstreg, int idxreg);

void sts (int srcreg);

void com (int reg);
void neg (int reg);
void inc (int reg);
void dec (int reg);
void asr (int reg);
void lsr (int reg);
void ror (int reg);

void or (int dstreg, unsigned char srcval);
void and (int dstreg, unsigned char srcval);
void eor (int dstreg, unsigned char srcval);

void cp (int dstreg, unsigned char srcval);
void cpc (int dstreg, unsigned char srcval);
void add (int dstreg, unsigned char srcval);
void sub (int dstreg, unsigned char srcval);
void adc (int dstreg, unsigned char srcval);
void sbc (int dstreg, unsigned char srcval);

void mul (int dstreg, unsigned char srcval);
void muls (int dstreg, unsigned char srcval);
void mulsu (int dstreg, unsigned char srcval);
void fmul (int dstreg, unsigned char srcval);
void fmuls (int dstreg, unsigned char srcval);
void fmulsu (int dstreg, unsigned char srcval);

void adiw (unsigned char imm, int reg);
void sbiw (unsigned char imm, int reg);

void call (long target);
void rcall (int offset);
void icall (void);
void eicall (void);

void ret (void);
void reti (void);

void brbc (int bidx, int offset);
void brbs (int bidx, int offset);

/* other functions */

void undefined_opcode (void);

#endif /* INCLUDED_AVR_H */
