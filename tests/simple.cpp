#include <gtest/gtest.h>
#include <cxxnever_json/parser.hpp>


TEST(test_simple, test_direct_int_from_object)
{
	const char* input = R"(   { "someint" : 13 }   )";

	cxxnever::json::value_t value {input};

	int someint = *value["someint"];

	EXPECT_EQ(13, someint);
}

TEST(test_simple, test_optional_int_from_object)
{
	const char* input = R"(   {"someint": 17}   )";

	cxxnever::json::value_t value {input};

	std::optional<int> someint = value["someint"].read<int>();

	EXPECT_EQ(true, bool(someint));
	EXPECT_EQ(17, *someint);
}

TEST(test_simple, test_optinal_from_object_failed)
{
	const char* input = R"(   {"someint": 19}   )";

	cxxnever::json::value_t value {input};

	std::optional<int> other = value["other_int"].read<int>();

	EXPECT_EQ(false, bool(other));
}

TEST(test_simple, test_optinal_with_default)
{
	const char* input = R"(   {"someint": 23}   )";

	cxxnever::json::value_t value {input};

	int other = value["other"].read<int>(29);

	EXPECT_EQ(29, other);
}

TEST(test_simple, test_optional_with_default_but_string)
{
	const char* input = R"(   {"someint": "oops"}    )";

	cxxnever::json::value_t value {input};

	int someint = value["someint"].read<int>(23);

	EXPECT_EQ(23, someint);
}

TEST(test_simple, test_string_view)
{
	const char* input = R"(   {"sometext": "abcdef"}   )";

	cxxnever::json::value_t value {input};

	std::string_view sometext {*value["sometext"]};

	EXPECT_EQ("abcdef", sometext);
}

TEST(test_simple, test_optional_string_view)
{
	const char* input = R"(   {"sometext": "abcdef"}   )";

	cxxnever::json::value_t value {input};

	std::optional<std::string_view> sometext =
	                             value["sometext"].read<std::string_view>();

	EXPECT_EQ(true, bool(sometext));
	EXPECT_EQ("abcdef", *sometext);

	if (auto p = value["sometext"].read<std::string_view>())
		EXPECT_EQ("abcdef", *p);
}

TEST(test_simple, test_bool)
{
	const char* input = R"(   {"a": true, "b": false}   )";

	cxxnever::json::value_t value {input};

	EXPECT_EQ(true, value["a"].read(false));
	EXPECT_EQ(false, value["b"].read(true));
}

TEST(test_simple, test_array)
{
	const char* input = R"(   {"list": [1, 2, 3]}   )";

	cxxnever::json::value_t value {input};

	int result = 0;

	for (auto x: value["list"])
		result = result * 10 + x.read<int>(0);

	EXPECT_EQ(123, result);

	for (cxxnever::json::value_t x: value["list"])
		result = result * 10 + int(*x);

	EXPECT_EQ(123123, result);
}

TEST(test_simple, test_deep)
{
	const char* input = R"(   {"a": {"b": [{"c": 1}, {"c": 2}]}}   )";

	cxxnever::json::value_t value {input};

	int result = 0;

	if (auto a = value["a"])
		for (auto b: a["b"])
			if (auto c = b["c"])
				result = result * 10 + int(*c);

	for (auto x: value["a"]["b"])
		result = result * 10 + int(*x["c"]);

	EXPECT_EQ(1212, result);
}

TEST(test_simple, test_validate)
{
	const char* input1 = R"(   {"a": {"b": 13}}   )";
	const char* input2 = R"(   {"a": {"b"         )";

	cxxnever::json::value_t value1 {input1};
	cxxnever::json::value_t value2 {input2};

	EXPECT_EQ(true, value1.validate());
	EXPECT_EQ(false, value2.validate());
}

TEST(test_simple, test_object_iterator)
{
	const char* input = R"(   {"a": 1}   )";

	cxxnever::json::value_t value {input};

	size_t count = 0;
	for (auto it = value.begin(); it != value.end(); ++it) {
		count++;
		EXPECT_EQ("a", it.name);
		EXPECT_EQ(1, int(**it));
	}

	EXPECT_EQ(1, count);
}
