# scan: статическая функция compile-time парсинга

Проект реализует статическую функцию `stdx::scan`, которая извлекает значения из строки по форматному шаблону на этапе компиляции (`consteval`).

Идея простая:
- вы задаете форматную строку с плейсхолдерами;
- передаете исходную строку;
- указываете ожидаемые типы;
- получаете типобезопасный результат с проверками в compile-time.

## Что умеет `scan`

- Парсит значения из строкового шаблона по плейсхолдерам `{...}`.
- Проверяет корректность формата во время компиляции.
- Проверяет соответствие числа плейсхолдеров и количества типов.
- Проверяет соответствие спецификатора и типа (например, `%u` для unsigned).
- Возвращает результат в `stdx::details::scan_result<Ts...>`.

## Поддерживаемые плейсхолдеры

Поддерживаются два вида:

- `{}` - без спецификатора (тип определяется по шаблонному параметру);
- `{%x}` - со спецификатором.

Поддерживаемые спецификаторы:

- `%d` - знаковые целые (`std::signed_integral`)
- `%u` - беззнаковые целые (`std::unsigned_integral`)
- `%f` - числа с плавающей точкой (`std::floating_point`)
- `%s` - `std::string_view`

## Публичный API

Основная функция объявлена в `include/scan.hpp`:

```cpp
template <details::format_string fmt, details::fixed_string source, typename... Ts>
consteval details::scan_result<Ts...> scan();
```

Также используется пользовательский литерал для форматной строки (`include/format_string.hpp`):

```cpp
using namespace stdx::details;
constexpr format_string fmt = "value:{%u}"_fs;
```

## Пример использования

```cpp
#include "scan.hpp"
#include "format_string.hpp"

using namespace stdx::details;

constexpr fixed_string src = "user:alice age:24 score:91.5";
constexpr format_string fmt = "user:{%s} age:{%u} score:{%f}"_fs;

constexpr auto result = stdx::scan<fmt, src, std::string_view, uint32_t, double>();

static_assert(result.template values<0>() == "alice");
static_assert(result.template values<1>() == 24);
static_assert(result.template values<2>() == 91.5);
```

## Ограничения и важные детали

- `scan` является `consteval`: вызов должен быть вычислим на этапе компиляции.
- Для `%f` разрешены:
  - опциональный знак `+`/`-` в начале;
  - не более одного десятичного разделителя (`.` или `,`);
  - как минимум одна цифра.
- Неподдерживаемые типы и неверные форматы приводят к ошибкам компиляции (`static_assert`).

## Сборка

### Через VS Code Tasks

В проекте уже есть задачи:

- `GCC: Build Debug app`
- `GCC: Build Release app`

### Через CMake вручную

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

## Запуск

Исполняемый файл с проверками:

```bash
./build/scan_tests
```

Если программа завершается успешно (код 0), все `static_assert` проверки пройдены.

## Структура проекта

- `include/scan.hpp` - точка входа и функция `stdx::scan`
- `include/format_string.hpp` - разбор и валидация форматной строки
- `include/parse.hpp` - извлечение подстрок и парсинг в типы
- `include/types.hpp` - `fixed_string`, `parse_error`, `scan_result`
- `tests/main.cpp` - compile-time тесты и примеры сценариев
