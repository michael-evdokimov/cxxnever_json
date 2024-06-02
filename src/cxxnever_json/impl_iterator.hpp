#pragma once
#include <string>
#include "impl_read_obj.hpp"


namespace cxxnever::json::impl
{

template<typename Holder>
struct iterator_t
{
	typedef iterator_t<Holder> iterator;

	Holder holder = {};
	std::string_view name;
	size_t index = 0;
	bool is_dict = false;

	iterator_t(std::string_view data)
	{
		impl::reader_obj_t r;
		r.unspace(data);
		if (data.size()) {
			if (data[0] == '{') {
				data.remove_prefix(1);
				r.unspace(data);
				if (!r.read_name(name, data))
					data = {};
				if (!(data.size() && data[0] == ':'))
					data = {};
				if (data.size())
					data.remove_prefix(1);
				is_dict = true;
			} else if (data[0] == '[') {
				data.remove_prefix(1);
			} else {
				data = {};
			}
			r.unspace(data);
		}
		holder.data = data;
	}

	void inc()
	{
		impl::reader_obj_t r;
		std::string_view& data = holder.data;

		if (!r.skip_any(data))
			return (void) (data = {});

		if (!(data.size() && data[0] == ','))
			return (void) (data = {});

		data.remove_prefix(1);
		r.unspace(data);

		if (is_dict) {
			if (!r.read_name(name, data))
				return (void) (data = {});
			if (!(data.size() && data[0] == ':'))
				return (void) (data = {});
			data.remove_prefix(1);
			r.unspace(data);
		}

		index++;
	}

	iterator& operator ++ ()
	{
		inc();
		return *this;
	}

	iterator operator ++ (int)
	{
		iterator r = *this;
		inc();
		return r;
	}

	Holder& operator * ()
	{
		return holder;
	}

	Holder* operator -> ()
	{
		return &holder;
	}

	friend bool operator == (const iterator& a, const iterator& b)
	{
		return a.holder.data.size() == b.holder.data.size();
	}

	friend bool operator != (const iterator& a, const iterator& b)
	{
		return a.holder.data.size() != b.holder.data.size();
	}
};

}
