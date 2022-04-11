#include "Instruction.hpp"
#include "Util.hpp"

namespace alone::inst {
	namespace sys {
		//0x00

		//halt
		void _halt(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			ctx->flags->PAF = false;
		}

		//0x10 - 0x1F

		//ncall %adress
		void _ncall(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			size_t id = ctx->getLocal <size_t>(*ctx->ip + 0x02);
			vm.getFunction(id)(vm);

			*ctx->ip += 0x0A;
		}
		//call %adress
		void _fcall(VirtualMachine& vm) {}

		//ret
		void _ret(VirtualMachine& vm) {}

		//0x20 - 0x3F

		//jmp %adress
		void _jmp(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			size_t adress = ctx->getLocal <size_t>(*ctx->ip + 0x02);

			*ctx->ip = info::RegMemSize + adress;
		}
		//goto %adress
		void _goto(VirtualMachine& vm) {}

		//xxx: [eql, noq, grt, goq, lwr, loq]

		//jif_xxx %adress
		void _jif(VirtualMachine& vm, std::function <bool(const info::Flags&)> condition) {
			auto& ctx = vm.ctx;
			size_t adress = ctx->getLocal <size_t>(*ctx->ip + 0x02);

			if (condition(*ctx->flags))
				*ctx->ip = info::RegMemSize + adress;
		}
		//jif_xxx %adress_if, %adress_else
		void _jif_else(VirtualMachine& vm, std::function <bool(const info::Flags&)> condition) {
			auto& ctx = vm.ctx;
			size_t adress_if = ctx->getLocal <size_t>(*ctx->ip + 0x02);
			size_t adress_else = ctx->getLocal <size_t>(*ctx->ip + 0x0A);

			*ctx->ip = info::RegMemSize + (condition(*ctx->flags) ? adress_if : adress_else);
		}
	}
	void Settings::initSystemSet() {
		namespace ph = std::placeholders;

		this->m_vm.setInstruction(0x00, sys::_halt);
		
		this->m_vm.setInstruction(0x10, sys::_ncall);
		this->m_vm.setInstruction(0x11, sys::_fcall);
		this->m_vm.setInstruction(0x12, sys::_ret);

		this->m_vm.setInstruction(0x20, sys::_jmp);
		//TODO: goto

		this->m_vm.setInstruction(0x22, std::bind(sys::_jif, ph::_1, op::_equal));
		this->m_vm.setInstruction(0x23, std::bind(sys::_jif_else, ph::_1, op::_not_equal));
		this->m_vm.setInstruction(0x24, std::bind(sys::_jif, ph::_1, op::_greater));
		this->m_vm.setInstruction(0x25, std::bind(sys::_jif_else, ph::_1, op::_greater_or_equal));
		this->m_vm.setInstruction(0x26, std::bind(sys::_jif, ph::_1, op::_lower));
		this->m_vm.setInstruction(0x27, std::bind(sys::_jif_else, ph::_1, op::_lower_or_equal));
	}

	namespace os {
		void _malloc(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			size_t length = ctx->getLocal <size_t>(*ctx->ip + 0x02);
			size_t* compute = &ctx->getRegister <size_t>(info::AX);

			void* array = std::malloc(length);
			/*TODO:
			if (array == nullptr)
				throw(std::bad_alloc());*/

			*compute = reinterpret_cast <size_t>(array);
			vm.observe(*compute);

			*ctx->ip += 0x0A;
		}
		void _realloc(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			size_t adress = ctx->getLocal <size_t>(*ctx->ip + 0x02);
			size_t length = ctx->getLocal <size_t>(*ctx->ip + 0x0A);
			size_t* compute = &ctx->getRegister <size_t>(info::AX);
			void* array = (void*)adress;

			array = (std::byte*)std::realloc(array, length);
			/*TODO:
			if (array == nullptr)
				throw(std::bad_alloc());*/

			size_t _adress = reinterpret_cast <size_t>(array);
			if (adress != _adress) {
				*compute = _adress;
				vm.forget(adress, false);
				vm.observe(_adress);
			}

			*ctx->ip += 0x12;
		}
		void _free(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			size_t adress = ctx->getLocal <size_t>(*ctx->ip + 0x4);

			vm.forget(adress, true);

			*ctx->ip += 0x0A;
		}
	}
	void Settings::initOsSet() {
		this->m_vm.setInstruction(0x10, os::_malloc);
		this->m_vm.setInstruction(0x11, os::_realloc);
		this->m_vm.setInstruction(0x12, os::_free);
	}

