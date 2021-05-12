#include "main_6502.h"
using namespace m6502;
s32 CPU::Execute(s32 Cycles, Mem& memory)
{
	s32 CyclesRequests = Cycles;
	while (Cycles > 0)
	{
		Byte Ins = FetchByte(Cycles, memory);
		switch (Ins)
		{
		case INS_LDA_IM: //����Ѱַ Immediate
		{
			A =  FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDX_IM: //����Ѱַ Immediate
		{
			X = FetchByte(Cycles, memory);
			SetStatus(X);
		}break;
		case INS_LDY_IM: //����Ѱַ Immediate
		{
			Y = FetchByte(Cycles, memory);
			SetStatus(Y);
		}break;
		case INS_LDA_ZP: //��ҳѰַ ..zero page
		{
			A = AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDX_ZP: //��ҳѰַ ..zero page
		{
			X = AddZeroPage(Cycles, memory);

			SetStatus(X);
		}break;
		case INS_LDY_ZP: //��ҳѰַ ..zero page
		{
			Y = AddZeroPage(Cycles, memory);
			SetStatus(Y);
		}break;
		case INS_LDA_ZPX: //��ҳѰַ ..zero page x
		{
			A = AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_LDX_ZPY: //��ҳѰַ ..zero page x
		{
			X = AddZeroPageAdd(Cycles, memory, Y);
			SetStatus(X);
		}break;
		case INS_LDY_ZPX: //��ҳѰַ ..zero page x
		{
			Y = AddZeroPageAdd(Cycles, memory, X);
			SetStatus(Y);
		}break;
		case INS_LDA_ABS:
		{
			A = AbsoluteAddress(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDX_ABS:
		{
			X = AbsoluteAddress(Cycles, memory);
			SetStatus(X);
		}break;
		case INS_LDY_ABS:
		{
			Y = AbsoluteAddress(Cycles, memory);
			SetStatus(Y);
		}break;
		case INS_LDA_ABS_X:
		{
			A = AbsoluteAddress_Register( Cycles,memory, X);
			SetStatus(A);
		}break;
		case INS_LDA_ABS_Y:
		{
			A = AbsoluteAddress_Register(Cycles, memory, Y);
			SetStatus(A);
		}break;
		case INS_LDX_ABS_Y:
		{
			X = AbsoluteAddress_Register(Cycles, memory, Y);
			SetStatus(X);
		}break;
		case INS_LDY_ABS_X:
		{
			Y = AbsoluteAddress_Register(Cycles, memory, X);
			SetStatus(Y);
		}break;
		case INS_LDA_IND_X:
		{
			A = IndirectAddressX( Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDA_IND_Y:
		{
			A = IndirectAddressY(Cycles, memory);
			SetStatus(A);
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
			printf("û������ 0x%x ָ��\n", Ins);
			Cycles++;//����ǰ���ʹ����һ�����ڶ�ȡ�������ݣ�����Ҫ�ӻ���
			goto finish;
		}break;
		}
	}
finish:
	return CyclesRequests - Cycles;
}

Byte CPU::AddZeroPage(s32& Cycles, Mem& memory)
{
	return ReadByte( FetchByte(Cycles, memory) , Cycles, memory);
}
Byte CPU::AddZeroPageAdd(s32& Cycles, Mem& memory, Byte _register)
{
	Byte ZeroPageAddress = FetchByte(Cycles, memory);
	ZeroPageAddress += _register;
	Cycles--;
	return  ReadByte(ZeroPageAddress, Cycles, memory);
}
Byte CPU::AbsoluteAddress(s32& Cycles, Mem& memory)
{
	return ReadByte(FetchWord(Cycles, memory),Cycles,memory);
}
Byte CPU::AbsoluteAddress_Register(s32& Cycles, Mem& memory,Byte _register)
{
	Word AbsAddress = FetchWord(Cycles, memory);
	auto Copy_AbsAddress = AbsAddress % 256;
	// �����и��õ�д����������ʱ����д https://github.com/davepoo/6502Emulator/blob/bb278fbf80d6e4f0f3bb60826afc2551977d9ead/6502/6502Lib/src/private/m6502.cpp#L1068 
	AbsAddress += _register;
	if (Copy_AbsAddress + _register > 254)
		Cycles--;
	return ReadByte(AbsAddress,Cycles,memory);
}

Byte CPU::IndirectAddressX(s32& Cycles, Mem& memory)
{
	Byte IndAddress = FetchByte(Cycles, memory);
	IndAddress += X;
	Cycles--;
	Word ind_address_ = ReadWord(
		IndAddress,
		Cycles,
		memory);
	return  ReadByte(ind_address_,Cycles,memory);
}
Byte CPU::IndirectAddressY(s32& Cycles, Mem& memory)
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
	return  ReadByte(ind_address_, Cycles, memory);
}

