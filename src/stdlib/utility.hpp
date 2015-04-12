#pragma once

#include "utils.hpp"
#include <stdio.h>
#include <iostream>
#include <errno.h>

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

//function eval | _eval, 1 | string

href_ptr _eval(Env *env, FunctionArguments &args){
    std::istringstream stream(args.first()->as<String>()->value);
    lang::Lexer lexer(&stream);
    lang::Parser parser(&lexer);
    lang::Node* node = parser.parse();
    std::ostringstream ostream;
    lang::Target target(&ostream);
    node->compile(target);
    std::istringstream istream(ostream.str());
    try {
        env->interpret(new VerboseParser(&istream));
    } catch (std::string str){
        std::cerr << str;
        exit(1);
    }
    auto val = env->stack->pop();
    return val;
}

//function system | _system, 1 | string

href_ptr _system(Env *env, FunctionArguments &args){
    char *cmd = const_cast<char *>(args.first()->as<String>()->value.c_str());
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        return env->integer(errno);
    }
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
       if(fgets(buffer, 128, pipe) != NULL)
           result += buffer;
    }
    pclose(pipe);
    return env->string(result);
}
