#pragma once

#include "utils.hpp"


//statement print_env

void print_env(Env *env, FunctionArguments&){
    std::cout << env->heap->str();
    std::cout << env->stack->str();
}

//function set_direct, 2

href_ptr set_direct(Env*, FunctionArguments &args){
    args.first()->value->set(args.second());
    args.first()->reference();
    return args.first();
}

