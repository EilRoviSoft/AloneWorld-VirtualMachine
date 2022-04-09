#pragma once
//first 64 bytes in memory
namespace alone::info {
	const size_t RegMemSize = 0x50;
	//including registers
	const size_t LocMemSize = 0x1000;

	enum Register {
		AX = 0x00, //Adress register
		RX = 0x08, //Result register
		LCX = 0x10, //Left computing register
		RCX = 0x18, //Right computing register

		FX = 0x20, //Flags register
		IP = 0x28, //Instruction pointer register
		SP = 0x30, //Stack pointer register
		BP = 0x38, //Base pointer register

		PLX = 0x40, //Program length register
		POX = 0x48, //Program offset register

		//----------------------------------------

		Adress = 0x00,
		Result = 0x08,
		Left = 0x10,
		Right = 0x18,

		//Flags = 0x20,
		//Instruction = 0x28,
		//Stack = 0x30,
		//Base = 0x38,

		ProgramLength = 0x40,
		ProgramOffset = 0x48
	};

	struct Flags {
		bool PAF : 1; //Program active flag
		bool ZF : 1; //Zero flag
		bool SF : 1; //Sign flag
	};

	//using for bitwise operations
	//enum Flag {
	//	ZF = 1 << 0, //Zero flag
	//	SF = 1 << 1, //Sign flag
	//};
}