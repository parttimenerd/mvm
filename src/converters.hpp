#ifndef CONVERTERS_HPP
#define CONVERTERS_HPP

#include "utils.hpp"

bool to_bool(rref obj);
bool to_bool(bref obj);
rref to_rref(bool boolean);
bref to_bref(bool boolean);

std::string to_string(rref obj);
std::string to_string(bref obj);
rref to_rref(std::string str);
bref to_bref(std::string str);

rref to_rref(bref obj);

#endif // CONVERTERS_HPP
