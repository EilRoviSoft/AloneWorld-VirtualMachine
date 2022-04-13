#include "VirtualMachine.hpp"

namespace alone {
	Context::Context(VirtualMachine& vm) {
		mem = vm._memory.data();

		flags = reinterpret_cast <info::Flags*>(mem + info::FX);
		ip = reinterpret_cast <size_t*>(mem + info::IP);
		sp = reinterpret_cast <size_t*>(mem + info::SP);
		bp = reinterpret_cast <size_t*>(mem + info::BP);

		offset = reinterpret_cast <size_t*>(mem + info::POX);
		length = reinterpret_cast <size_t*>(mem + info::PLX);
	}
}