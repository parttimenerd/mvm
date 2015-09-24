#ifndef _L

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define _LEFT(n) ""; std::cout.setf(std::ios::left); std::cout << std::setw(n)
#define _RIGHT(n) ""; std::cout.setf(std::ios::right); std::cout << std::setw(n)

#define _CONTEXT  std::cout << _LEFT(12) << __FILENAME__\
    << "[" << _RIGHT(4) << __LINE__ << \
    _LEFT(3) << "], " << _RIGHT(0) << std::setw(20) << __PRETTY_FUNCTION__
#define _L _CONTEXT << "\n";
#define _LOG(msg) _CONTEXT << msg << "\n";
#define _LOG_VAR(var) _CONTEXT << std::setw(10) << #var\
    << " = " << var << "\n";

#endif
