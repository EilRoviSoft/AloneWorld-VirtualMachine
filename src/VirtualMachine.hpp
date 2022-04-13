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
	struct info_t {
		size_t retSize;
		size_t argSize;
	};
	using NestedTask = std::pair <info_t, Task>;

	struct Context {
		info::Flags* flags;
		size_t* ip = nullptr, * bp = nullptr, * sp = nullptr;
		size_t* offset, * length;
		std::byte* mem = nullptr;

		Context(class VirtualMachine& vm);

		template <class _T>
		_T& get(size_t adress) {
			return *reinterpret_cast <_T*>(&mem[adress]);
		}
	};

	class VirtualMachine {
		friend struct Context;
		friend class Settings;
	public:
		std::shared_ptr <Context> ctx = nullptr;

		VirtualMachine() {
			this->_memory.resize(info::LocMemSize);
			this->ctx.reset(new Context(*this));
		}

		void load(std::vector <std::byte> code) {
			//TODO: handle extra memory allocation to the program
			if (this->_memory.size() <= code.size())
				return void();

			for (size_t i = 0; i != code.size(); i++)
				this->_memory[i + info::RegMemSize - 0x10] = code[i];

			const size_t& offset = ctx->get <size_t>(info::POX), & length = ctx->get <size_t>(info::PLX);

			ctx->get <size_t>(info::IP) = offset;
			ctx->get <size_t>(info::SP) = offset + length;
			ctx->get <size_t>(info::BP) = offset + length;
		}
		void process() {
			this->ctx->flags->PAF = true;
			while (this->ctx->flags->PAF) {
				uint16_t inst_id = ctx->get <uint16_t>(*ctx->ip);
				this->_instructions.at(inst_id)(*this);
			}
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

		void freeGlobals() {
			for (auto it : this->_global)
				std::free((void*)it);
			this->_global.clear();
		}

		//instructions and functions

		void setInstruction(uint16_t id, Task instruction) {
			this->_instructions.emplace(id, instruction);
		}

		void setFunction(size_t id, info_t info, Task function) {
			this->_functions.emplace(id, NestedTask(info, function));
		}
		const Task& getFunction(size_t id) const {
			return this->_functions.at(id).second;
		}

	private:
		std::vector <std::byte> _memory;
		std::set <size_t> _global;
		std::unordered_map <size_t, NestedTask> _functions;
		std::unordered_map <uint16_t, Task> _instructions;
	};
}