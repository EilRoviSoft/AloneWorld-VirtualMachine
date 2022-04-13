#pragma once
//std
#include <cstdint>

namespace alone::info {
	const size_t InstructionSize = sizeof(uint16_t);
	const size_t PointerSize = sizeof(uint64_t);

	const size_t RegMemSize = 0x80;
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

		GRS = 0x40, //General registers start

		PLX = RegMemSize - 0x10, //Program length register
		POX = RegMemSize - 0x08, //Program offset register
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