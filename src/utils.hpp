#pragma once

#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <functional>
#include <tuple>
#include <exception>
#include <iomanip>

using namespace std::string_literals;

/** Type of a normal id in the following */
typedef uint64_t id_type;
/** Type of the value of an Int object */
typedef int64_t int_type;

typedef double float_type;

typedef uint32_t counter_type;

//#ifdef DO_LOG
#include "logging.hpp"
#define L _L
#define LOG(msg) _LOG(msg)
#define LOG_VAR(var) _LOG_VAR(#var)
//#endif

#include "smartreference.hpp"
#include "optional.hpp"
#include "converters.hpp"

class ExceptionContext;
class FunctionArguments;
class FunctionTuple;
