#pragma once
#include <tuple>
#include <string>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
    // здесь ваш код
    std::tuple<Ts...> &values(){
        return scanValue;
    }
private:
    std::tuple<Ts...> scanValue;
};

} // namespace stdx::details
