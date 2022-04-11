#include "src/VirtualMachine.hpp"
#include "src/Instruction.hpp"
#include "src/Console.hpp"

int main() {
	alone::VirtualMachine vm;

	alone::inst::Settings settings(vm);
	settings.initSystemSet();
	settings.initOsSet();
	settings.initIntegerSets();

	alone::Console console(vm);
	return console.process(std::cin);
}