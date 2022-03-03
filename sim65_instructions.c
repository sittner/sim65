/*
  instructions.c
  Copyright 2000,2001,2003,2020 by William Sheldon Simms

  This file is a part of Sim65 -- a free 6502 simulator / debugger
 
  Sim65 is free software; you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software Foundation; either
  version 3, or (at your option) any later version.
 
  Sim65 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License along with Sim65;
  see the file COPYING. If not, visit the Free Software Foundation website at http://www.fsf.org
*/

#include <stdint.h>

#include "instructions.h"
#include "tables.h"

#ifdef OPENAPPLE
#include "apple_cpu.h"
#include "iie_memory.h"
#else
#include "AddressPeripheral.h"
#endif

/* 65c02 registers */

uint8_t  A;         /* Accumulator     */
uint8_t  X, Y;      /* Index Registers */
uint8_t  P = 0x34;  /* Status Byte     */
uint8_t  S;         /* Stack Pointer   */
uint8_t  C = 0, Z = 0, I = 1, D = 0, B = 1, V = 0, N = 0;
uint8_t  interrupt_flags = 0;
uint16_t emPC;

uint64_t cycle_clock = 0;

/* Abbreviations */

/* Macros to push bytes to and pull bytes from the 65c02 stack */

#define PUSH(b) (WRITE_STACK(S,(b)),--S)
#define PULL()  (++S,READ_STACK(S))

/* Addressing mode macros */

#define ABS(effaddr,index) do {				\
    uint16_t baseaddr;					\
    baseaddr = ((uint16_t)READ(emPC));			\
    emPC++;						\
    baseaddr += (((uint16_t)READ(emPC)) << 8);		\
    emPC++;						\
    (effaddr) = baseaddr;				\
    if (index) {					\
      (effaddr) += (uint16_t)(index);			\
      if ((effaddr & 0xFF00) != ((baseaddr) & 0xFF00))	\
	cycle_clock++;					\
    }							\
  } while(0)  

#define ABS_INDIRECT(effaddr,index) do {		\
    uint16_t baseaddr;					\
    ABS(baseaddr,index);				\
    (effaddr) = ((uint16_t)READ(baseaddr));		\
    (effaddr) += (((uint16_t)READ(baseaddr+1)) << 8);	\
  } while (0)

#define ZP(effaddr,index) do {					\
    (effaddr) = (uint16_t)(uint8_t)(READ(emPC) + (uint8_t)(index));	\
    emPC++;							\
  } while (0)

#define ZP_INDIRECT(effaddr,index) do {				\
    uint8_t zpaddr;						\
    zpaddr = READ(emPC) + (uint8_t)(index);			\
    emPC++;							\
    (effaddr) = ((uint16_t)READ((uint16_t)zpaddr));		\
    (effaddr) += (((uint16_t)READ((uint16_t)(zpaddr+1))) << 8);	\
  } while (0)  
  
/* Macro to set N and Z flags */

#define SET_FLAGS_NZ(a)     \
  N = ((a) & 0x80) ? 1 : 0; \
  Z = (a) ? 0 : 1;          \

/* 65c02 addressing modes */

static unsigned short imm (void)
{
  return emPC++;
}

static inline uint16_t zp_indirect (void)
{
  uint16_t effaddr;
  ZP_INDIRECT(effaddr,0);
  return effaddr;
}

static inline uint16_t indirect_x (void)
{
  uint16_t effaddr;
  ZP_INDIRECT(effaddr,X);
  return effaddr;
}

static inline uint16_t indirect_y (void)
{
  uint16_t baseaddr;
  uint16_t effaddr;
  ZP_INDIRECT(baseaddr,0);
  effaddr = baseaddr + (unsigned short)Y;
  if ((effaddr & 0xFF00) != (baseaddr & 0xFF00))
    cycle_clock++;
  return effaddr;
}

static inline uint16_t absolute (void)
{
  uint16_t effaddr;
  ABS(effaddr,0);
  return effaddr;
}

static inline uint16_t absolute_x (void)
{
  uint16_t effaddr;
  ABS(effaddr,X);
  return effaddr;
}

static inline uint16_t absolute_y (void)
{
  uint16_t effaddr;
  ABS(effaddr,Y);
  return effaddr;
}

