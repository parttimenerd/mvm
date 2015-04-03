#include "utils.hpp"

#include "function.hpp"
#include "scope.hpp"
#include "heapobject.hpp"
#include "heap.hpp"
#include "parser.hpp"
#include "array.hpp"
#include "nothing.hpp"
#include "string.hpp"

Function::Function(Env *env, size_t parameter_count) : Map(env) {
    type = Type::FUNCTION;
    Map::set(make_sref(env->string("parameter_count")).value,
        (Reference<HeapObject>*)make_sref(env->integer((int_type)parameter_count)).reference);
    this->parameter_count = parameter_count;
}

void Function::exec(std::vector<Reference<HeapObject>*> &arguments){
    if (parameter_count > arguments.size()){
        throw std::string("not enough arguments");
    } else {
        std::vector<Reference<HeapObject>*> args(arguments.begin(), arguments.begin() + parameter_count);
        std::vector<Reference<HeapObject>*> misc_args(arguments.begin() + parameter_count, arguments.end());
        FunctionArguments argss(this, args, misc_args, arguments);
        exec(argss);
    }
}

CodeFunction::CodeFunction(Env *env, Scope *parent_scope,
    std::vector<std::string> parameters, std::vector<Line*> lines) : Function(env, 0) {
    this->parent_scope = parent_scope;
    this->parameter_count = parameters.size();
    this->parameters = parameters;
    this->lines = lines;
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

    // execute the code lines and wrap it with a try catch
    env->interpret(functionBaseScope, lines);

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
}

void CPPFunction::exec(FunctionArguments &args){
   auto ret_val = this->impl_func(env, args);
   env->stack->push(ret_val);
   for (auto &arg : args.all_arguments){
       arg->dereference();
   }
}
