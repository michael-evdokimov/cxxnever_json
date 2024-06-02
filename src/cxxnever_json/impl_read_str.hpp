#pragma once
#include <string>


namespace cxxnever::json::impl
{

struct reader_str_t
{
	bool read(std::string_view& value, std::string_view input)
	{
		const char* p = &*input.begin();
		const char* end = &*input.end();

		if (!(p != end && *p == '"'))
			return false;

		const char* start = ++p;

		while (p != end && *p != '"') {
			while (p != end && *p != '"' && *p != '\\')
				p++;
			if (p != end && *p == '\\')
				if (++p != end)
					if (*p == '"')
						++p;
		}

		if (p == end)
			return false;

		value = std::string_view {start, size_t(p - start)};
		return true;
	}

	bool read(std::string& value, std::string_view input)
	{
		std::string_view view;
		if (!read(view, input))
			return false;

		value = {};
		value.reserve(view.size());

		for (size_t i = 0; i != view.size(); ++i) {
			if (view[i] != '\\') {
				value += view[i];
				continue;
			}
			if (++i == view.size())
				return false;
			char c = view[i];
			switch (c) {
				case '"': value += '"'; continue;
				case '\\': value += '\\'; continue;
				case '/': value += '/'; continue;
				case 'b': value += '\b'; continue;
				case 'f': value += '\f'; continue;
				case 'n': value += '\n'; continue;
				case 'r': value += '\r'; continue;
				case 't': value += '\t'; continue;
				case 'u': break;
				default: return false;
			}
			if (i + 4 >= view.size())
				return false;
			char buf[4] = {};
			if (!convert_unicode(buf, &view[i + 1]))
				return false;
			value += buf;
			i += 4;
		}

		return true;
	}

	bool convert_unicode(char (&out)[4], const char* in)
	{
		char buf[5] = {in[0], in[1], in[2], in[3], 0};
		char* end = nullptr;
		int num = strtoull(buf, &end, 16);
		if (*end)
			return false;
		if (num <= 0x7f) {
			out[0] = num;
			return true;
		}
		if (num <= 0x07ff) {
			out[0] = ((num >> 6)&0b00011111) | 0b110'00000;
			out[1] = ((num >> 0)&0b00111111) | 0b10'000000;
			return true;
		}
		if (num <= 0xffff) {
			out[0] = ((num >> 12)&0b00001111) | 0b1110'0000;
			out[1] = ((num >> 6 )&0b00111111) | 0b10'000000;
			out[2] = ((num >> 0 )&0b00111111) | 0b10'000000;
			return true;
		}
		return false;
	}
};

}