static inline uint16_t zp (void)
{
  uint16_t effaddr;
  ZP(effaddr,0);
  return effaddr;
}

static inline uint16_t zp_x (void)
{
  uint16_t effaddr;
  ZP(effaddr,X);
  return effaddr;
}

static inline uint16_t zp_y (void)
{
  uint16_t effaddr;
  ZP(effaddr,Y);
  return effaddr;
}

/* 65c02 relative branch */

static inline void branch (void)
{
  uint16_t offset, target;

  offset = (uint16_t)((int16_t)((int8_t)READ(emPC)));
  emPC++;
  target = emPC + offset;

  ++cycle_clock;
  if ((emPC & 0xFF00) != (target & 0xFF00))
    ++cycle_clock;

  emPC = target;
}

static inline void branch_bit_reset (char bit)
{
  unsigned char zp_byte;
  unsigned char zp_addr;

  zp_addr = zp();
  zp_byte = READ(zp_addr);
  if (zp_byte & bit)
    ++emPC;
  else
    branch();
}

static inline void branch_bit_set (char bit)
{
  unsigned char zp_byte;
  unsigned char zp_addr;

  zp_addr = zp();
  zp_byte = READ(zp_addr);
  if (zp_byte & bit)
    branch();
  else
    ++emPC;
}

static inline void reset_memory_bit (char bit)
{
  unsigned char zp_byte;
  unsigned char zp_addr;

  zp_addr  = zp();
  zp_byte  = READ(zp_addr);
  zp_byte &= ~bit;

  WRITE(zp_addr, zp_byte);  
}

static inline void set_memory_bit (char bit)
{
  unsigned char zp_byte;
  unsigned char zp_addr;

  zp_addr  = zp();
  zp_byte  = READ(zp_addr);
  zp_byte |= bit;

  WRITE(zp_addr, zp_byte);  
}

/* 65c02 loads */

#define LOAD(reg,addr)				\
  do {						\
    uint16_t taddr = (addr);			\
    (reg) = READ(taddr);			\
    SET_FLAGS_NZ((reg));			\
  } while (0)

// stores use memory.h WRITE() macro directly

/* 65c02 ALU operations */

static inline void BIT (uint16_t addr)
{
  uint8_t byte = READ(addr);
  V = (byte & 0x40) ? 1 : 0;
  N = (byte & 0x80) ? 1 : 0;
  Z = (byte & A)    ? 0 : 1;
}

static inline void ORA (uint16_t addr)
{
  A |= READ(addr);
  SET_FLAGS_NZ(A);
}

static inline void EOR (uint16_t addr)
{
  A ^= READ(addr);
  SET_FLAGS_NZ(A);
}

static inline void AND (uint16_t addr)
{
  A &= READ(addr);
  SET_FLAGS_NZ(A);
}

static inline void ASL (uint16_t addr)
{
  uint8_t byte = READ(addr);
  C = (byte & 0x80) ? 1 : 0;
  byte <<= 1;
  SET_FLAGS_NZ(byte);
  WRITE(addr, byte);
}

static inline void LSR (uint16_t addr)
{
  uint8_t byte = READ(addr);
  C = byte & 0x01;
  byte >>= 1;
  N = 0;
  Z = byte ? 0 : 1;
  WRITE(addr, byte);
}

static inline void ROL (uint16_t addr)
{
  uint8_t byte   = READ(addr);
  uint8_t result = (byte << 1) | C;
  C = (byte & 0x80) ? 1 : 0;
  SET_FLAGS_NZ(result);
  WRITE(addr, result);
}

static inline void ROR (uint16_t addr)
{
  uint8_t byte   = READ(addr);
  uint8_t result = byte >> 1;
  result |= (C ? 0x80 : 0x00);
  C = byte & 0x01;
  SET_FLAGS_NZ(result);
  WRITE(addr, result);
}

static inline void CMP (uint8_t regval, uint16_t addr)
{
  uint8_t byte = READ(addr);
  C = (regval >= byte) ? 1 : 0;
  uint8_t result = regval - byte;
  SET_FLAGS_NZ(result);
}

static inline void INC (uint16_t addr)
{
  uint8_t byte = READ(addr) + 1;
  SET_FLAGS_NZ(byte);
  WRITE(addr, byte);
}

