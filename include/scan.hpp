#pragma once

#include <tuple>

#include "parse.hpp"
#include "format_string.hpp"
#include "types.hpp"

namespace stdx {

// Главная функция
template <details::format_string fmt, details::fixed_string source, typename... Ts>
consteval details::scan_result<Ts...> scan() { // передайте пакет параметров в scan_result
// измените реализацию
    return details::scan_result<Ts...>{42};
}

} // namespace stdx