#pragma once
//std
#include <fstream>
#include <string>
#include <vector>

#include "Util.hpp"
#include "VirtualMachine.hpp"

#include "string.h"

namespace alone {
	class Console {
	public:
		Console(VirtualMachine& vm) : m_vm(vm) {}

		int process(std::istream& is) {
			bool flag = true;

			while (true) {
				std::vector <std::string> splitted;
				std::string line;
				std::getline(is, line);

				splitted = string::split(line, ' ');
				if (splitted[0] == "exec") {
					this->_execute(splitted[1]);
				} else if (splitted[0] == "halt") {
					flag = false;
				}
			}

			return 0;
		}

	protected:
		VirtualMachine& m_vm;

		void _execute(std::string filename) {
			std::string text;
			std::fstream file(filename, std::ios::in);
			while (!file.eof()) {
				std::string line;
				std::getline(file, line);
				text += line + '\n';
			}
			file.close();

			text = alone::alg::cleanup(text);
			text = alone::alg::shrink(text);
			text = alone::alg::hex2ascii(text);
			auto code = alone::alg::string2vector(text);
			auto code_as_bytes = reinterpret_cast <std::vector <std::byte>*>(&code);

			this->m_vm.load(*code_as_bytes);
			this->m_vm.process();
			this->m_vm.freeGlobals();
		}
	};
}