static inline void DEC (uint16_t addr)
{
  uint8_t byte = READ(addr) - 1;
  SET_FLAGS_NZ(byte);
  WRITE(addr, byte);
}

static inline void ADC_decimal (uint16_t addr)
{
  uint8_t ones, tens;
  uint8_t byte = READ(addr);

  /* add 'ones' digits */
  ones = (A & 0xF) + (byte & 0xF) + C;

  /* carry if needed */
  C = 0;
  if (ones >= 10)
    {
      C = 1;
      ones -= 10;
    }

  /* add 'tens' digits */
  tens = (A >> 4) + (byte >> 4) + C;

  /* carry if needed */
  C = 0;
  if (tens > 9)
    {
      C = 1;
      tens -= 10;
    }

  A = (tens << 4) + ones;

  /* set flags */
  V = C;
  SET_FLAGS_NZ(A);

  /* takes one cycle more than binary */
  ++cycle_clock;
}

static inline void SBC_decimal (uint16_t addr)
{
  uint8_t ones, tens;
  uint8_t byte = READ(addr);

  /* subtract 'ones' digits */
  ones = (A & 0xF) - (byte & 0xF) - (1 - C);

  /* borrow if needed */
  C = 1;
  if (ones & 0x80)
    {
      C = 0;
      ones -= 6;
    }
  
  /* subtract 'tens' digits */
  tens = (A >> 4) - (byte >> 4) - (1 - C);

  /* borrow if needed */
  C = 1;
  if (tens & 0x80)
    {
      C = 0;
      tens -= 6;
    }

  A = (tens << 4) + (ones & 0xF);
  
  /* set flags */
  V = C;
  SET_FLAGS_NZ(A);

  /* takes one cycle more than binary */
  ++cycle_clock;
}

static inline void ADC_binary (uint16_t addr)
{
  uint8_t high;
  uint8_t byte = READ(addr);

  high = A >> 7;
  A = A + byte + C;

  if (A & 0x80) /* A < 0 */
    {
      Z = 0;
      N = 1;

      if (byte & 0x80) /* byte < 0 */
	{
	  C = high;
	  V = 0;
	}
      else /* byte >= 0 */
	{
	  C = 0;
	  V = 1 ^ high;
	}
    }
  else /* A >= 0 */
    {
      N = 0;
      Z = A ? 0 : 1;

      if (byte & 0x80) /* byte < 0 */
	{
	  C = 1;
	  V = high;
	}
      else /* byte >= 0 */
	{
	  C = high;
	  V = 0;
	}
    }
}

static inline void SBC_binary (uint16_t addr)
{
  uint8_t high;
  uint8_t byte = READ(addr);

  high = A >> 7;
  A = A - byte - (1 - C);

  if (A & 0x80) /* A < 0 */
    {
      Z = 0;
      N = 1;

      if (byte & 0x80) /* byte < 0 */
	{
	  C = 0;
	  V = 1 ^ high;
	}
      else /* byte >= 0 */
	{
	  C = high;
	  V = 0;
	}
    }
  else /* A >= 0 */
    {
      N = 0;
      Z = A ? 0 : 1;

      if (byte & 0x80) /* byte < 0 */
	{
	  C = high;
	  V = 0;
	}
      else /* byte >= 0 */
	{
	  C = 1;
	  V = high;
	}
    }
}

/* Convert individual bits to P and reverse */

uint8_t build_P (void)
{
  P = 0x20;

  if (N) P |= 0x80;
  if (V) P |= 0x40;
  if (B) P |= 0x10;
  if (D) P |= 0x08;
  if (I) P |= 0x04;
  if (Z) P |= 0x02;
  if (C) P |= 0x01;

  return P;
}

void unbuild_P (uint8_t status)
{
  P = status;

  N = (P & 0x80) ? 1 : 0;
  V = (P & 0x40) ? 1 : 0;

  if (B == 0)
    B = (P & 0x10) ? 1 : 0;

  D = (P & 0x08) ? 1 : 0;
  I = (P & 0x04) ? 1 : 0;
  Z = (P & 0x02) ? 1 : 0;
  C = (P & 0x01) ? 1 : 0;

  if (D)
    instruction_table = decimal_instruction_table;
  else
    instruction_table = binary_instruction_table;
}

