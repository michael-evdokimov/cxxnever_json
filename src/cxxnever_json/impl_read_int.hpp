#pragma once
#include <string>
#include <cstring>


namespace cxxnever::json::impl
{

struct reader_int_t
{
	template<typename T>
	bool read(T& value, std::string_view input)
	{
		const char* p = &*input.begin();
		const char* end = &*input.end();

		value = 0;

		int8_t sign = 1;
		if (p != end)
			if (*p == '-')
				sign = -1, ++p;

		if (sign == -1)
			if (T(-1) > 0)
				return false;

		if (!(p != end && '0' <= *p && *p <= '9'))
			return false;

		while (p != end && '0' <= *p && *p <= '9') {
			T saved = value;
			value = value * 10 + (*p++ - '0') * sign;
			if constexpr(T(0.1) == 0)
				if (value / 10 != saved)
					return false;
		}

		if constexpr(T(0.1)) {
			if (p != end && *p == '.') {
				p++;
				T i = T(0.1) * sign;
				while (p != end && '0' <= *p && *p <= '9')
					value += i * (*p++ - '0'), i /= 10;
			}
		} else {
			if (p != end && *p == '.')
				return false;
		}

		if (p != end && (*p == 'e' || *p == 'E')) {
			int exp = 0;
			int exp_sign = 1;
			++p;
			if (p == end)
				return false;
			else if (*p == '+')
				exp_sign = 1;
			else if (*p == '-')
				exp_sign = -1;
			else
				return false;
			++p;
			while (p != end && '0' <= *p && *p <= '9')
				exp = exp * 10 + (*p++ - '0') * exp_sign;
			while (exp > 0)
				value *= 10, exp--;
			while (exp < 0) {
				if constexpr(T(0.1) == 0)
					if (value % 10)
						return false;
				value /= 10, exp++;
			}
		}

		if (p != end)
			if (!strchr(",}]\x20\t\n", *p))
				return false;

		return true;
	}
};

}
