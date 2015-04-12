#pragma once 
/** Don't modify this file. It's auto generated. */

#include "utils.hpp"
#include "math.hpp"
#include "string.hpp"
#include "io.hpp"
#include "utility.hpp"
#include "logic.hpp"


void loadStdLib(Env *env){
	
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 47, 0), "plus"), "plus", 2, [](Env *env, FunctionArguments &args){
		
		for (size_t i = -1; i < args.all_arguments.size(); i++){
                        
		if (!(args.at(i)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(i)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 47, 0), "plus"));
		}

		}
		
		return plus(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 57, 0), "plus!"), "plus!", 2, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 2 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("math.hpp"), 57, 0), "plus!"));	
		}

		if (!(args.at(0)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(0)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 57, 0), "plus!"));
		}

		if (!(args.at(1)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(1)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 57, 0), "plus!"));
		}

		return plus_set(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 64, 0), "minus"), "minus", 2, [](Env *env, FunctionArguments &args){
		
		for (size_t i = -1; i < args.all_arguments.size(); i++){
                        
		if (!(args.at(i)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(i)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 64, 0), "minus"));
		}

		}
		
		return minus(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 74, 0), "minus!"), "minus!", 2, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 2 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("math.hpp"), 74, 0), "minus!"));	
		}

		if (!(args.at(0)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(0)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 74, 0), "minus!"));
		}

		if (!(args.at(1)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(1)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 74, 0), "minus!"));
		}

		return minus_set(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 80, 0), "multiply"), "multiply", 2, [](Env *env, FunctionArguments &args){
		
		for (size_t i = -1; i < args.all_arguments.size(); i++){
                        
		if (!(args.at(i)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(i)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 80, 0), "multiply"));
		}

		}
		
		return multiply(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 90, 0), "multiply!"), "multiply!", 2, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 2 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("math.hpp"), 90, 0), "multiply!"));	
		}

		if (!(args.at(0)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(0)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 90, 0), "multiply!"));
		}

		if (!(args.at(1)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(1)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 90, 0), "multiply!"));
		}

		return multiply_set(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 96, 0), "divide"), "divide", 2, [](Env *env, FunctionArguments &args){
		
		for (size_t i = -1; i < args.all_arguments.size(); i++){
                        
		if (!(args.at(i)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(i)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 96, 0), "divide"));
		}

		}
		
		return divide(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 106, 0), "divide!"), "divide!", 2, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 2 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("math.hpp"), 106, 0), "divide!"));	
		}

		if (!(args.at(0)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(0)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 106, 0), "divide!"));
		}

		if (!(args.at(1)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(1)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 106, 0), "divide!"));
		}

		return divide_set(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 112, 0), "modulo"), "modulo", 2, [](Env *env, FunctionArguments &args){
		
		for (size_t i = -1; i < args.all_arguments.size(); i++){
                        
		if (!(args.at(i)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(i)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 112, 0), "modulo"));
		}

		}
		
		return modulo(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 122, 0), "modulo!"), "modulo!", 2, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 2 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("math.hpp"), 122, 0), "modulo!"));	
		}

		if (!(args.at(0)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(0)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 122, 0), "modulo!"));
		}

		if (!(args.at(1)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(1)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 122, 0), "modulo!"));
		}

		return modulo_set(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 128, 0), "pow"), "pow", 2, [](Env *env, FunctionArguments &args){
		
		for (size_t i = -1; i < args.all_arguments.size(); i++){
                        
		if (!(args.at(i)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(i)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 128, 0), "pow"));
		}

		}
		
		return pow(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("math.hpp"), 138, 0), "pow!"), "pow!", 2, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 2 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("math.hpp"), 138, 0), "pow!"));	
		}

		if (!(args.at(0)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(0)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 138, 0), "pow!"));
		}

		if (!(args.at(1)->value->isNumeric())){
			throw new Exception("wrong argument type", "Expected numeric but got " + args.at(1)->value->str(), ExceptionContext(LangContext(new std::string("math.hpp"), 138, 0), "pow!"));
		}

		return pow_set(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("string.hpp"), 5, 0), "add_strings"), "add_strings", 2, [](Env *env, FunctionArguments &args){
		
		return add_strings(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("string.hpp"), 15, 0), "add_strings!"), "add_strings!", 2, [](Env *env, FunctionArguments &args){
		
		return add_strings(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("io.hpp"), 5, 0), "print"), "print", 0, [](Env *env, FunctionArguments &args){
		
		print(env, args);
		return env->snothing().reference; 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("io.hpp"), 12, 0), "puts"), "puts", 0, [](Env *env, FunctionArguments &args){
		
		puts(env, args);
		return env->snothing().reference; 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("utility.hpp"), 6, 0), "print_env"), "print_env", 0, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 0 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("utility.hpp"), 6, 0), "print_env"));	
		}

		print_env(env, args);
		return env->snothing().reference; 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("utility.hpp"), 13, 0), "set_direct"), "set_direct", 2, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 2 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("utility.hpp"), 13, 0), "set_direct"));	
		}

		return set_direct(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("utility.hpp"), 21, 0), "eval"), "eval", 1, [](Env *env, FunctionArguments &args){
		
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << "Expected 1 arguments, but got ";
			stream << args.all_arguments.size();
			throw new Exception("too much arguments", stream.str(), ExceptionContext(LangContext(new std::string("utility.hpp"), 21, 0), "eval"));	
		}

		if (!(args.at(0)->value->type == Type::STRING)){
			throw new Exception("wrong argument type", "Expected string but got " + args.at(0)->value->str(), ExceptionContext(LangContext(new std::string("utility.hpp"), 21, 0), "eval"));
		}

		return eval(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("logic.hpp"), 17, 0), "not_equal?"), "not_equal?", 2, [](Env *env, FunctionArguments &args){
		
		return not_equal(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("logic.hpp"), 25, 0), "equal?"), "equal?", 2, [](Env *env, FunctionArguments &args){
		
		return equal(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("logic.hpp"), 33, 0), "less?"), "less?", 2, [](Env *env, FunctionArguments &args){
		
		return less(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("logic.hpp"), 41, 0), "greater?"), "greater?", 2, [](Env *env, FunctionArguments &args){
		
		return greater(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("logic.hpp"), 49, 0), "less_equal?"), "less_equal?", 2, [](Env *env, FunctionArguments &args){
		
		return less_equal(env, args);
		 
	});
	env->addFunction(ExceptionContext(LangContext(new std::string("logic.hpp"), 57, 0), "greater_equal?"), "greater_equal?", 2, [](Env *env, FunctionArguments &args){
		
		return greater_equal(env, args);
		 
	});
}

