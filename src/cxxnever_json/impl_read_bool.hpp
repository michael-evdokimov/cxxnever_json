#pragma once
#include <string>


namespace cxxnever::json::impl
{

struct reader_bool_t
{
	bool read(bool& value, std::string_view input)
	{
		if (input.starts_with("true"))
			if (input.size()==4 || strchr(",}]\x20\t\n", input[4]))
				return (value = true), true;

		if (input.starts_with("false"))
			if (input.size()==5 || strchr(",}]\x20\t\n", input[5]))
				return (value = false), true;

		return false;
	}
};

}