/* Generate an interrupt */

void interrupt (uint16_t vector, uint8_t flag)
{
  uint8_t byte;

  interrupt_flags &= (~flag);

  if (flag == F_RESET)
    {
      B = 1; /* is this for real? */
    }
  else
    {
      byte = emPC >> 8;
      PUSH(byte);
      byte = emPC & 0xFF;
      PUSH(byte);

      B = 0;
      byte = build_P();
      PUSH(byte);
    }

  I = 1;
  D = 0;
  instruction_table = binary_instruction_table;
  build_P();

  emPC = READ(vector);
  vector++;
  emPC = emPC + (READ(vector) << 8);

  cycle_clock += 7;
}

/* 65c02 instructions */

void i00_BRK (void)
{
  ++emPC;

  PUSH(emPC / 256);
  PUSH(emPC & 255);
    
  B = 1;
  PUSH(build_P());

  I = 1;
  D = 0;
  instruction_table = binary_instruction_table;

  emPC = READ(0xfffe) + (256 * READ(0xffff));
}

void i01_ORA (void)
{
  ORA(indirect_x());
}

void i02_NOP (void)
{
  /* Two-Byte NOP */
  emPC++;
}

void i04_TSB (void)
{
  unsigned char zp_byte;
  unsigned char zp_addr;

  zp_addr  = zp();
  zp_byte  = READ(zp_addr);
  Z        = (A & zp_byte) ? 0 : 1;
  zp_byte |= A;

  WRITE(zp_addr, zp_byte);
}

void i05_ORA (void)
{
  ORA(zp());
}

void i06_ASL (void)
{
  ASL(zp());
}

void i07_RMB (void)
{
  reset_memory_bit(0x01);
}

void i08_PHP (void)
{
  PUSH(build_P());
}

void i09_ORA (void)
{
  ORA(imm());
}

void i0A_ASL (void)
{
  C = (A & 0x80) ? 1 : 0;
  A <<= 1;
  SET_FLAGS_NZ(A);
}

void i0C_TSB (void)
{
  unsigned char  op_byte;
  unsigned short op_addr;

  op_addr  = absolute();
  op_byte  = READ(op_addr);
  Z        = (A & op_byte) ? 0 : 1;
  op_byte |= A;

  WRITE(op_addr, op_byte);
}

void i0D_ORA (void)
{
  ORA(absolute());
}

void i0E_ASL (void)
{
  ASL(absolute());
}

void i0F_BBR (void)
{
  branch_bit_reset(0x01);
}

void i10_BPL (void)
{
  if (N)
    ++emPC;
  else
    branch();
}

void i11_ORA (void)
{
  ORA(indirect_y());
}

void i12_ORA (void)
{
  ORA(zp_indirect());
}

void i14_TRB (void)
{
  unsigned char zp_addr;
  unsigned char zp_byte;

  zp_addr  = zp();
  zp_byte  = READ(zp_addr);
  Z        = (A & zp_byte) ? 0 : 1;
  zp_byte &= (~A);

  WRITE(zp_addr, zp_byte);
}

void i15_ORA (void)
{
  ORA(zp_x());
}

void i16_ASL (void)
{
  ASL(zp_x());
}

void i17_RMB (void)
{
  reset_memory_bit(0x02);
}

void i18_CLC (void)
{
  C = 0;
}

void i19_ORA (void)
{
  ORA(absolute_y());
}

void i1A_INC (void)
{
  ++A;
  SET_FLAGS_NZ(A);
}

void i1C_TRB (void)
{
  unsigned char  op_byte;
  unsigned short op_addr;

  op_addr  = absolute();
  op_byte  = READ(op_addr);
  Z        = (A & op_byte) ? 0 : 1;
  op_byte &= (~A);

  WRITE(op_addr, op_byte);
}

void i1D_ORA (void)
{
  ORA(absolute_x());
}

void i1E_ASL (void)
{
  ASL(absolute_x());
}

void i1F_BBR (void)
{
  branch_bit_reset(0x02);
}

void i20_JSR (void)
{
  unsigned short addr;
  addr = emPC + 1;
  PUSH(addr / 256);
  PUSH(addr & 255);
  emPC = absolute();
}

void i21_AND (void)
{
  AND(indirect_x());
}

