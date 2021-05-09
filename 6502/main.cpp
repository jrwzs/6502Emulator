#include <stdio.h>
#include <stdlib.h>

// http://www.obelisk.me.uk/6502/
using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;
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
	void WriteWord(Word Value, u32 Address,u32& Cycles)
	{
		Data[Address] = Value&0xFF;
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
	Byte FetchByte(u32& Cycles, Mem& memory)
	{
		Byte Data  = memory[PC];
		PC++;
		Cycles--;
		return Data;
	}
	Byte ReadByte(Byte address,u32& Cycles, Mem& memory)
	{
		Byte Data = memory[address];
		Cycles--;
		return Data;
	}
	Word FetchWord(u32& Cycles, Mem& memory)
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



	static constexpr Byte INS_LDA_IM = 0xA9;
	static constexpr Byte INS_LDA_ZP = 0xA5;
	static constexpr Byte INS_LDA_ZPX = 0xB5;
	static constexpr Byte INS_JSR = 0x20;
	void LDASetStatus()
	{
		Z = (A == 0);
		N = (A & 0b10000000) > 0;
	}

	void Execute(u32 Cycles, Mem& memory)
	{
		while (Cycles > 0)
		{
			Byte Ins = FetchByte(Cycles, memory);
			switch (Ins)
			{
			case INS_LDA_IM: //����Ѱַ Immediate
			{
				Byte Value = FetchByte(Cycles, memory);
				A = Value;
				Cycles--;
				LDASetStatus();
			}break;
			case INS_LDA_ZP: //��ҳѰַ ..zero page
			{
				Byte ZeroPageAddress = FetchByte(Cycles, memory);
				A = ReadByte(ZeroPageAddress, Cycles, memory);
				LDASetStatus();
			}break;
			case INS_LDA_ZPX: //��ҳѰַ ..zero page
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
			case INS_JSR:
			{
				Word SubAddress =  FetchWord(Cycles, memory); //��������
				memory.WriteWord(PC - 1, SP, Cycles); //��������
				SP += 2;
 				PC = SubAddress;
				Cycles--;
			}break;
			default:
			{
				printf("û�����ô���ָ��\n");
			}break;


			}
		}
	}

};
int main()
{
	Mem mem;
	CPU cpu;
	cpu.Reset(mem);
	//��ʼ  
	mem[0xFFFC] = CPU::INS_JSR;
	mem[0XFFFD] = 0x43;
	mem[0XFFFE] = 0x52;
	mem[0X5243] = CPU::INS_LDA_IM;
	mem[0x5244] = 0x62;
	//����
	cpu.Execute(9,mem);
	return 0;
}