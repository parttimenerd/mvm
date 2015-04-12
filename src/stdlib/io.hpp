#pragma once

#include "utils.hpp"

//statement print, 0 | var
void print(Env*, FunctionArguments &arg){
    for (auto obj : arg.all_arguments){
        std::cout << obj->value->str();
    }
}

//statement puts, 0 | var
void puts(Env* env, FunctionArguments &arg){
    print(env, arg);
    std::cout << "\n";
}

