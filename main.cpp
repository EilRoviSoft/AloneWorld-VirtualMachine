#include "src/VirtualMachine.hpp"
#include "src/Instruction.hpp"
#include "src/Console.hpp"

std::istream* in;
std::ostream* out;

void link(alone::VirtualMachine& vm) {
	using namespace alone;

	//int getint()
	vm.setFunction(0x0012, { 0x04, 0x00 }, [](VirtualMachine vm) {
		auto& ctx = vm.ctx;
		int ch;
		int& heap = ctx->get <int>(*ctx->sp);

		*in >> ch;
		heap = ch;
		*ctx->sp += 0x04;
	});
	//void putint(int)
	vm.setFunction(0x0013, { 0x00, 0x04 }, [](VirtualMachine vm) {
		auto& ctx = vm.ctx;
		int& heap = ctx->get <int>(*ctx->sp - sizeof(int));
		
		*out << heap;
	});
	//void putstr(char*, size_t)
	vm.setFunction(0x0015, { 0x00, 0x10 }, [](VirtualMachine& vm) {
		auto& ctx = vm.ctx;
		size_t size = ctx->get <size_t>(*ctx->sp - sizeof(size_t) * 2);
		size_t adress = ctx->get <size_t>(*ctx->sp - sizeof(size_t));
		char& ptr = ctx->get <char>(adress);

		out->write(&ptr, size);
	});
}

int main() {
	alone::VirtualMachine vm;

	alone::inst::Settings settings(vm);
	settings.initSystemSet();
	settings.initOsSet();
	settings.initIntegerSets();

	link(vm);

	std::ifstream input("input.txt");

	in = &std::cin;
	out = &std::cout;

	alone::Console console(vm);
	int err = console.process(input);
	
	input.close();
	return err;
}