#include "format_string.hpp"
#include "scan.hpp"
#include <cstdint>

using namespace stdx::details;

int main() {
    // проверки плавающей точки
    {
        constexpr fixed_string src("str:42.2");
        constexpr format_string<"str:{%f}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, double>();
        static_assert(result.values<0>() == 42.2, "unexpected parse value");
    }
    {
        constexpr fixed_string src("str:.2");
        constexpr format_string<"str:{%f}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, double>();
        static_assert(result.values<0>() == 0.2, "unexpected parse value");
    }
    {
        constexpr fixed_string src("str:2.");
        constexpr format_string<"str:{%f}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, double>();
        static_assert(result.values<0>() == 2.0, "unexpected parse value");
    }
    {
        constexpr fixed_string src("str:2000,2345");
        constexpr format_string<"str:{%f}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, double>();
        static_assert(result.values<0>() == 2000.2345, "unexpected parse value");
    }
    {
        constexpr fixed_string src("str:+9000000000000,");
        constexpr format_string<"str:{%f}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, double>();
        static_assert(result.values<0>() == 9000000000000.0, "unexpected parse value");
    }
    {
        constexpr fixed_string src("str:-.0000000000002");
        constexpr format_string<"str:{%f}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, double>();
        static_assert(result.values<0>() == -0.0000000000002, "unexpected parse value");
    }

    // проверки целых чисел
    {
        constexpr fixed_string src("str:55");
        constexpr format_string<"str:{%u}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, uint8_t>();
        static_assert(result.values<0>() == 55, "unexpected parse value");
    }
    {
        constexpr fixed_string src("str:65535");
        constexpr format_string<"str:{%u}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, uint16_t>();
        static_assert(result.values<0>() == UINT16_MAX, "unexpected parse value");
    }
    {
        constexpr fixed_string src("str:-32768");
        constexpr format_string<"str:{%d}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, int16_t>();
        static_assert(result.values<0>() == INT16_MIN, "unexpected parse value");
    }
    // не скомпилируется "str:-32768"
    // {
    //     constexpr fixed_string src("str:-32768");
    //     constexpr format_string<"str:{%d}"> fmt{};
    //     constexpr auto result = stdx::scan<fmt, src, int8_t>();
    //     static_assert(result.values<0>() == INT8_MIN, "unexpected parse value");
    // }
    {
        constexpr fixed_string src("str:4294967295");
        constexpr format_string<"str:{%u}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, uint32_t>();
        static_assert(result.values<0>() == UINT32_MAX, "unexpected parse value");
    }

    // проверки строк
    {
        constexpr fixed_string src("str:string");
        constexpr format_string<"str:{%s}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, std::string_view>();
        static_assert(result.values<0>() == "string", "unexpected parse value");
    }
    {
        constexpr fixed_string src(
            "str:long-long-looooooooooooog string. so long that i tired to write, so .........................");
        constexpr format_string<"str:{%s}"> fmt{};
        constexpr auto result = stdx::scan<fmt, src, std::string_view>();
        static_assert(
            result.values<0>() ==
                "long-long-looooooooooooog string. so long that i tired to write, so .........................",
            "unexpected parse value");
    }
    {
        constexpr fixed_string src = "str:string";
        constexpr format_string fmt = "str:{%s}"_fs;  // наконец-то заработал пользовательский литерал! П_П
        constexpr auto result = stdx::scan<fmt, src, std::string_view>();
        static_assert(result.values<0>() == "string", "unexpected parse value");
    }
    //  не скомпилируется "str123:string" - "str:{%s}"_fs
    // {
    //     constexpr fixed_string src = "str123:string";
    //     constexpr format_string fmt = "str:{%s}"_fs;
    //     constexpr auto result = stdx::scan<fmt, src, std::string_view>();
    //     static_assert(result.values<0>() == "string", "unexpected parse value");
    // }
    // проверка нескольких плейсхолдеров в длинной строке
    {
        constexpr fixed_string src =
            "a tale about 24 mice and 1 cat (with 6,5 kg of pure laziness). one day Kumkvat "
            "woke up and hunted 23 mice, but one mouse was so fast that Kumkvat couldn't "
            "catch it. and then Kumkvat woke up again and realized that it was all a dream. the end.";
        constexpr format_string fmt =
            "a tale about {%u} mice and {%d} cat (with {%f} kg of pure laziness). one day {%s} woke up and hunted {%u} mice, but one mouse was so fast that Kumkvat couldn't catch it. {%s}"_fs;
        constexpr auto result =
            stdx::scan<fmt, src, uint32_t, int32_t, double, std::string_view, uint32_t, std::string_view>();
        static_assert(result.values<0>() == 24, "unexpected parse value");
        static_assert(result.values<1>() == 1, "unexpected parse value");
        static_assert(result.values<2>() == 6.5, "unexpected parse value");
        static_assert(result.values<3>() == "Kumkvat", "unexpected parse value");
        static_assert(result.values<4>() == 23, "unexpected parse value");
        static_assert(result.values<5>() ==
                          "and then Kumkvat woke up again and realized that it was all a dream. the end.",
                      "unexpected parse value");
    }
}