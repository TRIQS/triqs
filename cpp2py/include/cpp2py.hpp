#pragma once

#define CPP2PY_IGNORE  __attribute__((annotate("ignore_in_python")))
#define CPP2PY_ARG_AS_DICT __attribute__((annotate("use_parameter_class"))) 

#include "./cpp2py/signal_handler.hpp"
#include "./cpp2py/exceptions.hpp"
#include "./cpp2py/pyref.hpp"
#include "./cpp2py/py_converter.hpp"
#include "./cpp2py/misc.hpp"
#include "./cpp2py/converters/basic_types.hpp"
