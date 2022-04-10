#include "VirtualMachine.hpp"

namespace alone {
	Context::Context(VirtualMachine& vm) {
		mem = vm._memory.data();
		regmem = mem;
		localmem = mem + info::RegMemSize;

		flags = reinterpret_cast <info::Flags*>(regmem + info::FX);
		ip = reinterpret_cast <size_t*>(regmem + info::IP);
		sp = reinterpret_cast <size_t*>(regmem + info::SP);
		bp = reinterpret_cast <size_t*>(regmem + info::BP);

		offset = reinterpret_cast <size_t*>(regmem + info::POX);
		length = reinterpret_cast <size_t*>(regmem + info::PLX);
	}
}