	namespace mem {
		//assigment operations
		
		template <class _T>
		void _mov_const(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& var = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T val = ctx->getLocal <_T>(*ctx->ip + 0x0A);

			var = val;

			*ctx->ip += 0x12;
		}
		template <class _T>
		void _mov_var(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& lhs = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T& rhs = ctx->getLocal <_T>(*ctx->ip + 0x0A);

			lhs = rhs;

			*ctx->ip += 0x12;
		}

		//memory operations

		template <class _T>
		void _push_const(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& heap = ctx->getLocal <_T>(*ctx->sp);
			_T value = ctx->getLocal <_T>(*ctx->ip + 0x02);

			heap = value;

			*ctx->sp += sizeof(_T);
			*ctx->ip += 0x02 + sizeof(_T);
		}
		template <class _T>
		void _push_var(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& heap = ctx->getLocal <_T>(*ctx->sp);
			_T& var = ctx->getLocal <_T>(*ctx->ip + 0x02);

			heap = var;

			*ctx->sp += sizeof(_T);
			*ctx->ip += 0x0A;
		}

		template <class _T>
		void _pop_to_reg(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& heap = ctx->getLocal <_T>(*ctx->sp);
			_T& pop = ctx->getRegister <_T>(info::RX);

			pop = heap;
			heap = 0;

			*ctx->sp -= sizeof(_T);
			*ctx->ip += 0x02;
		}
		template <class _T>
		void _pop_to_var(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& heap = ctx->getLocal <_T>(*ctx->sp);
			_T& var = ctx->getLocal <_T>(*ctx->ip + 0x02);

			var = heap;
			heap = 0;

			*ctx->sp -= sizeof(_T);
			*ctx->ip += 0x02;
		}

		//global memory operations

		template <class _T>
		void _get(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& local = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T& global = *reinterpret_cast <_T*>(&ctx->getLocal <_T>(*ctx->ip + 0x0A));

			local = global;

			*ctx->ip += 0x12;
		}
		template <class _T>
		void _set(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& global = *reinterpret_cast <_T*>(&ctx->getLocal <_T>(*ctx->ip + 0x02));
			_T& local = ctx->getLocal <_T>(*ctx->ip + 0x0A);

			global = local;

			*ctx->ip += 0x12;
		}

		//compare

		template <class _T>
		void _cmp_const(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& var = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T val = ctx->getLocal <_T>(*ctx->ip + 0x0A);

			auto& flags = ctx->flags;
			auto compared = var - val;

			flags->SF = compared >> (sizeof(_T) * 8 - 1);
			flags->ZF = compared;

			*ctx->ip += 0x0A + sizeof(_T);
		}
		template <class _T>
		void _cmp_var(VirtualMachine& vm) {
			auto& ctx = vm.ctx;
			_T& lhs = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T& rhs = ctx->getLocal <_T>(*ctx->ip + 0x0A);

			auto& flags = ctx->flags;
			auto compared = lhs - rhs;

			flags->SF = compared >> (sizeof(_T) * 8 - 1);
			flags->ZF = compared;

			*ctx->ip += 0x12;
		}

		//binary operations

