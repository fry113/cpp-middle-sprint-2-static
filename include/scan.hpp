#pragma once
#include <cstddef>
#include <utility>

#include "format_string.hpp"
#include "parse.hpp"
#include "types.hpp"

namespace stdx {

// Главная функция
template <details::format_string fmt, details::fixed_string source, typename... Ts>
consteval details::scan_result<Ts...> scan() {
    static_assert(fmt.number_placeholders, "format string is invalid");
    constexpr size_t ph_count = fmt.number_placeholders;
    static_assert(sizeof...(Ts) == ph_count, "count of placeholders doesn't match count of types");

    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return details::scan_result<Ts...>{details::parse_input<Is, fmt, source, Ts>()...};
    }(std::index_sequence_for<Ts...>{});
}

}  // namespace stdx