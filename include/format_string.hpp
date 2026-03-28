#pragma once

#include <expected>

#include "types.hpp"

namespace stdx::details {

// Шаблонный класс для хранения форматирующей строчки и ее особенностей
// ваш код здесь
class format_string {
    // ваш код здесь
};

// Пользовательский литерал
/*
ваш код здесь
ваш код здесь operator"" _fs()  сигнатуру также поменяйте
{
ваш код здесь
}
*/

// Функция для получения количества плейсхолдеров и проверки корректности формирующей строки
// Функция закомментирована, так как еще не реализованы классы, которые она использует
/*
// Сделайте эту свободную функцию методом класса format_string
template<fixed_string str>
consteval std::expected<size_t, parse_error> get_number_placeholders() {
    constexpr size_t N = str.size();
    if (!N)
        return 0;
    size_t placeholder_count = 0;
    size_t pos = 0;
    const size_t size = N - 1; // -1 для игнорирования нуль-терминатора

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
*/

// Функция для получения позиций плейсхолдеров

// ваш код здесь
void get_placeholder_positions() {  // сигнатуру тоже нужно изменить
    // ваш код здесь
}

} // namespace stdx::details
