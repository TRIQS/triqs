#pragma once

#if __has_include(<generator>) // Check if <generator> exists
#include <generator>
using std::generator;
#else
#include <triqs/third_party/cppcoro/generator.hpp>
using cppcoro::generator;
#endif
