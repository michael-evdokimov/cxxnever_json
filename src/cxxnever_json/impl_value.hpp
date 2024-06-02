#pragma once
#include <string>
#include <optional>
#include "impl_read.hpp"
#include "impl_iterator.hpp"
#include "impl_validator.hpp"


namespace cxxnever::json::impl
{

struct value_t
{
	std::string_view data;

	typedef iterator_t<value_t> iterator;

	value_t()
	{
	}

	value_t(std::string_view input)
	{
		impl::reader_obj_t p;
		p.unspace(input);
		data = input;
	}

	explicit operator bool () const
	{
		return data.size();
	}

	struct extracter
	{
		std::string_view data;

		operator bool     () const { return read<bool>  (); }

		operator int8_t   () const { return read<int8_t>  (); }
		operator uint8_t  () const { return read<uint8_t> (); }
		operator int16_t  () const { return read<int16_t> (); }
		operator uint16_t () const { return read<uint16_t>(); }
		operator int32_t  () const { return read<int32_t> (); }
		operator uint32_t () const { return read<uint32_t>(); }
		operator int64_t  () const { return read<int64_t> (); }
		operator uint64_t () const { return read<uint64_t>(); }

		operator float    () const { return read<float>   (); }
		operator double   () const { return read<double>  (); }

		explicit operator std::string () const
		{
			return read<std::string>();
		}

		explicit operator std::string_view () const
		{
			return read<std::string_view>();
		}

		template<typename T>
		T read() const
		{
			T value = {};
			impl::reader_t r;
			if (!r.read(value, data))
				return {};
			return value;
		}
	};

	extracter operator * () const
	{
		return extracter {data};
	}

	template<typename T>
	std::optional<T> read() const
	{
		T value = {};
		impl::reader_t r;
		if (!r.read(value, data))
			return {};

		return value;
	}

	template<typename T>
	T read(T default_value) const
	{
		T value = {};
		impl::reader_t r;
		if (!r.read(value, data))
			return default_value;

		return value;
	}

	value_t operator [] (std::string_view name) const
	{
		for (iterator it {data}, end {""}; it != end; ++it)
			if (it.name == name)
				return *it;

		return value_t {{}};
	}

	iterator begin() const
	{
		return iterator {data};
	}

	iterator end() const
	{
		return iterator {std::string_view{}};
	}

	bool validate() const
	{
		size_t i = 0;
		impl::validator_t r;
		return r.check_any(data, i);
	}
};

}
