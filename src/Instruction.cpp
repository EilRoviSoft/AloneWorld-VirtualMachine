#include "Instruction.hpp"

namespace alone::inst {
	namespace sys {
		//0x00

		//halt
		void _halt(Context& ctx) {
			ctx.flags->PAF = false;
		}

		//0x10 - 0x1F

		//ncall %adress
		void _ncall(Context& ctx) {

		}
		//call %adress
		void _fcall(Context& ctx) {}

		//ret
		void _ret(Context& ctx) {}

		//0x20 - 0x3F

		//jmp %adress
		void _jmp(Context& ctx) {}
		//goto %adress
		void _goto(Context& ctx) {}

		//xxx: [eql, noq, grt, goq, lwr, loq]

		//jif_xxx %adress
		void _jif(Context& ctx) {}
		//jif_xxx %adress_if_true, %adress_if_false
		void _jif_else(Context& ctx) {}
	}
	void Settings::initSystemSet() {
		this->m_vm.setInstruction(0x00, sys::_halt);
		
		this->m_vm.setInstruction(0x10, sys::_ncall);
		this->m_vm.setInstruction(0x11, sys::_fcall);
		this->m_vm.setInstruction(0x12, sys::_ret);
	}
}