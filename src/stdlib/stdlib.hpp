#pragma once 
/** Don't modify this file. It's auto generated. */

#include "utils.hpp"
#include "utility.hpp"


void loadStdLib(Env *env){
	
	env->addFunction(ExceptionContext(LangContext(new std::string("utility.hpp"), 10, 0), "print_env"), "print_env", 0, [](Env *env, FunctionArguments &args){
		print_env(env, args);
		return env->snothing().reference; 
	});
}

