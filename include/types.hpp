#pragma once
#include <algorithm>
#include <cstddef>
#include <tuple>

namespace stdx::details {
constexpr size_t pe_size = 80;

// Шаблонный класс, хранящий C-style строку фиксированной длины
template <std::size_t N>
struct fixed_string {
    // c-style массив фикс.размера для хранения строки
    char data[N]{};
    // размер строки
    using size_type = std::size_t;
    static constexpr size_type size_value{N};

    // конструктор, принимающий ссылку на константный массив символов такого же размера
    template <std::size_t K>
        requires(K == N)
    constexpr fixed_string(const char (&str)[K]) {
        std::copy_n(str, N, data);
    }

    // конструктор, принимающий ссылку на константный массив символов меньшего размера
    template <std::size_t K>
        requires(K < N)
    constexpr fixed_string(const char (&str)[K]) {
        std::copy_n(str, K, data);
    }

    // конструктор по двум указателям
    constexpr fixed_string(const char *begin, const char *end) { std::copy(begin, end, data); }

    // явно заданный конструктор по умолчанию
    constexpr fixed_string() = default;

    // метод для получения размера строки (для get_number_placeholders),
    // размер фактический может отличаться от N в меньшую сторону
    constexpr size_type size() const {
        size_type ret = 0;
        while (ret < N && data[ret] != '\0') {
            ++ret;
        }
        return ++ret;
    }
};

// deduction guide
template <std::size_t K>
fixed_string(const char (&)[K]) -> fixed_string<K>;

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга
struct parse_error : fixed_string<pe_size> {
    using size_type = typename fixed_string<pe_size>::size_type;
    // явно заданный конструктор по умолчанию
    constexpr parse_error() = default;

    // конструктор, принимающий строку фиксированной длины
    template <size_type K>
        requires(K <= pe_size)
    constexpr parse_error(const char (&str)[K]) : fixed_string<pe_size>(str) {}

    // конструктор по двум указателям
    constexpr parse_error(const char *begin, const char *end) : fixed_string<pe_size>(begin, end) {}
};

// Шаблонный класс для хранения результатов парсинга
template <typename... Ts>
struct scan_result {
    // картеж для готовых сканированных значений
    std::tuple<Ts...> vals;

    // метод values() для доступа к vals в картеже
    template <std::size_t I>
    constexpr auto values() const {
        return std::get<I>(vals);
    }

    // конструктор
    constexpr scan_result(Ts... args) : vals(args...) {}
};

}  // namespace stdx::details