void i24_BIT (void)
{
  BIT(zp());
}

void i25_AND (void)
{
  AND(zp());
}

void i26_ROL (void)
{
  ROL(zp());
}

void i27_RMB (void)
{
  reset_memory_bit(0x04);
}

void i28_PLP (void)
{
  unbuild_P(PULL());
}

void i29_AND (void)
{
  AND(imm());
}

void i2A_ROL (void)
{
  unsigned char result;

  result = (A << 1) | C;
  C      = (A & 0x80) ? 1 : 0;
  A      = result;

  SET_FLAGS_NZ(A);
}

void i2C_BIT (void)
{
  BIT(absolute());
}

void i2D_AND (void)
{
  AND(absolute());
}

void i2E_ROL (void)
{
  ROL(absolute());
}

void i2F_BBR (void)
{
  branch_bit_reset(0x04);
}

void i30_BMI (void)
{
  if (N)
    branch();
  else
    ++emPC;
}

void i31_AND (void)
{
  AND(indirect_y());
}

void i32_AND (void)
{
  AND(zp_indirect());
}

void i34_BIT (void)
{
  BIT(zp_x());
}

void i35_AND (void)
{
  AND(zp_x());
}

void i36_ROL (void)
{
  ROL(zp_x());
}

void i37_RMB (void)
{
  reset_memory_bit(0x08);
}

void i38_SEC (void)
{
  C = 1;
}

void i39_AND (void)
{
  AND(absolute_y());
}

void i3A_DEC (void)
{
  --A;
  SET_FLAGS_NZ(A);
}

void i3C_BIT (void)
{
  BIT(absolute_x());
}

void i3D_AND (void)
{
  AND(absolute_x());
}

void i3E_ROL (void)
{
  ROL(absolute_x());
}

void i3F_BBR (void)
{
  branch_bit_reset(0x08);
}

void i40_RTI (void)
{
  unbuild_P(PULL());
  emPC = PULL();
  emPC += (256 * PULL());
}

void i41_EOR (void)
{
  EOR(indirect_x());
}

void i45_EOR (void)
{
  EOR(zp());
}

void i46_LSR (void)
{
  LSR(zp());
}

void i47_RMB (void)
{
  reset_memory_bit(0x10);
}

void i48_PHA (void)
{
  PUSH(A);
}

void i49_EOR (void)
{
  EOR(imm());
}

void i4A_LSR (void)
{
  C = A & 0x01;
  A >>= 1;
  N = 0;
  Z = A ? 0 : 1;
}

void i4C_JMP (void)
{
  emPC = absolute();
}

void i4D_EOR (void)
{
  EOR(absolute());
}

void i4E_LSR (void)
{
  LSR(absolute());
}

void i4F_BBR (void)
{
  branch_bit_reset(0x10);
}

void i50_BVC (void)
{
  if (V)
    ++emPC;
  else
    branch();
}

void i51_EOR (void)
{
  EOR(indirect_y());
}

void i52_EOR (void)
{
  EOR(zp_indirect());
}

void i55_EOR (void)
{
  EOR(zp_x());
}

void i56_LSR (void)
{
  LSR(zp_x());
}

void i57_RMB (void)
{
  reset_memory_bit(0x20);
}

void i58_CLI (void)
{
  I = 0;
}

void i59_EOR (void)
{
  EOR(absolute_y());
}

void i5A_PHY (void)
{
  PUSH(Y);
}

void i5C_NOP (void)
{
  /* Three-Byte NOP */
  emPC+=2;
}

void i5D_EOR (void)
{
  EOR(absolute_x());
}

void i5E_LSR (void)
{
  LSR(absolute_x());
}

void i5F_BBR (void)
{
  branch_bit_reset(0x20);
}

void i60_RTS (void)
{
  emPC  = PULL();
  emPC += (256 * PULL());
  emPC += 1;
}

void i61_ADC_binary (void)
{
  ADC_binary(indirect_x());
}

void i61_ADC_decimal (void)
{
  ADC_decimal(indirect_x());
}

void i64_STZ (void)
{
  unsigned char zp_addr;
  zp_addr = zp();
  WRITE(zp_addr, 0);
}

void i65_ADC_binary (void)
{
  ADC_binary(zp());
}

