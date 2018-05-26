#pragma once
#include "./a.hpp"

/// some function using A
inline void print_a2(A const &a) { std::cout << "module B : my a is " << a.x << std::endl; }
