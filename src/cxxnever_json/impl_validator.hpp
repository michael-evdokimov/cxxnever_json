#pragma once
#include <string>


namespace cxxnever::json::impl
{

struct validator_t
{
	void unspace(std::string_view data, size_t& index)
	{
		index = data.find_first_not_of("\x20\t\r\n", index);
		if (index == -1)
			index = data.size();
	}

	bool check_word(std::string_view data, size_t& i, const char* word)
	{
		size_t len = strlen(word);
		if (data.size() - i < len)
			return false;
		if (memcmp(&data[i], word, len) != 0)
			return false;
		if (data.size() - i == len)
			return i += len, true;

		char c = data[i + len];
		if (strchr("\x20\t\r\n,}]", c) == nullptr)
			return false;

		i += len;
		return true;
	}

	bool check_null(std::string_view data, size_t& index)
	{
		return check_word(data, index, "null");
	}

	bool check_bool(std::string_view da, size_t& i)
	{
		return check_word(da, i, "true") || check_word(da, i, "false");
	}

	bool check_four(std::string_view data, size_t& index)
	{
		for (int8_t i = 0; i != 4; ++i) {
			char c = data[index + i];
			if ('0' <= c && c <= '9')
				continue;
			if ('a' <= c && c <= 'f')
				continue;
			if ('A' <= c && c <= 'F')
				continue;
			return false;
		}
		index += 4;
		return true;
	}

	bool check_str(std::string_view data, size_t& index)
	{
		if (!(index < data.size() && data[index] == '"'))
			return false;
		++index;

		while (index < data.size() && data[index] != '"') {
			if (data[index++] != '\\')
				continue;
			if (!(index < data.size()))
				return false;
			switch (data[index++]) {
				case '"': continue;
				case '\\': continue;
				case '/': continue;
				case 'b': continue;
				case 'f': continue;
				case 'n': continue;
				case 'r': continue;
				case 't': continue;
				case 'u': break;
				default:  return false;
			}
			if (!check_four(data, index))
				return false;
		}
		if (!(index < data.size()))
			return false;
		index++;
		return true;
	}

	bool check_int(std::string_view data, size_t& i)
	{
		size_t size = data.size();

		if (i < size && data[i] == '-')
			i++;

		if (!(i < size && '0' <= data[i] && data[i] <= '9'))
			return false;

		while (i < size && '0' <= data[i] && data[i] <= '9')
			i++;

		if (i < size && data[i] == '.') {
			i++;
			while (i < size && '0' <= data[i] && data[i] <= '9')
				i++;
		}
		if (i < size && (data[i] == 'e' || data[i] == 'E')) {
			i++;
			if (!(i < size && (data[i] == '-' || data[i] == '+')))
				return false;
			i++;
			if (!(i < size && '0' <= data[i] && data[i] <= '9'))
				return false;
			while (i < size && '0' <= data[i] && data[i] <= '9')
				i++;
		}

		if (i < size && strchr("\x20\t\r\n,}]", data[i]))
			return true;
		if (i == size)
			return true;
		return false;
	}

	bool check_obj(std::string_view data, size_t& i)
	{
		if (!(i < data.size() && data[i] == '{'))
			return false;
		i++;
		unspace(data, i);
		if (i < data.size() && data[i] == '}')
			return ++i, true;
		while (true) {
			unspace(data, i);
			if (!check_str(data, i))
				return false;
			unspace(data, i);
			if (!(i < data.size() && data[i] == ':'))
				return false;
			i++;
			unspace(data, i);
			if (!check_any(data, i))
				return false;
			unspace(data, i);
			if (i < data.size() && data[i] == '}')
				return ++i, true;
			if (!(i < data.size() && data[i] == ','))
				return false;
			i++;
		}
	}

	bool check_list(std::string_view data, size_t& i)
	{
		if (!(i < data.size() && data[i] == '['))
			return false;
		i++;
		unspace(data, i);
		if (i < data.size() && data[i] == ']')
			return ++i, true;
		while (true) {
			unspace(data, i);
			if (!check_any(data, i))
				return false;
			unspace(data, i);
			if (i < data.size() && data[i] == ']')
				return ++i, true;
			if (!(i < data.size() && data[i] == ','))
				return false;
			i++;
		}
	}

	bool check_any(std::string_view data, size_t& index)
	{
		if (index >= data.size())
			return false;
		unspace(data, index);
		bool flag = false;
		switch (data[index]) {
			case '"': flag = check_str(data, index); break;
			case '{': flag = check_obj(data, index); break;
			case '[': flag = check_list(data, index); break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-': flag = check_int(data, index); break;
			case 't':
			case 'f': flag = check_bool(data, index); break;
			case 'n': flag = check_null(data, index); break;
			default:
				return false;
		}
		if (!flag)
			return flag;
		unspace(data, index);
		return true;
	}
};

}