void i65_ADC_decimal (void)
{
  ADC_decimal(zp());
}

void i66_ROR (void)
{
  ROR(zp());
}

void i67_RMB (void)
{
  reset_memory_bit(0x40);
}

void i68_PLA (void)
{
  A = PULL();
  SET_FLAGS_NZ(A);
}

void i69_ADC_binary (void)
{
  ADC_binary(imm());
}

void i69_ADC_decimal (void)
{
  ADC_decimal(imm());
}

void i6A_ROR (void)
{
  unsigned char result;

  result  = A >> 1;
  result |= (C ? 0x80 : 0x00);
  C       = A & 0x01;
  A       = result;

  SET_FLAGS_NZ(A);
}

void i6C_JMP (void)
{
  unsigned short addr;

  addr  = absolute();
  emPC  = READ(addr); addr++;
  emPC += (256 * READ(addr));
}

void i6D_ADC_binary (void)
{
  ADC_binary(absolute());
}

void i6D_ADC_decimal (void)
{
  ADC_decimal(absolute());
}

void i6E_ROR (void)
{
  ROR(absolute());
}

void i6F_BBR (void)
{
  branch_bit_reset(0x40);
}

void i70_BVS (void)
{
  if (V)
    branch();
  else
    ++emPC;
}

void i71_ADC_binary (void)
{
  ADC_binary(indirect_y());
}

void i71_ADC_decimal (void)
{
  ADC_decimal(indirect_y());
}

void i72_ADC_binary (void)
{
  ADC_binary(zp_indirect());
}

void i72_ADC_decimal (void)
{
  ADC_decimal(zp_indirect());
}

void i74_STZ (void)
{
  unsigned char zp_addr;
  zp_addr = zp_x();
  WRITE(zp_addr, 0);
}

void i75_ADC_binary (void)
{
  ADC_binary(zp_x());
}

void i75_ADC_decimal (void)
{
  ADC_decimal(zp_x());
}

void i76_ROR (void)
{
  ROR(zp_x());
}

void i77_RMB (void)
{
  reset_memory_bit(0x80);
}

void i78_SEI (void)
{
  I = 1;
}

void i79_ADC_binary (void)
{
  ADC_binary(absolute_y());
}

void i79_ADC_decimal (void)
{
  ADC_decimal(absolute_y());
}

void i7A_PLY (void)
{
  Y = PULL();
  SET_FLAGS_NZ(Y);
}

void i7C_JMP (void)
{
  unsigned short addr;

  addr  = absolute() + (unsigned short)X;
  emPC  = READ(addr); addr++;
  emPC += (256 * READ(addr));
}

void i7D_ADC_binary (void)
{
  ADC_binary(absolute_x());
}

void i7D_ADC_decimal (void)
{
  ADC_decimal(absolute_x());
}

void i7E_ROR (void)
{
  ROR(absolute_x());
}

void i7F_BBR (void)
{
  branch_bit_reset(0x80);
}

void i80_BRA (void)
{
  branch();
}

void i81_STA (void)
{
  unsigned short addr;
  addr = indirect_x();
  WRITE(addr, A);
}

void i84_STY (void)
{
  unsigned char zp_addr;
  zp_addr = zp();
  WRITE(zp_addr, Y);
}

void i85_STA (void)
{
  unsigned char zp_addr;
  zp_addr = zp();
  WRITE(zp_addr, A);
}

void i86_STX (void)
{
  unsigned char zp_addr;
  zp_addr = zp();
  WRITE(zp_addr, X);
}

void i87_SMB (void)
{
  set_memory_bit(0x01);
}

void i88_DEY (void)
{
  --Y;
  SET_FLAGS_NZ(Y);
}

void i89_BIT (void)
{
  /* doesn't use BIT() because in immediate addressing function is different */
  int addr = imm();
  Z = (READ(addr) & A) ? 0 : 1;
}

void i8A_TXA (void)
{
  A = X;
  SET_FLAGS_NZ(A);
}

void i8C_STY (void)
{
  unsigned short addr;
  addr = absolute();
  WRITE(addr, Y);
}

void i8D_STA (void)
{
  unsigned short addr;
  addr = absolute();
  WRITE(addr, A);
}

void i8E_STX (void)
{
  unsigned short addr;
  addr = absolute();
  WRITE(addr, X);
}

