#include "Util.hpp"
namespace alone {
	namespace alg {
		std::string hex2ascii(const std::string& src) {
			std::string result;
			result.resize(src.size() / 2);

			for (size_t i = 0; i != result.size(); i++)
				result[i] = std::stoi(src.substr(i * 2, 2), 0, 16);

			return result;
		}
		std::string ascii2hex(size_t number, size_t length) {
			static const char* digits = "0123456789ABCDEF";
			std::string result(length, '\0');

			for (size_t i = 0, j = (length - 1) * 4; i < length; i++, j += 4)
				result[i] = digits[(number >> j) & 0x0f];

			return result;
		}
		std::string cleanup(const std::string& src) {
			std::string result;
			result.reserve(src.size());

			bool flag = false;
			for (const auto& it : src) {
				if (it == ';')
					flag = true;
				if (flag && it == '\n')
					flag = false;
				if (flag)
					continue;
				result += it;
			}

			result.shrink_to_fit();
			return result;
		}
		std::string shrink(const std::string& src) {
			const std::string pattern = " \n\t";
			std::string result;
			result.reserve(src.size() - src.find(pattern));

			for (const auto& it : src)
				if (!pattern.contains(it))
					result += it;

			return result;
		}
		std::vector<char> string2vector(const std::string& src) {
			return std::vector <char>(src.begin(), src.end());
		}
	}
	namespace op {
		bool _greater(const info::Flags& flags) {
			return false;
		}
		bool _greater_or_equal(const info::Flags& flags) {
			return !flags.SF || flags.ZF;
		}
		bool _lower(const info::Flags& flags) {
			return flags.SF;
		}
		bool _lower_or_equal(const info::Flags& flags) {
			return flags.SF || flags.ZF;
		}
		bool _equal(const info::Flags& flags) {
			return flags.ZF;
		}
		bool _not_equal(const info::Flags& flags) {
			return !flags.ZF;
		}
	}
}