#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <cstddef>
#include <exception>
#include <expected>
#include <tuple>
#include <utility>

namespace stdx {

// замените болванку функции scan на рабочую версию
template <typename... Ts>
constexpr std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {
    using paramVector_t = std::vector<std::string_view>;
    using parsedData_t = std::pair<paramVector_t, paramVector_t>;

    // 1. Парсим исходную строку
    auto _result = details::parse_sources<Ts...>(input, format);
    if(!_result)
        return std::unexpected(_result.error());
    
   parsedData_t parsedData = *_result;
    

    if (parsedData.first.size() != parsedData.second.size())
        return std::unexpected(details::scan_error("Parsing error. Numbers of format specifiers and values in input string doesn't equal."));

    if (parsedData.first.size() != sizeof...(Ts))
        return std::unexpected(details::scan_error("Parsing error. Numbers of format specifiers and values doesn't equal to number of template parameters."));

    // 2. Создадим вспомогательную лямбду для вызова 
    auto parseValuesHlp = [&]<size_t... I>(std::index_sequence<I...>) -> std::expected<details::scan_result<Ts...>, details::scan_error> {
        bool success = true;
        details::scan_error first_error{};

        auto results = std::make_tuple(
            details::parse_value_with_format<Ts>( parsedData.second[I], parsedData.first[I])...
        );

        (([&](auto &&res){
            if(!res && success){
                success = false;
                first_error = res.error();
            }
        }(std::get<I>(results))), ...);

        if (!success)
            return std::unexpected(first_error);

        details::scan_result<Ts...> final_results;
        final_results.values() = std::make_tuple(std::move(*std::get<I>(results))...);
        return final_results;
    } ;

    auto parsedValues = parseValuesHlp(std::make_index_sequence<sizeof...(Ts)>());
    if (parsedValues)
        return  parsedValues;
    else
        return std::unexpected(parsedValues.error());
}

} // namespace stdx
