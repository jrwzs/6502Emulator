#pragma once 
#include <stdio.h>
#include <stdlib.h>

// http://www.obelisk.me.uk/6502/
using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;
using s32 = signed int;
struct Mem
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
struct CPU
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

	static constexpr Byte   INS_LDA_IM = 0xA9,
							INS_LDA_ZP = 0xA5,
							INS_LDA_ZPX = 0xB5,
							INS_LDA_ABS = 0xAD,
							INS_LDA_ABS_X = 0xBD,
							INS_LDA_ABS_Y = 0xB9,
							INS_LDA_IND_X = 0x61,
							INS_LDA_IND_Y = 0xB1,
						    INS_JSR = 0x20;

	void LDASetStatus()
	{
		Z = (A == 0);
		N = (A & 0b10000000) > 0;
	}

	s32 Execute(s32 Cycles, Mem& memory)
	{
		s32 CyclesRequests = Cycles;
		while (Cycles > 0)
		{
			Byte Ins = FetchByte(Cycles, memory);
			switch (Ins)
			{
				case INS_LDA_IM: //����Ѱַ Immediate
				{
					Byte Value = FetchByte(Cycles, memory);
					A = Value;
					LDASetStatus();
				}break;
				case INS_LDA_ZP: //��ҳѰַ ..zero page
				{
					Byte ZeroPageAddress = FetchByte(Cycles, memory);
					A = ReadByte(ZeroPageAddress, Cycles, memory);
					LDASetStatus();
				}break;
				case INS_LDA_ZPX: //��ҳѰַ ..zero page x
				{
					Byte ZeroPageAddress = FetchByte(Cycles, memory);
					ZeroPageAddress += X;
					Cycles--;
					A = ReadByte(
						ZeroPageAddress,
						Cycles,
						memory);
					LDASetStatus();
				}break;
				case INS_LDA_ABS:
				{
					Word AbsAddress = FetchWord(Cycles, memory);
					A = ReadByte(
						AbsAddress,
						Cycles,
						memory);
					LDASetStatus();
				}break;
				case INS_LDA_ABS_X:
				{
					Word AbsAddress = FetchWord(Cycles, memory);
					auto Copy_AbsAddress = AbsAddress % 256;
					// �����и��õ�д����������ʱ����д https://github.com/davepoo/6502Emulator/blob/bb278fbf80d6e4f0f3bb60826afc2551977d9ead/6502/6502Lib/src/private/m6502.cpp#L1068 
					AbsAddress += X;
					if (Copy_AbsAddress + X > 254)
						Cycles--;
					A = ReadByte(
						AbsAddress,
						Cycles,
						memory);
					LDASetStatus();
				}break;
				case INS_LDA_ABS_Y:
				{
					Word AbsAddress = FetchWord(Cycles, memory);
					auto Copy_AbsAddress = AbsAddress % 256;
					AbsAddress += Y;
					if (Copy_AbsAddress + Y > 254)
						Cycles--;
					A = ReadByte(
						AbsAddress,
						Cycles,
						memory);
					LDASetStatus();
				}break;
				case INS_LDA_IND_X: 
				{
					Byte IndAddress = FetchByte(Cycles, memory);
					IndAddress += X;
					Cycles--;
					Word ind_address_ = ReadWord(
						IndAddress,
						Cycles,
						memory);
					A = ReadByte(
						ind_address_,
						Cycles,
						memory);
					LDASetStatus();
				}break;
				case INS_LDA_IND_Y:
				{
					Byte IndAddress = FetchByte(Cycles, memory);
					Word ind_address_ = ReadWord(
						IndAddress,
						Cycles,
						memory);
					auto Copy_ind_address_ = ind_address_ % 256;
					ind_address_ += Y;
					if (Copy_ind_address_ + Y > 254)
						Cycles--;
					A = ReadByte(
						ind_address_,
						Cycles,
						memory);
					LDASetStatus();
				}break;
				case INS_JSR:
				{
					Word SubAddress = FetchWord(Cycles, memory); //��������
					memory.WriteWord(PC - 1, SP, Cycles); //��������
					SP += 2;
					PC = SubAddress;
					Cycles--;
				}break;
				default:
				{
					printf("û������ 0x%x ָ��\n",Ins);
					Cycles++;//����ǰ���ʹ����һ�����ڶ�ȡ�������ݣ�����Ҫ�ӻ���
					goto finish;
				}break;
			}
		}
finish:
		return CyclesRequests - Cycles ;
	}

};