void i8F_BBS (void)
{
  branch_bit_set(0x01);
}

void i90_BCC (void)
{
  if (C)
    ++emPC;
  else
    branch();
}

void i91_STA (void)
{
  unsigned short addr;
  addr = indirect_y();
  WRITE(addr, A);
}

void i92_STA (void)
{
  unsigned short addr;
  addr = zp_indirect();
  WRITE(addr, A);
}

void i94_STY (void)
{
  unsigned char zp_addr;
  zp_addr = zp_x();
  WRITE(zp_addr, Y);
}

void i95_STA (void)
{
  unsigned char zp_addr;
  zp_addr = zp_x();
  WRITE(zp_addr, A);
}

void i96_STX (void)
{
  unsigned char zp_addr;
  zp_addr = zp_y();
  WRITE(zp_addr, X);
}

void i97_SMB (void)
{
  set_memory_bit(0x02);
}

void i98_TYA (void)
{
  A = Y;
  SET_FLAGS_NZ(A);
}

void i99_STA (void)
{
  unsigned short addr;
  addr = absolute_y();
  WRITE(addr, A);
}

void i9A_TXS (void)
{
  S = X;
}

void i9C_STZ (void)
{
  unsigned short addr;
  addr = absolute();
  WRITE(addr, 0);
}

void i9D_STA (void)
{
  unsigned short addr;
  addr = absolute_x();
  WRITE(addr, A);
}

void i9E_STZ (void)
{
  unsigned short addr;
  addr = absolute_x();
  WRITE(addr, 0);
}

void i9F_BBS (void)
{
  branch_bit_set(0x02);
}

void iA0_LDY (void)
{
  LOAD(Y,imm());
}

void iA1_LDA (void)
{
  LOAD(A,indirect_x());
}

void iA2_LDX (void)
{
  LOAD(X,imm());
}

void iA4_LDY (void)
{
  LOAD(Y,zp());
}

void iA5_LDA (void)
{
  LOAD(A,zp());
}

void iA6_LDX (void)
{
  LOAD(X,zp());
}

void iA7_SMB (void)
{
  set_memory_bit(0x04);
}

void iA8_TAY (void)
{
  Y = A;
  SET_FLAGS_NZ(Y);
}

void iA9_LDA (void)
{
  LOAD(A,imm());
}

void iAA_TAX (void)
{
  X = A;
  SET_FLAGS_NZ(X);
}

void iAC_LDY (void)
{
  LOAD(Y,absolute());
}

void iAD_LDA (void)
{
  LOAD(A,absolute());
}

void iAE_LDX (void)
{
  LOAD(X,absolute());
}

void iAF_BBS (void)
{
  branch_bit_set(0x04);
}

void iB0_BCS (void)
{
  if (C)
    branch();
  else
    ++emPC;
}

void iB1_LDA (void)
{
  LOAD(A,indirect_y());
}

void iB2_LDA (void)
{
  LOAD(A,zp_indirect());
}

void iB4_LDY (void)
{
  LOAD(Y,zp_x());
}

void iB5_LDA (void)
{
  LOAD(A,zp_x());
}

void iB6_LDX (void)
{
  LOAD(X,zp_y());
}

void iB7_SMB (void)
{
  set_memory_bit(0x08);
}

void iB8_CLV (void)
{
  V = 0;
}

void iB9_LDA (void)
{
  LOAD(A,absolute_y());
}

void iBA_TSX (void)
{
  X = S;
  SET_FLAGS_NZ(X);
}

void iBC_LDY (void)
{
  LOAD(Y,absolute_x());
}

void iBD_LDA (void)
{
  LOAD(A,absolute_x());
}

void iBE_LDX (void)
{
  LOAD(X,absolute_y());
}

void iBF_BBS (void)
{
  branch_bit_set(0x08);
}

void iC0_CPY (void)
{
  CMP(Y, imm());
}

void iC1_CMP (void)
{
  CMP(A, indirect_x());
}

void iC4_CPY (void)
{
  CMP(Y, zp());
}

void iC5_CMP (void)
{
  CMP(A, zp());
}

void iC6_DEC (void)
{
  DEC(zp());
}

void iC7_SMB (void)
{
  set_memory_bit(0x10);
}

