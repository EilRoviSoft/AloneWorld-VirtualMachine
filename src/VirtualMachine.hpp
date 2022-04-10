#pragma once
//std
#include <memory>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>
#include <functional>

//alone
#include "Info.hpp"

namespace alone {
	using Task = std::function <void(class VirtualMachine&)>;

	struct Context {
		info::Flags* flags;
		size_t* ip = nullptr, * bp = nullptr, * sp = nullptr;
		size_t* offset, * length;
		std::byte* mem = nullptr, * regmem = nullptr, * localmem = nullptr;

		Context(class VirtualMachine& vm);

		template <class _T>
		_T& getRegister(info::Register reg) {
			return regmem[reg];
		}
		template <class _T>
		_T& getLocal(size_t adress) {
			return localmem[adress];
		}
	};

	class VirtualMachine {
		friend struct Context;
		friend class Settings;
	public:
		std::shared_ptr <Context> ctx = nullptr;

		VirtualMachine() {
			this->_Resize(info::LocMemSize);
			this->ctx.reset(new Context(*this));
		}

		void load(std::vector <std::byte> code) {
			//TODO: handle extra memory allocation to the program
			if (this->_memory.size() <= code.size())
				return void();

			for (size_t i = 0; i != code.size(); i++)
				this->_memory[i + info::RegMemSize - 0x10] = code[i];

			const size_t& offset = *reinterpret_cast <const size_t*>(info::POX), & length = *reinterpret_cast <const size_t*>(info::PLX);

			*reinterpret_cast <size_t*>(info::IP) = offset;
			*reinterpret_cast <size_t*>(info::SP) = offset + length;
			*reinterpret_cast <size_t*>(info::BP) = offset + length;
		}
		void process() {
			this->ctx->flags->PAF = true;
		}
		void stop() {
			this->ctx->flags->PAF = false;
		}

		//memory manipulating

		template <class _T>
		_T& at(size_t adress) {
			return reinterpret_cast <_T*>(&this->_memory[adress]);
		}

		void observe(size_t ptr) {
			this->_global.emplace(ptr);
		}
		void forget(size_t adress, bool isFree) {
			if (isFree)
				std::free((void*)adress);
			this->_global.erase(adress);
		}

		//instructions and functions

		void setInstruction(uint16_t id, Task instruction) {
			this->_instructions.emplace(id, instruction);
		}

		void setFunction(size_t id, Task function) {
			this->_functions.emplace(id, function);
		}
		const Task& getFunction(size_t id) const {
			return this->_functions.at(id);
		}

	private:
		std::vector <std::byte> _memory;
		std::set <size_t> _global;
		std::unordered_map <size_t, Task> _functions;
		std::unordered_map <uint16_t, Task> _instructions;

		void _Resize(size_t size) {
			this->_memory.resize(size);
		}
		void _Free() {

		}
	};
}