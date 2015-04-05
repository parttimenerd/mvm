#include "utils.hpp"

#include "function.hpp"
#include "scope.hpp"
#include "heapobject.hpp"
#include "heap.hpp"
#include "parser.hpp"
#include "array.hpp"
#include "nothing.hpp"
#include "string.hpp"
#include "exception.hpp"

Function::Function(Env *env, ExceptionContext location, Scope *parent_scope,
                   size_t parameter_count)
    : Map(env), location(location), parent_scope(parent_scope), parameter_count(parameter_count){
    type = Type::FUNCTION;
    Map::set(env->sstring("parameter_count").value,
        env->sinteger((int_type)parameter_count).reference);
    parent_scope->reference();
}

void Function::exec(std::vector<Reference<HeapObject>*> &arguments){
    if (parameter_count > arguments.size()){
        throw std::string("not enough arguments");
    } else {
        std::vector<Reference<HeapObject>*> args(arguments.begin(), arguments.begin() + parameter_count);
        std::vector<Reference<HeapObject>*> misc_args(arguments.begin() + parameter_count, arguments.end());
        FunctionArguments argss(this, args, misc_args, arguments);
        try {
            exec(argss);
        } catch (Exception *ex){
            ex->addContext(location);
            throw ex;
        }
    }
}

CodeFunction::CodeFunction(Env *env, ExceptionContext location, Scope *parent_scope,
    std::vector<std::string> parameters, std::vector<Line*> lines)
    : Function(env, location, parent_scope, 0),
    lines(lines), parameters(parameters) {
    parameter_count = parameters.size();
}

Scope* CodeFunction::initFunctionScope(FunctionArguments &args){
    Scope *scope = parent_scope->createChild();
    for (size_t i = 0; i < parameters.size(); i++){
        scope->setHere(parameters.at(i), args.arguments.at(i));
    }
    scope->setHere(std::string("arguments"), env->array(args.misc_arguments));
    scope->setHere(std::string("self"), make_sref(args.self).reference);
    return scope;
}

void CodeFunction::exec(FunctionArguments &args){
    Scope *functionBaseScope = initFunctionScope(args);
    env->stack->pushFrame();
    Exception *exception = 0;
    try {
        env->interpret(functionBaseScope, lines);
    } catch (Exception *ex){
        exception = ex;
    }

    Reference<HeapObject>* returnVal = env->stack->pop();
    env->stack->popFrameAndAddReturn(returnVal);
    while (functionBaseScope != parent_scope){
        Scope *parent = functionBaseScope->parent;
        parent->reference();
        functionBaseScope->dereference();
        functionBaseScope = parent;
    }

    for (auto &arg : args.all_arguments){
        arg->dereference();
    }

    if (exception != 0){
        throw exception;
    }
}

void CPPFunction::exec(FunctionArguments &args){
   auto ret_val = this->impl_func(env, args);
   Exception *exception = 0;
   try {
       env->stack->push(ret_val);
   } catch (Exception *ex){
       exception = ex;
   }
   for (auto &arg : args.all_arguments){
       arg->dereference();
   }
   if (exception != 0){
       throw exception;
   }
}