void iC8_INY (void)
{
  Y++;
  SET_FLAGS_NZ(Y);
}

void iC9_CMP (void)
{
  CMP(A, imm());
}

void iCA_DEX (void)
{
  X--;
  SET_FLAGS_NZ(X);
}

void iCC_CPY (void)
{
  CMP(Y, absolute());
}

void iCD_CMP (void)
{
  CMP(A, absolute());
}

void iCE_DEC (void)
{
  DEC(absolute());
}

void iCF_BBS (void)
{
  branch_bit_set(0x10);
}

void iD0_BNE (void)
{
  if (Z)
    ++emPC;
  else
    branch();
}

void iD1_CMP (void)
{
  CMP(A, indirect_y());
}

void iD2_CMP (void)
{
  CMP(A, zp_indirect());
}

void iD5_CMP (void)
{
  CMP(A, zp_x());
}

void iD6_DEC (void)
{
  DEC(zp_x());
}

void iD7_SMB (void)
{
  set_memory_bit(0x20);
}

void iD8_CLD (void)
{
  D = 0;
  instruction_table = binary_instruction_table;
}

void iD9_CMP (void)
{
  CMP(A, absolute_y());
}

void iDA_PHX (void)
{
  PUSH(X);
}

void iDD_CMP (void)
{
  CMP(A, absolute_x());
}

void iDE_DEC (void)
{
  DEC(absolute_x());
}

void iDF_BBS (void)
{
  branch_bit_set(0x20);
}

void iE0_CPX (void)
{
  CMP(X, imm());
}

void iE1_SBC_binary (void)
{
  SBC_binary(indirect_x());
}

void iE1_SBC_decimal (void)
{
  SBC_decimal(indirect_x());
}

void iE4_CPX (void)
{
  CMP(X, zp());
}

void iE5_SBC_binary (void)
{
  SBC_binary(zp());
}

void iE5_SBC_decimal (void)
{
  SBC_decimal(zp());
}

void iE6_INC (void)
{
  INC(zp());
}

void iE7_SMB (void)
{
  set_memory_bit(0x40);
}

void iE8_INX (void)
{
  X++;
  SET_FLAGS_NZ(X);
}

void iE9_SBC_binary (void)
{
  SBC_binary(imm());
}

void iE9_SBC_decimal (void)
{
  SBC_decimal(imm());
}

void iEA_NOP (void)
{
  /* Do nothing! */
}

void iEC_CPX (void)
{
  CMP(X, absolute());
}

void iED_SBC_binary (void)
{
  SBC_binary(absolute());
}

void iED_SBC_decimal (void)
{
  SBC_decimal(absolute());
}

void iEE_INC (void)
{
  INC(absolute());
}

void iEF_BBS (void)
{
  branch_bit_set(0x40);
}

void iF0_BEQ (void)
{
  if (Z)
    branch();
  else
    ++emPC;
}

void iF1_SBC_binary (void)
{
  SBC_binary(indirect_y());
}

void iF1_SBC_decimal (void)
{
  SBC_decimal(indirect_y());
}

void iF2_SBC_binary (void)
{
  SBC_binary(zp_indirect());
}

void iF2_SBC_decimal (void)
{
  SBC_decimal(zp_indirect());
}

void iF5_SBC_binary (void)
{
  SBC_binary(zp_x());
}

void iF5_SBC_decimal (void)
{
  SBC_decimal(zp_x());
}

void iF6_INC (void)
{
  INC(zp_x());
}

void iF7_SMB (void)
{
  set_memory_bit(0x80);
}

void iF8_SED (void)
{
  D = 1;
  instruction_table = decimal_instruction_table;
}

void iF9_SBC_binary (void)
{
  SBC_binary(absolute_y());
}

void iF9_SBC_decimal (void)
{
  SBC_decimal(absolute_y());
}

void iFA_PLX (void)
{
  X = PULL();
  SET_FLAGS_NZ(X);
}

void iFD_SBC_binary (void)
{
  SBC_binary(absolute_x());
}

void iFD_SBC_decimal (void)
{
  SBC_decimal(absolute_x());
}

void iFE_INC (void)
{
  INC(absolute_x());
}

void iFF_BBS (void)
{
  branch_bit_set(0x80);
}

