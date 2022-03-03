/*
  instructions.h
  Copyright 2000,2001 by William Sheldon Simms III

  This file is a part of open apple -- a free Apple II emulator.
 
  Open apple is free software; you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software Foundation; either
  version 2, or (at your option) any later version.
 
  Open apple is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License along with open apple;
  see the file COPYING. If not, visit the Free Software Foundation website at http://www.fsf.org
*/

#ifndef INCLUDED_INSTRUCTIONS_H
#define INCLUDED_INSTRUCTIONS_H

#define F_RESET  0x1
#define F_IRQ    0x2
#define F_NMI    0x4

#define IRQ_VECTOR 0xfffe
#define NMI_VECTOR 0xfffa
#define RES_VECTOR 0xfffc

#define EXECUTE(opcode) \
do { \
  instruction_table[(opcode)](); \
  cycle_clock += instruction_cycles[(opcode)]; \
} while (0)

extern unsigned char A, X, Y, P, S;
extern unsigned char N, V, B, D, I, Z, C;
extern unsigned char interrupt_flags;
extern unsigned short emPC;
extern unsigned long long cycle_clock;

unsigned char build_P (void);
void unbuild_P (unsigned char status);

void interrupt (unsigned short vector, unsigned char flag);

/* 65c02 instructions */

