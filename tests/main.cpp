#include "scan.hpp"

int main() { 
    static_assert(stdx::scan<stdx::details::format_string{}, stdx::details::fixed_string{}, int>().i == 42); 
}
