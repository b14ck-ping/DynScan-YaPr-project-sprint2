#include <gtest/gtest.h>
#include <print>
#include <string>

#include "scan.hpp"

TEST(OneScanTest, StringTestSuccess) {
    auto result = stdx::scan<std::string>("number", "{s}");
    ASSERT_TRUE(result);
}

TEST(OneScanTest, FloatTestSuccess) {
    auto result = stdx::scan<float>("-1.45", "{f}");
    ASSERT_TRUE(result);
}

TEST(OneScanTest, SignIntTestSuccess) {
    auto result = stdx::scan<int8_t>("-55", "{d}");
    ASSERT_TRUE(result);
}

TEST(OneScanTest, UnsignIntTestSuccess) {
    auto result = stdx::scan<uint8_t>("55", "{u}");
    ASSERT_TRUE(result);
}

TEST(OneScanTest, UnsignIntAnyFmtTestSuccess) {
    auto result = stdx::scan<uint8_t>("55", "{}");
    ASSERT_TRUE(result);
}

TEST(OneScanTest, UnsignIntTestLongStringFormatSuccess) {
    auto result = stdx::scan<uint8_t>("This is number 55", "This is number {u}");
    ASSERT_TRUE(result);
}

TEST(MultScanTest, StringFloatTestSuccess) {
    auto result = stdx::scan<std::string, uint64_t>("It's number 55", "It's {} {d}");
    ASSERT_TRUE(result);
}

TEST(MultScanTest, FloatSignIntTestSuccess) {
    auto result = stdx::scan<float, int32_t>("-1.45 1", "{f} {}");
    ASSERT_TRUE(result);
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

