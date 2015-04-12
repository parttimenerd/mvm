#pragma once

#include "utils.hpp"

//function add_strings, 2 | var

href_ptr add_strings(Env *env, FunctionArguments &args){
    std::ostringstream stream;
    for (auto arg : args.all_arguments){
        stream << arg->value->str();
    }
    return env->string(stream.str());
}

//function add_strings! | add_strings, 2 | var

href_ptr add_strings_set(Env *env, FunctionArguments &args){
    auto res = env->call("add_strings", {args.first(), args.second()});
    args.first()->set(res);
    return res;
}
