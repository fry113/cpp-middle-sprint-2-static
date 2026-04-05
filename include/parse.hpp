#pragma once
#include <charconv>
#include <concepts>
#include <cstdint>
#include <string_view>

#include "format_string.hpp"
#include "types.hpp"

namespace stdx::details {

// Шаблонная функция, возвращающая пару позиций в строке с исходными данными, соотвествующих I-ому плейсхолдеру
template <int I, format_string fmt, fixed_string source>
consteval auto get_current_source_for_parsing() {
    static_assert(I >= 0 && I < fmt.number_placeholders, "Invalid placeholder index");

    constexpr auto to_sv = [](const auto &fs) { return std::string_view(fs.data, fs.size() - 1); };

    constexpr auto fmt_sv = to_sv(fmt.str);
    constexpr auto src_sv = to_sv(source);
    constexpr auto &positions = fmt.placeholder_positions;

    // Получаем границы текущего плейсхолдера в формате
    constexpr auto pos_i = positions[I];
    constexpr size_t fmt_start = pos_i.first, fmt_end = pos_i.second;

    // Находим начало в исходной строке
    constexpr auto src_start = [&] {
        if constexpr (I == 0) {
            return fmt_start;
        } else {
            // Находим конец предыдущего плейсхолдера в исходной строке
            constexpr auto prev_bounds = get_current_source_for_parsing<I - 1, fmt, source>();
            const auto prev_end = prev_bounds.second;

            // Получаем разделитель между текущим и предыдущим плейсхолдерами
            constexpr auto prev_fmt_end = positions[I - 1].second;
            constexpr auto sep = fmt_sv.substr(prev_fmt_end + 1, fmt_start - (prev_fmt_end + 1));

            // Ищем разделитель после предыдущего значения
            auto pos = src_sv.find(sep, prev_end);
            return pos != std::string_view::npos ? pos + sep.size() : src_sv.size();
        }
    }();

    // Находим конец в исходной строке
    constexpr auto src_end = [&] {
        // Получаем разделитель после текущего плейсхолдера
        if constexpr (fmt_end == (fmt_sv.size() - 1)) {
            return src_sv.size();
        }
        constexpr auto sep =
            fmt_sv.substr(fmt_end + 1, (I < fmt.number_placeholders - 1) ? positions[I + 1].first - (fmt_end + 1)
                                                                         : fmt_sv.size() - (fmt_end + 1));
        // Ищем разделитель после текущего значения
        constexpr auto pos = src_sv.find(sep, src_start);
        return pos != std::string_view::npos ? pos : src_sv.size();
    }();
    return std::pair{src_start, src_end};
}

// SFINAE-реализация parse_value (unsigned)
template <fixed_string source, typename T>
consteval T parse_value()
    requires std::unsigned_integral<T>
{
    uint32_t ret{};
    std::from_chars(source.data, source.data + source.size() - 1, ret);
    return T(ret);
}

// SFINAE-реализация parse_value (signed)
template <fixed_string source, typename T>
consteval T parse_value()
    requires std::signed_integral<T>
{
    int32_t ret{};
    std::from_chars(source.data, source.data + source.size() - 1, ret);
    return T(ret);
}

// концепт проверки, что строка может быть корректно распарсена в float/double
template <fixed_string source>
concept valid_float_source = [] {
    constexpr size_t size = source.size() - 1;
    if (size == 0) {
        return false;
    }

    constexpr size_t first = (source.data[0] == '-' || source.data[0] == '+') ? 1 : 0;
    if (first >= size) {
        return false;
    }

    bool has_separators = false;
    bool has_numbers = false;

    for (size_t i = first; i < size; ++i) {
        const char ch = source.data[i];
        if (ch >= '0' && ch <= '9') {
            has_numbers = true;
            continue;
        }
        if (ch == '.' || ch == ',') {
            if (has_separators) {
                return false;
            }
            has_separators = true;
            continue;
        }
        return false;
    }

    return has_numbers;
}();

// SFINAE-реализация parse_value (float/double)
template <fixed_string source, typename T>
consteval T parse_value()
    requires std::floating_point<T>
{
    // перенес концепт проверки на возможность спарсить source, чтобы вывести сообщение об ошибке через static_assert
    static_assert(
        valid_float_source<source>,
        "invalid float/double value: at least 1 digit, no more than 1 separator (,/.), optional -/+ at the beginning");

    constexpr size_t size = source.size() - 1;
    constexpr size_t first = (source.data[0] == '-' || source.data[0] == '+') ? 1 : 0;

    long double ret = 0;
    long double frac_div = 1;
    bool fractional = false;

    for (size_t i = first; i < size; ++i) {
        const char ch = source.data[i];
        if (ch >= '0' && ch <= '9') {
            const T digit = static_cast<T>(ch - '0');
            if (!fractional) {
                ret = ret * static_cast<T>(10) + digit;
            } else {
                frac_div *= static_cast<T>(10);
                ret += digit / frac_div;
            }
            continue;
        }
        fractional = true;
    }

    return source.data[0] == '-' ? T(-ret) : T(ret);
}

// SFINAE-реализация parse_value (string_view)
template <fixed_string source, typename T>
consteval T parse_value()
    requires std::same_as<std::remove_cv_t<T>, std::string_view>
{
    return T(source.data, source.size() - 1);
}

// SFINAE-реализация parse_value (fail)
template <fixed_string source, typename T>
consteval T parse_value() {
    T ret{};
    static_assert(false, "fail to parse unsupported type");
    return ret;
}

// концепт проверки соотв.спецификатора I-ого плейсхолдера типу T (если пустой, то любой тип T подходит)
template <size_t I, format_string fmt, typename T>
concept placeholder_same_as = [] {
    constexpr std::pair<size_t, size_t> fmt_pos = fmt.placeholder_positions[I];

    // нет спецификатора => подходит любой тип
    if constexpr (fmt_pos.second - fmt_pos.first <= 2) {
        return true;
    }

    // проверка спецификатора
    constexpr char ch = fmt.str.data[fmt_pos.first + 2];
    if constexpr (ch == 'd') {
        return std::signed_integral<T>;
    }
    if constexpr (ch == 'u') {
        return std::unsigned_integral<T>;
    }
    if constexpr (ch == 'f') {
        return std::floating_point<T>;
    }
    if constexpr (ch == 's') {
        return std::same_as<std::remove_cv_t<T>, std::string_view>;
    }
    return false;
}();

// Шаблонная функция, выполняющая преобразования исходных данных в конкретный тип на основе I-го плейсхолдера
template <size_t I, format_string fmt, fixed_string source, typename T>
    requires placeholder_same_as<I, fmt, T>
consteval T parse_input() {
    // позиции I-ого плейсхолдера
    constexpr std::pair<size_t, size_t> fmt_pos = fmt.placeholder_positions[I];
    // позиции в исходной строке, подходящие I-ому плейсхолдеру
    constexpr std::pair<size_t, size_t> source_pos = get_current_source_for_parsing<I, fmt, source>();

    // формирование фикс.строки
    constexpr size_t substr_size = source_pos.second - source_pos.first + 1;
    constexpr fixed_string<substr_size> src_substr{source.data + source_pos.first, source.data + source_pos.second};

    // парсинг полученной подстроки
    constexpr T ret = parse_value<src_substr, T>();

    return ret;
}

// для вывода сообщения о несоответствии типа T спецификатору I-ого плейсхолдера (вывод через концепт)
template <size_t I, format_string fmt, fixed_string source, typename T>
consteval T parse_input() {
    static_assert(placeholder_same_as<I, fmt, T>, "type T != format specifier of placeholder ");
    return T{};
}

}  // namespace stdx::details
