#pragma once

#include <cstdlib>
#include <exception>
#include <expected>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <concepts>
#include <iostream>

#include "types.hpp"

namespace stdx::details {

template<typename T>
requires std::integral<T> || std::floating_point<T>
constexpr std::expected<T, scan_error>  parse_value(std::string_view &input)
{
    T result{};
    auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), result);
    if (ec == std::errc())
        return result;
    else  if (ec == std::errc::invalid_argument)
        return std::unexpected(scan_error{std::string("Can't convert \"" + std::string(input) + "\". Invalid argument.")});
    else if (ec == std::errc::result_out_of_range)
        return std::unexpected(scan_error{std::string("Can't convert \"" + std::string(input) + "\". Result out of range.")});

    return std::unexpected(scan_error{std::string("Can't convert \"" + std::string(input) + "\".")});
}

template<typename T>
requires std::convertible_to<T, std::string_view> || std::convertible_to<T, std::string>
constexpr std::expected<T, scan_error>  parse_value(std::string_view &input)
{
    return static_cast<T>(input);
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
constexpr std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) 
{  
    enum type_format{FMT_INTEGRAL, FMT_NATURAL, FMT_FLOATING, FMT_STRING, FMT_ANY, FMT_UNDEFINED};
    
    type_format fmt_type = FMT_UNDEFINED;

    if (!fmt.compare("d"))
        fmt_type = FMT_INTEGRAL;
    else if (!fmt.compare("s"))
        fmt_type = FMT_STRING;
    else if (!fmt.compare("u"))
        fmt_type = FMT_NATURAL;
    else if (!fmt.compare("f"))
        fmt_type = FMT_FLOATING;
    else if (!fmt.compare(""))
        fmt_type = FMT_ANY;

    // Добавить проверку сответствия формата и типа Т 
    if ((fmt_type == FMT_INTEGRAL && !std::is_integral<T>()) ||
        (fmt_type == FMT_STRING && !(std::is_same<T, std::string>() || std::is_same<T, std::string_view>())) ||
        (fmt_type == FMT_NATURAL && !(std::is_integral<T>() && std::is_unsigned<T>())) ||
        (fmt_type == FMT_FLOATING && !std::is_floating_point<T>()))
            return std::unexpected(scan_error{std::string("Format specifier \"{" + std::string(fmt) + "}\" does not match template type\n\r")});
    else if (fmt_type == FMT_UNDEFINED) 
        return std::unexpected(scan_error{std::string("Format specifier \"{" + std::string(fmt) + "}\" is undefined\n\r")});

    auto res = parse_value<T>(input);
    if (res)
        return res;
    else 
        return std::unexpected(res.error());
   
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
constexpr std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

} // namespace stdx::details