		template <class _T>
		void _binary_imm_const(VirtualMachine& vm, std::function <_T(const _T&, const _T&)> func) {
			auto& ctx = vm.ctx;
			_T& var = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T val = ctx->getLocal <_T>(*ctx->ip + 0x0A);

			var = func(var, val);

			*ctx->ip += 0x0A + sizeof(_T);
		}
		template <class _T>
		void _binary_imm_var(VirtualMachine& vm, std::function <_T(const _T&, const _T&)> func) {
			auto& ctx = vm.ctx;
			_T& lhs = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T& rhs = ctx->getLocal <_T>(*ctx->ip + 0x0A);

			lhs = func(lhs, rhs);

			*ctx->ip += 0x12;
		}
		template <class _T>
		void _binary_mov_const(VirtualMachine& vm, std::function <_T(const _T&, const _T&)> func) {
			auto& ctx = vm.ctx;
			_T& dest = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T& lhs = ctx->getLocal <_T>(*ctx->ip + 0x0A);
			_T rhs = ctx->getLocal <_T>(*ctx->ip + 0x12);

			dest = func(lhs, rhs);

			*ctx->ip += 0x12 + sizeof(_T);
		}
		template <class _T>
		void _binary_mov_var(VirtualMachine& vm, std::function <_T(const _T&, const _T&)> func) {
			auto& ctx = vm.ctx;
			_T& dest = ctx->getLocal <_T>(*ctx->ip + 0x02);
			_T& lhs = ctx->getLocal <_T>(*ctx->ip + 0x0A);
			_T& rhs = ctx->getLocal <_T>(*ctx->ip + 0x12);

			dest = func(lhs, rhs);

			*ctx->ip += 0x1A;
		}
		template <class _T>
		void _binary_stack(VirtualMachine& vm, std::function <_T(const _T&, const _T&)> func) {
			auto& ctx = vm.ctx;
			_T& lhs = ctx->getLocal <_T>(*ctx->sp - sizeof(_T) * 2);
			_T& rhs = ctx->getLocal <_T>(*ctx->sp - sizeof(_T));

			lhs = func(lhs, rhs);
			rhs = 0;

			*ctx->sp -= sizeof(_T);
			*ctx->ip += 0x02;
		}
		template <class _T>
		void _binary_reg(VirtualMachine& vm, std::function <_T(const _T&, const _T&)> func) {
			auto& ctx = vm.ctx;
			_T& res = ctx->getRegister <_T>(info::RX);
			_T& lhs = ctx->getRegister <_T>(info::LCX);
			_T& rhs = ctx->getRegister <_T>(info::RCX);

			res = func(lhs, rhs);

			*ctx->ip += 0x02;
		}
	}
	template <class _T, size_t StartId>
	void Settings::initIntTypeSet() {
		namespace ph = std::placeholders;
		static const std::vector <std::function <_T(const _T&, const _T&)>> binary_funcs = {
			op::_add <_T>, op::_sub <_T>, op::_mul <_T>, op::_div <_T>,
			op::_mod <_T>, op::_and <_T>, op::_or <_T>, op::_xor <_T>,
			op::_shl <_T>, op::_shr <_T>
		};

		this->m_vm.setInstruction(StartId + 0x00, mem::_mov_const <_T>);
		this->m_vm.setInstruction(StartId + 0x01, mem::_mov_var <_T>);

		this->m_vm.setInstruction(StartId + 0x02, mem::_push_const <_T>);
		this->m_vm.setInstruction(StartId + 0x03, mem::_push_var <_T>);

		this->m_vm.setInstruction(StartId + 0x04, mem::_pop_to_reg <_T>);
		this->m_vm.setInstruction(StartId + 0x05, mem::_pop_to_var <_T>);

		this->m_vm.setInstruction(StartId + 0x06, mem::_get <_T>);
		this->m_vm.setInstruction(StartId + 0x07, mem::_set <_T>);

		this->m_vm.setInstruction(StartId + 0x08, mem::_cmp_const <_T>);
		this->m_vm.setInstruction(StartId + 0x09, mem::_cmp_var <_T>);

		for (size_t i = 0; i != binary_funcs.size(); i++) {
			this->m_vm.setInstruction(StartId + i * 0x06 + 0x0A, std::bind(mem::_binary_imm_const <_T>, ph::_1, binary_funcs.at(i)));
			this->m_vm.setInstruction(StartId + i * 0x06 + 0x0B, std::bind(mem::_binary_imm_var <_T>, ph::_1, binary_funcs.at(i)));
			this->m_vm.setInstruction(StartId + i * 0x06 + 0x0C, std::bind(mem::_binary_mov_const <_T>, ph::_1, binary_funcs.at(i)));
			this->m_vm.setInstruction(StartId + i * 0x06 + 0x0D, std::bind(mem::_binary_mov_var <_T>, ph::_1, binary_funcs.at(i)));
			this->m_vm.setInstruction(StartId + i * 0x06 + 0x0E, std::bind(mem::_binary_reg <_T>, ph::_1, binary_funcs.at(i)));
			this->m_vm.setInstruction(StartId + i * 0x06 + 0x0F, std::bind(mem::_binary_stack <_T>, ph::_1, binary_funcs.at(i)));
		}

		//neg
		//not
	}
	void Settings::initIntegerSets() {
		this->initIntTypeSet <uint8_t, 0x100>();
		this->initIntTypeSet <int8_t, 0x180>();
		this->initIntTypeSet <uint16_t, 0x200>();
		this->initIntTypeSet <int16_t, 0x280>();
		this->initIntTypeSet <uint32_t, 0x300>();
		this->initIntTypeSet <int32_t, 0x380>();
		this->initIntTypeSet <uint64_t, 0x400>();
		this->initIntTypeSet <int64_t, 0x480>();
	}
}