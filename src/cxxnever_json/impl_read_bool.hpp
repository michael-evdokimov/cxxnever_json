#pragma once
#include <string>


namespace cxxnever::json::impl
{

struct reader_bool_t
{
	bool read_const(std::string_view name, std::string_view input)
	{
		if (name.size() <= input.size()) {
			if (memcmp(&name[0], &input[0], name.size()) != 0)
				return false;
			if (name.size() == input.size())
				return true;
			if (strchr(",}]\x20\t\r\n", input[name.size()]))
				return true;
		}

		return false;
	}

	bool read(bool& value, std::string_view input)
	{
		if (read_const("true", input))
			return value = true, true;

		if (read_const("false", input))
			return value = false, true;

		return false;
	}
};

}
