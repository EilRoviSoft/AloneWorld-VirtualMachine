#pragma once
//alone
#include "Info.hpp"
#include "VirtualMachine.hpp"

namespace alone::inst {
	class Settings {
	public:
		Settings(VirtualMachine& vm) : m_vm(vm) {}

		void initSystemSet();
		void initOsSet();
		template <class _T>
		void initTypeSet();

	protected:
		VirtualMachine& m_vm;
	};
}