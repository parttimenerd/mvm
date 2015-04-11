#pragma once

#include "utils.hpp"

void print_env(Env *env, FunctionArguments&){
    std::cout << env->heap->str();
    std::cout << env->stack->str();
}

//STATEMENT(print_env);

