#pragma once 
#include <stdio.h>
#include <stdlib.h>

// http://www.obelisk.me.uk/6502/
namespace m6502
{
	using Byte = unsigned char;
	using Word = unsigned short;
	using u32 = unsigned int;
	using s32 = signed int;

	struct Mem;
	struct CPU;
}
struct m6502::Mem
{
	static constexpr u32 MAX_MEM = 1024 * 64;
	Byte Data[MAX_MEM];
	void Initialise()
	{
		for (u32 i = 0; i < MAX_MEM; i++) Data[i] = 0;
	}
	Byte operator[](u32 Address) const
	{
		return Data[Address];
	}
	Byte& operator[](u32 Address)
	{
		return Data[Address];
	}
	//д�������ֽ�
	void WriteWord(Word Value, u32 Address, s32& Cycles)
	{
		Data[Address] = Value & 0xFF;
		Data[Address + 1] = (Value >> 8);
		Cycles -= 2;
	}
};
struct m6502::CPU
{
	Word PC;
	Word SP;

	Byte A, X, Y;

	Byte C : 1;
	Byte Z : 1;
	Byte I : 1;
	Byte D : 1;
	Byte B : 1;
	Byte V : 1;
	Byte N : 1;

	void Reset(Mem& mem)
	{
		PC = 0xFFFC; //��PC�Ĵ�����ʼ�� LDX #$FF
		SP = 0x0100; //��ջ�Ĵ�����ʼ��
		C = Z = I = D = B = V = N = 0;		 //����־�Ĵ�����ʼ����
					 //ʵ������ĳ��ָ��ʵ�ֵĳ�ʼ����������ʱ������
					 //����D=0����д��CLD
		A = X = Y = 0;
		mem.Initialise();
	}
	Byte FetchByte(s32& Cycles, Mem& memory)
	{
		Byte Data = memory[PC];
		PC++;
		Cycles--;
		return Data;
	}
	Byte ReadByte(Byte address, s32& Cycles, Mem& memory)
	{
		Byte Data = memory[address];
		Cycles--;
		return Data;
	}
	Byte ReadByte(Word address, s32& Cycles, Mem& memory)
	{
		Byte Data = memory[address];
		Cycles--;
		return Data;
	}
	Word ReadWord(Word address, s32& Cycles, Mem& memory)
	{
		Word Data = memory[address];
		Data |= (memory[address+1] << 8);
		Cycles-=2;
		return Data;
	}
	Word ReadWord(Byte address, s32& Cycles, Mem& memory)
	{
		Word Data = memory[address];
		Data |= (memory[address + 1] << 8);
		Cycles -= 2;
		return Data;
	}
	Word FetchWord(s32& Cycles, Mem& memory)
	{
		//С�˴洢����  ����Ǵ�˿ڣ���Ҫ��������˳��
		Word Data = memory[PC];
		PC++;

		Cycles--;
		Data |= (memory[PC] << 8);
		PC++;

		Cycles--;
		return Data;
	}

	static constexpr Byte   //LDA
							INS_LDA_IM = 0xA9,
							INS_LDA_ZP = 0xA5,
							INS_LDA_ZPX = 0xB5,
							INS_LDA_ABS = 0xAD,
							INS_LDA_ABS_X = 0xBD,
							INS_LDA_ABS_Y = 0xB9,
							INS_LDA_IND_X = 0x61,
							INS_LDA_IND_Y = 0xB1,
							//LDX
							INS_LDX_IM = 0xA2,
							INS_LDX_ZP = 0xA6,
							INS_LDX_ZPY = 0xB6,
							INS_LDX_ABS = 0xAE,
							INS_LDX_ABS_Y = 0xBE,
							// LDY
							INS_LDY_IM = 0xA0,
							INS_LDY_ZP = 0xA4,
							INS_LDY_ZPX = 0xB4,
							INS_LDY_ABS = 0xAC,
							INS_LDY_ABS_X = 0xBC,
							
						    INS_JSR = 0x20;

	void SetStatus(const m6502::Byte _register)
	{
		Z = (_register == 0);
		N = (_register & 0b10000000) > 0;
	}

	/* CPU�ж�ָ���*/
	s32 Execute(s32 Cycles, Mem& memory);

	/* Zero Page������*/
	Byte AddZeroPage(s32& Cycles, Mem& memory);
	
	/* Zero Page + X or Y ������*/
	Byte AddZeroPageAdd(s32& Cycles, Mem& memory,Byte _register);

	/* Absolute address */
	Byte AbsoluteAddress(s32& Cycles, Mem& memory);

	/* Absolute address + register*/
	Byte AbsoluteAddress_Register(s32& Cycles, Mem& memory, Byte _register);

	/* Indirect address X */
	Byte IndirectAddressX(s32& Cycles, Mem& memory);

	/* Indirect address Y*/
	Byte IndirectAddressY(s32& Cycles, Mem& memory);
};