void i00_BRK( void );
void i01_ORA( void );
void i02_NOP( void );
void i04_TSB( void );
void i05_ORA( void );
void i06_ASL( void );
void i07_RMB( void );
void i08_PHP( void );
void i09_ORA( void );
void i0A_ASL( void );
void i0C_TSB( void );
void i0D_ORA( void );
void i0E_ASL( void );
void i0F_BBR( void );
void i10_BPL( void );
void i11_ORA( void );
void i12_ORA( void );
void i14_TRB( void );
void i15_ORA( void );
void i16_ASL( void );
void i17_RMB( void );
void i18_CLC( void );
void i19_ORA( void );
void i1A_INC( void );
void i1C_TRB( void );
void i1D_ORA( void );
void i1E_ASL( void );
void i1F_BBR( void );
void i20_JSR( void );
void i21_AND( void );
void i24_BIT( void );
void i25_AND( void );
void i26_ROL( void );
void i27_RMB( void );
void i28_PLP( void );
void i29_AND( void );
void i2A_ROL( void );
void i2C_BIT( void );
void i2D_AND( void );
void i2E_ROL( void );
void i2F_BBR( void );
void i30_BMI( void );
void i31_AND( void );
void i32_AND( void );
void i34_BIT( void );
void i35_AND( void );
void i36_ROL( void );
void i37_RMB( void );
void i38_SEC( void );
void i39_AND( void );
void i3A_DEC( void );
void i3C_BIT( void );
void i3D_AND( void );
void i3E_ROL( void );
void i3F_BBR( void );
void i40_RTI( void );
void i41_EOR( void );
void i45_EOR( void );
void i46_LSR( void );
void i47_RMB( void );
void i48_PHA( void );
void i49_EOR( void );
void i4A_LSR( void );
void i4C_JMP( void );
void i4D_EOR( void );
void i4E_LSR( void );
void i4F_BBR( void );
void i50_BVC( void );
void i51_EOR( void );
void i52_EOR( void );
void i55_EOR( void );
void i56_LSR( void );
void i57_RMB( void );
void i58_CLI( void );
void i59_EOR( void );
void i5A_PHY( void );
void i5C_NOP( void );
void i5D_EOR( void );
void i5E_LSR( void );
void i5F_BBR( void );
void i60_RTS( void );
void i61_ADC_binary( void );
void i61_ADC_decimal( void );
void i64_STZ( void );
void i65_ADC_binary( void );
void i65_ADC_decimal( void );
void i66_ROR( void );
void i67_RMB( void );
void i68_PLA( void );
void i69_ADC_binary( void );
void i69_ADC_decimal( void );
void i6A_ROR( void );
void i6C_JMP( void );
void i6D_ADC_binary( void );
void i6D_ADC_decimal( void );
void i6E_ROR( void );
void i6F_BBR( void );
void i70_BVS( void );
void i71_ADC_binary( void );
void i71_ADC_decimal( void );
void i72_ADC_binary( void );
void i72_ADC_decimal( void );
void i74_STZ( void );
void i75_ADC_binary( void );
void i75_ADC_decimal( void );
void i76_ROR( void );
void i77_RMB( void );
void i78_SEI( void );
void i79_ADC_binary( void );
void i79_ADC_decimal( void );
void i7A_PLY( void );
void i7C_JMP( void );
void i7D_ADC_binary( void );
void i7D_ADC_decimal( void );
void i7E_ROR( void );
void i7F_BBR( void );
void i80_BRA( void );
void i81_STA( void );
void i84_STY( void );
void i85_STA( void );
void i86_STX( void );
void i87_SMB( void );
void i88_DEY( void );
void i89_BIT( void );
void i8A_TXA( void );
void i8C_STY( void );
void i8D_STA( void );
void i8E_STX( void );
void i8F_BBS( void );
void i90_BCC( void );
void i91_STA( void );
void i92_STA( void );
void i94_STY( void );
void i95_STA( void );
void i96_STX( void );
void i97_SMB( void );
void i98_TYA( void );
void i99_STA( void );
void i9A_TXS( void );
void i9C_STZ( void );
void i9D_STA( void );
void i9E_STZ( void );
void i9F_BBS( void );
void iA0_LDY( void );
void iA1_LDA( void );
void iA2_LDX( void );
void iA4_LDY( void );
void iA5_LDA( void );
void iA6_LDX( void );
void iA7_SMB( void );
void iA8_TAY( void );
void iA9_LDA( void );
void iAA_TAX( void );
void iAC_LDY( void );
void iAD_LDA( void );
void iAE_LDX( void );
void iAF_BBS( void );
void iB0_BCS( void );
void iB1_LDA( void );
void iB2_LDA( void );
void iB4_LDY( void );
void iB5_LDA( void );
void iB6_LDX( void );
void iB7_SMB( void );
void iB8_CLV( void );
void iB9_LDA( void );
void iBA_TSX( void );
void iBC_LDY( void );
void iBD_LDA( void );
void iBE_LDX( void );
void iBF_BBS( void );
void iC0_CPY( void );
void iC1_CMP( void );
void iC4_CPY( void );
void iC5_CMP( void );
void iC6_DEC( void );
void iC7_SMB( void );
void iC8_INY( void );
void iC9_CMP( void );
void iCA_DEX( void );
void iCC_CPY( void );
void iCD_CMP( void );
void iCE_DEC( void );
void iCF_BBS( void );
void iD0_BNE( void );
void iD1_CMP( void );
void iD2_CMP( void );
void iD5_CMP( void );
void iD6_DEC( void );
void iD7_SMB( void );
void iD8_CLD( void );
void iD9_CMP( void );
void iDA_PHX( void );
void iDD_CMP( void );
void iDE_DEC( void );
void iDF_BBS( void );
void iE0_CPX( void );
void iE1_SBC_binary( void );
void iE1_SBC_decimal( void );
void iE4_CPX( void );
void iE5_SBC_binary( void );
void iE5_SBC_decimal( void );
void iE6_INC( void );
void iE7_SMB( void );
void iE8_INX( void );
void iE9_SBC_binary( void );
void iE9_SBC_decimal( void );
void iEA_NOP( void );
void iEC_CPX( void );
void iED_SBC_binary( void );
void iED_SBC_decimal( void );
void iEE_INC( void );
void iEF_BBS( void );
void iF0_BEQ( void );
void iF1_SBC_binary( void );
void iF1_SBC_decimal( void );
void iF2_SBC_binary( void );
void iF2_SBC_decimal( void );
void iF5_SBC_binary( void );
void iF5_SBC_decimal( void );
void iF6_INC( void );
void iF7_SMB( void );
void iF8_SED( void );
void iF9_SBC_binary( void );
void iF9_SBC_decimal( void );
void iFA_PLX( void );
void iFD_SBC_binary( void );
void iFD_SBC_decimal( void );
void iFE_INC( void );
void iFF_BBS( void );

#endif // #ifndef INCLUDED_INSTRUCTIONS_H
