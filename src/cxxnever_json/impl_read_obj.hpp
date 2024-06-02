#pragma once
#include <string>


namespace cxxnever::json::impl
{

struct reader_obj_t
{
	void unspace(std::string_view& s)
	{
		ssize_t i = s.find_first_not_of("\x20\t\r\n");
		if (i == -1)
			i = s.size();
		s.remove_prefix(i);
	}

	bool read_name(std::string_view& ret, std::string_view& data)
	{
		size_t i = 1;
		if (!(data.size() && data[0] == '"'))
			return false;
		for (; i != data.size(); ++i) {
			if (data[i] == '"')
				break;
			if (data[i] != '\\')
				continue;
			if (++i == data.size())
				return false;
		}
		if (i == data.size() || data[i] != '"')
			return false;
		ret = std::string_view {&data[1], i - 1};
		data.remove_prefix(i + 1);
		unspace(data);
		return true;
	}

	bool skip_str(std::string_view& data)
	{
		size_t i = 1;
		for (; i < data.size() && data[i] != '"'; ++i)
			if (data[i] == '\\')
				++i;

		if (i + 1 <= data.size()) {
			data.remove_prefix(i + 1);
			unspace(data);
			return true;
		}

		return false;
	}

	bool skip_obj(std::string_view& data)
	{
		char closing = (data[0] == '{') ? '}' : ']';
		size_t i = 1;
		for (; i < data.size() && data[i] != closing; ++i) {
			char c = data[i];
			if (c == '"' || c == '{' || c == '[') {
				data.remove_prefix(i);
				i = 0;
				if (!skip_any(data))
					return false;
			}
		}
		if (i < data.size() && data[i] == closing) {
			data.remove_prefix(i + 1);
			unspace(data);
			return true;
		}
		return false;
	}

	bool skip_any(std::string_view& data)
	{
		if (data.size() && data[0] == '"')
			return skip_str(data);
		if (data.size() && (data[0] == '{' || data[0] == '['))
			return skip_obj(data);

		for (size_t i = 0; i != data.size(); ++i) {
			char c = data[i];
			if (c == ',' || c == '}' || c == ']') {
				data.remove_prefix(i);
				unspace(data);
				return true;
			}
		}

		return false;
	}
};

}
