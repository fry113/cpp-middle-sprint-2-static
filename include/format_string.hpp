#pragma once
#include <array>
#include <cstddef>
#include <expected>

#include "types.hpp"

namespace stdx::details {

// Шаблонный класс для хранения форматирующей строчки и ее особенностей,
// параметризованный NTTP типа fixed_string
template <fixed_string fx_str>
class format_string {
    // private: ?
public:
    // статическое поле класса для доступа к NTTP снаружи
    static constexpr fixed_string str{fx_str};

    // статический метод для получения количества плейсхолдеров и проверки корректности формирующей строки
    static consteval std::expected<size_t, parse_error> get_number_placeholders();

    // static_assert успешного вызова get_number_placeholders()
    static_assert(get_number_placeholders().has_value(),
                  "format string is invalid: fail to get number of placeholders");

    // статическое поле количества плейсхолдеров
    static constexpr auto number_placeholders = get_number_placeholders().value();

    // статический метод получения пар позиций плейсхолдеров
    static consteval std::array<std::pair<size_t, size_t>, number_placeholders> get_placeholder_positions();

    // статическое поле для хранения позиций плейсхолдеров
    static constexpr auto placeholder_positions = get_placeholder_positions();
    // static_assert для проверки успешной инициализации placeholder_positions
    static_assert(placeholder_positions.size() != 0, "placeholder_positions initialization failed");
};

// Пользовательский литерал
template <fixed_string S>
consteval auto operator""_fs() {
    return format_string<S>{};
}

// Функция для получения количества плейсхолдеров и проверки корректности формирующей строки
template <fixed_string str>
consteval std::expected<size_t, parse_error> format_string<str>::get_number_placeholders() {
    constexpr size_t N = str.size();
    if (!N)
        return 0;
    size_t placeholder_count = 0;
    size_t pos = 0;
    const size_t size = N;

    while (pos < size) {
        // Пропускаем все символы до '{'
        if (str.data[pos] != '{') {
            ++pos;
            continue;
        }

        // Проверяем незакрытый плейсхолдер
        if (pos + 1 >= size) {
            return std::unexpected(parse_error{"Unclosed last placeholder"});
        }

        // Начало плейсхолдера
        ++placeholder_count;
        ++pos;

        // Проверка спецификатора формата
        if (str.data[pos] == '%') {
            ++pos;
            if (pos >= size) {
                return std::unexpected(parse_error{"Unclosed last placeholder"});
            }

            // Проверяем допустимые спецификаторы
            const char spec = str.data[pos];
            constexpr char valid_specs[] = {'d', 'u', 'f', 's'};
            bool valid = false;

            for (const char s : valid_specs) {
                if (spec == s) {
                    valid = true;
                    break;
                }
            }

            if (!valid) {
                return std::unexpected(parse_error{"Invalid specifier."});
            }
            ++pos;
        }

        // Проверяем закрывающую скобку
        if (pos >= size || str.data[pos] != '}') {
            return std::unexpected(parse_error{"\'}\' hasn't been found in appropriate place"});
        }
        ++pos;
    }

    return placeholder_count;
}

// Функция для получения позиций плейсхолдеров
template <fixed_string str>
consteval std::array<std::pair<size_t, size_t>, format_string<str>::number_placeholders>
format_string<str>::get_placeholder_positions() {
    constexpr size_t ph_count = number_placeholders;
    std::array<std::pair<size_t, size_t>, ph_count> ret{};
    size_t pos = 0;
    size_t ph_num = 0;
    const size_t str_size = str.size();

    while (pos < str_size && ph_num < ph_count) {
        // поиск '{'
        if (str.data[pos] != '{') {
            ++pos;
            continue;
        }

        // начало плейсхолдера
        size_t begin_ph = pos;
        ++pos;

        // пропуск содержимого плейсхолдера
        if (str.data[pos] == '%') {
            ++pos;  // пропуск '%'
            if (pos < str_size) {
                ++pos;  // пропуск спецификатора ('d', 'u', 'f', 's')
            }
        }

        // проверка '}', сохранение начала и конца плейсхолдера
        if (pos < str_size && str.data[pos] == '}') {
            ret[ph_num] = {begin_ph, pos};
            ++ph_num;
        }
        ++pos;
    }
    return ret;
}

}  // namespace stdx::details
