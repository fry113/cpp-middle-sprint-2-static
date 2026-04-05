#pragma once

namespace stdx::details {

// Шаблонный класс, хранящий C-style строку фиксированной длины

// ваш код здесь
struct fixed_string {
    // ваш код здесь
};

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга

// ваш код здесь
struct parse_error {};

// Шаблонный класс для хранения результатов парсинга

template <typename... Ts>
struct scan_result {
// ваш код здесь
// измените реализацию
    int i;
};

} // namespace stdx::details
