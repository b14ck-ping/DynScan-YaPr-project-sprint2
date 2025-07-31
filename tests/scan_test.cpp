#include <gtest/gtest.h>
#include <print>
#include <string>

#include "scan.hpp"

TEST(OneScanTest, SignIntTestSuccess) {
    auto result = stdx::scan<int8_t, int16_t, int32_t, int64_t, float, double, std::string, std::string_view>
    ("-128 -32768 -2147483648 -9223372036854775808 3.14 2.7 string1 string_view1", "{d} {d} {d} {d} {f} {f} {s} {s}");    
    ASSERT_TRUE((result && 
                (*result).values() == std::tuple{-128, -32768, -2147483648L, -9223372036854775807LL-1, 3.14f, 2.7, std::string("string1"), std::string_view("string_view1")}));
}

TEST(OneScanTest, UnsignIntTestSuccess) {
    auto result = stdx::scan<uint8_t, uint16_t, uint32_t, uint64_t>("255 65535 4294967295 18446744073709551615", "{u} {u} {u} {u}");
    ASSERT_TRUE((result && (*result).values() == std::tuple{255, 65535, 4294967295L, 18446744073709551615LL}));
}

TEST(OneScanTest, UnsignIntAnyFmtTestSuccess) {
    auto result = stdx::scan<int8_t, int16_t, int32_t, int64_t, float, double, std::string, std::string_view>
    ("-128 -32768 -2147483648 -9223372036854775808 3.14 2.7 string1 string_view1", "{} {} {} {} {} {} {} {}");    
    ASSERT_TRUE((result && 
                (*result).values() == std::tuple{-128, -32768, -2147483648L, -9223372036854775807LL-1, 3.14f, 2.7, std::string("string1"), std::string_view("string_view1")}));
}

TEST(OneScanTest, UnsignIntTestLongStringFormatSuccess) {
    auto result = stdx::scan<uint8_t>("This is number 55", "This is number {u}");
    ASSERT_TRUE((result && (*result).values() == std::tuple{55}));
}

TEST(MultScanTest, StringFloatTestSuccess) {
    auto result = stdx::scan<std::string, uint64_t>("It's number 55", "It's {} {d}");
    ASSERT_TRUE((result && (*result).values() == std::tuple{std::string("number"), 55}));
}

TEST(MultScanTest, FloatSignIntTestSuccess) {
    auto result = stdx::scan<float, int32_t>("-1.45 -2147483648", "{f} {}");
    ASSERT_TRUE((result && (*result).values() == std::tuple{-1.45f, -2147483648}));
}

TEST(ScanTestFails, SignIntTestWrongFmt) {
    auto result = stdx::scan<int8_t>("-55", "{u}");
    ASSERT_FALSE(result);
    ASSERT_EQ(result.error().message, std::string("Format specifier \"{u}\" does not match template type\n\r"));
}

TEST(ScanTestFails, UndefinedFmt) {
    auto result = stdx::scan<uint8_t>("55", "{r}");
    ASSERT_FALSE(result); 
    ASSERT_EQ(result.error().message, std::string("Format specifier \"{r}\" is undefined\n\r"));
}

TEST(ScanTestFails, TemplateTypesCntDoesntEqualFmtCnt) {
    auto result = stdx::scan<uint8_t, float>("55", "{}");
    ASSERT_FALSE(result); 
    ASSERT_EQ(result.error().message, std::string("Parsing error. Numbers of format specifiers and values doesn't equal to number of template parameters."));
}

TEST(ScanTestFails, IntValueOverload) {
    auto result = stdx::scan<uint8_t>("300", "{}");
    ASSERT_FALSE(result); 
    ASSERT_EQ(result.error().message, std::string("Can't convert \"300\". Result out of range."));
}

TEST(ScanTestFails, UnignIntTestWithNegativeVal) {
    auto result = stdx::scan<uint8_t>("-55", "{u}");
    ASSERT_FALSE(result);
    ASSERT_EQ(result.error().message, std::string("Can't convert \"-55\". Invalid argument."));
}
