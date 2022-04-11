#include "src/VirtualMachine.hpp"
#include "src/Instruction.hpp"
#include "src/Console.hpp"

void link(alone::VirtualMachine& vm) {
	//int getint()
	vm.setFunction(0x0012, [](alone::VirtualMachine vm) {
		auto& ctx = vm.ctx;
		int ch;
		char& heap = ctx->getLocal <char>(*ctx->sp);

		std::cin >> ch;
		heap = ch;
		*ctx->sp += 0x04;
	});
	//void putint(int)
	vm.setFunction(0x0013, [](alone::VirtualMachine vm) {
		auto& ctx = vm.ctx;
		int& heap = ctx->getLocal <int>(*ctx->sp - sizeof(int));
		
		std::cout << heap;
	});
}

int main() {
	alone::VirtualMachine vm;

	alone::inst::Settings settings(vm);
	settings.initSystemSet();
	settings.initOsSet();
	settings.initIntegerSets();

	link(vm);

	alone::Console console(vm);
	return console.process(std::cin);
}