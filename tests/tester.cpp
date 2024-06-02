#include <gtest/gtest.h>
#include <cxxnever_json/parser.hpp>


using cxxnever::json::value_t;

TEST(test, constr)
{
	if (value_t v = value_t())
		EXPECT_TRUE(false);
	else
		EXPECT_TRUE(true);

	if (value_t v {""})
		EXPECT_TRUE(false);
	else
		EXPECT_TRUE(true);

	if (value_t v {" 	"})
		EXPECT_TRUE(false);
	else
		EXPECT_TRUE(true);

	if (value_t v {"13"})
		EXPECT_EQ(13, int(*v));
	else
		EXPECT_TRUE(false);
}

TEST(test, convert)
{
	auto v = value_t {"128"};
	EXPECT_EQ(0, int8_t(*v));
	EXPECT_EQ(128, uint8_t(*v));
	EXPECT_EQ(128, int16_t(*v));
	EXPECT_EQ(128, uint16_t(*v));

	v = value_t {"256"};
	EXPECT_EQ(0, int8_t(*v));
	EXPECT_EQ(0, uint8_t(*v));
	EXPECT_EQ(256, int16_t(*v));
	EXPECT_EQ(256, uint16_t(*v));

	v = value_t {"65537"};
	EXPECT_EQ(0, int8_t(*v));
	EXPECT_EQ(0, uint8_t(*v));
	EXPECT_EQ(0, int16_t(*v));
	EXPECT_EQ(0, uint16_t(*v));
	EXPECT_EQ(65537, int32_t(*v));
	EXPECT_EQ(65537, uint32_t(*v));

	v = value_t {"-1"};
	EXPECT_EQ(-1, int8_t(*v));
	EXPECT_EQ(0, uint8_t(*v));
	EXPECT_EQ(-1, int16_t(*v));
	EXPECT_EQ(0, uint16_t(*v));
	EXPECT_EQ(-1, int32_t(*v));
	EXPECT_EQ(0, uint32_t(*v));
	EXPECT_EQ(-1, int64_t(*v));
	EXPECT_EQ(0, uint64_t(*v));

	v = value_t {"true"};
	EXPECT_EQ(0, int(*v));
	EXPECT_EQ(true, bool(*v));

	v = value_t {"false"};
	EXPECT_EQ(0, int(*v));
	EXPECT_EQ(false, bool(*v));

	v = value_t {"null"};
	EXPECT_EQ(0, int(*v));
	EXPECT_EQ(false, bool(*v));
	EXPECT_EQ("", std::string_view(*v));
	EXPECT_EQ("", std::string(*v));
}

TEST(test, convert_float)
{
	auto v = value_t {"13.5"};
	EXPECT_EQ(13.5, float(*v));
	EXPECT_EQ(13.5, double(*v));
	EXPECT_EQ(0, int(*v));

	v = value_t {"-17.2e+2"};
	EXPECT_EQ(-1720.0, float(*v));
	EXPECT_EQ(-1720.0, double(*v));
	EXPECT_EQ(0, int(*v));

	v = value_t {"19.4E-2"};
	EXPECT_EQ(0.194, double(*v));
	EXPECT_EQ(0, int(*v));

	v = value_t {"19e+1"};
	EXPECT_EQ(190, int(*v));

	v = value_t {"190e-1"};
	EXPECT_EQ(19, int(*v));
}

TEST(test, convert_string)
{
	auto v = value_t {"\"abc\""};
	EXPECT_EQ("abc", std::string_view(*v));
	EXPECT_EQ("abc", std::string(*v));

	v = value_t {"\"ab\\tc\\nd\""};
	EXPECT_EQ("ab\\tc\\nd", std::string_view(*v));
	EXPECT_EQ("ab\tc\nd", std::string(*v));

	v = value_t {R"("abc\"\\\/\b\f\n\r\tdef")"};
	EXPECT_EQ("abc\"\\/\b\f\n\r\tdef", std::string(*v));

	v = value_t {"\"abc\\u0020def\""};
	EXPECT_EQ("abc def", std::string(*v));

	v = value_t {"\"abc\u00e2\""};
	EXPECT_EQ("abcâ", std::string(*v));

	v = value_t {"\"\\u529f\\u592b\""};
	EXPECT_EQ("功夫", std::string(*v));
}

TEST(test, test_object)
{
	value_t value {R"( {"a": 11, "b": 13, "c": 17, "d": 19} )"};

	EXPECT_EQ(11, int(*value["a"]));
	EXPECT_EQ(13, int(*value["b"]));
	EXPECT_EQ(17, int(*value["c"]));
	EXPECT_EQ(19, int(*value["d"]));
	EXPECT_EQ(0, int(*value["e"]));

	int result = 0;
	for (auto it: value)
		result = result * 100 + int(*it);

	EXPECT_EQ(11131719, result);

	EXPECT_EQ(true, bool(value_t{"{}"}));
}

TEST(test, test_list)
{
	value_t value {R"( {"a": [], "b": [1], "c": [2, 3], "d": [4, 5, 7]} )"};

	int result = 0;
	for (auto it: value["a"])
		result = result * 10 + int(*it);
	for (auto it: value["b"])
		result = result * 10 + int(*it);
	for (auto it: value["c"])
		result = result * 10 + int(*it);
	for (auto it: value["d"])
		result = result * 10 + int(*it);

	EXPECT_EQ(123457, result);
}

TEST(test, test_validate)
{
	EXPECT_EQ(true, value_t("null").validate());
	EXPECT_EQ(false, value_t("nulla").validate());
	EXPECT_EQ(true, value_t("true").validate());
	EXPECT_EQ(true, value_t("false").validate());

	EXPECT_EQ(true, value_t("1").validate());
	EXPECT_EQ(true, value_t("0").validate());
	EXPECT_EQ(true, value_t("-1").validate());

	EXPECT_EQ(true, value_t("1.01").validate());
	EXPECT_EQ(true, value_t("1.01e+1").validate());
	EXPECT_EQ(true, value_t("1.01E-1").validate());

	EXPECT_EQ(true, value_t("\"abc\"").validate());
	EXPECT_EQ(true, value_t("\"abc\"").validate());
	EXPECT_EQ(true, value_t(R"("abc\"\\\/\b\f\n\r\tdef")").validate());
	EXPECT_EQ(true, value_t("\"abc\\uffff\"").validate());

	value_t x(R"({"a": 1, "b": 1.1, "c": true, "d": "A", "e": null})");
	EXPECT_EQ(true, x.validate());
	EXPECT_EQ(true, value_t("{}").validate());

	value_t y(R"([1, 1.1, true, false, null, "hello"])");
	EXPECT_EQ(true, y.validate());
	EXPECT_EQ(true, value_t("[]").validate());

	EXPECT_EQ(false, value_t("{\"a: 1}").validate());
	EXPECT_EQ(false, value_t("{a: 1}").validate());
	EXPECT_EQ(false, value_t("{\"a\" 1}").validate());
	EXPECT_EQ(false, value_t("{\"a\"}").validate());
	EXPECT_EQ(false, value_t("[\"a\",]").validate());
	EXPECT_EQ(false, value_t("[a]").validate());
}
