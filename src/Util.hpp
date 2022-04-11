#pragma once
//std
#include <string>
#include <vector>

//alone
#include "Info.hpp"

namespace alone {
	namespace alg {
		std::string hex2ascii(const std::string& src);
		std::string ascii2hex(size_t number, size_t length);
		std::string cleanup(const std::string& src);
		std::string shrink(const std::string& src);
		std::vector <char> string2vector(const std::string& src);
	}
	namespace op {
		//compare operations
		bool _greater(const info::Flags& flags);
		bool _greater_or_equal(const info::Flags& flags);
		bool _lower(const info::Flags& flags);
		bool _lower_or_equal(const info::Flags& flags);
		bool _equal(const info::Flags& flags);
		bool _not_equal(const info::Flags& flags);

		//binary operators
		template <class _T>
		_T _add(const _T& lhs, const _T& rhs) {
			return lhs + rhs;
		}
		template <class _T>
		_T _sub(const _T& lhs, const _T& rhs) {
			return lhs - rhs;
		}
		template <class _T>
		_T _mul(const _T& lhs, const _T& rhs) {
			return lhs * rhs;
		}
		template <class _T>
		_T _div(const _T& lhs, const _T& rhs) {
			return lhs / rhs;
		}

		template <class _T>
		_T _mod(const _T& lhs, const _T& rhs) {
			return lhs % rhs;
		}
		template <class _T>
		_T _and(const _T& lhs, const _T& rhs) {
			return lhs & rhs;
		}
		template <class _T>
		_T _or(const _T& lhs, const _T& rhs) {
			return lhs | rhs;
		}
		template <class _T>
		_T _xor(const _T& lhs, const _T& rhs) {
			return lhs ^ rhs;
		}

		template <class _T>
		_T _shl(const _T& lhs, const _T& rhs) {
			return lhs << rhs;
		}
		template <class _T>
		_T _shr(const _T& lhs, const _T& rhs) {
			return lhs >> rhs;
		}
	}
}