#pragma once
#include "impl_read_int.hpp"
#include "impl_read_str.hpp"
#include "impl_read_bool.hpp"


namespace cxxnever::json::impl
{

struct reader_t
{
	template<typename T>
	bool read(T& value, std::string_view data)
	{
		if constexpr(std::is_same_v<T, bool>) {
			reader_bool_t r;
			return r.read(value, data);
		}
		if constexpr(std::is_integral_v<T>) {
			reader_int_t r;
			return r.read(value, data);
		}
		if constexpr(std::is_floating_point_v<T>) {
			reader_int_t r;
			return r.read(value, data);
		}
		if constexpr(std::is_same_v<T, std::string>) {
			reader_str_t r;
			return r.read(value, data);
		}
		if constexpr(std::is_same_v<T, std::string_view>) {
			reader_str_t r;
			return r.read(value, data);
		}
		return false;
	}
};

}
