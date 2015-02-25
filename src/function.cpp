#include "utils.hpp"

#include "function.hpp"
#include "scope.hpp"
#include "heapobject.hpp"
#include "heap.hpp"
#include "parser.hpp"
#include "array.hpp"
#include "nothing.hpp"

CodeFunction::CodeFunction(Env *env, Scope *parent_scope,
    std::vector<std::string> parameters, std::vector<Line*> lines) : Function(env, 0) {
    this->parent_scope = parent_scope;
    this->parameter_count = parameters.size();
    this->parameters = parameters;
    this->lines = lines;
}

Scope* CodeFunction::initFunctionScope(FunctionArguments args){
    Scope *scope = parent_scope->createChild();
    for (size_t i = 0; i < parameters.size(); i++){
        scope->setHere(parameters.at(i), args.arguments.at(i));
    }
    scope->setHere(std::string{"arguments"}, env->createArray(args.misc_arguments, true));
    return scope;
}

void CodeFunction::exec(FunctionArguments args){
    Scope *functionBaseScope = initFunctionScope(args);
    env->stack->pushFrame();

    // execute the code lines and wrap it with a try catch

    env->interpret(functionBaseScope, lines);

    HeapObject *returnVal = env->stack->pop();
    env->stack->popFrameAndAddReturn(returnVal);
    while (functionBaseScope != parent_scope){
        Scope *parent = functionBaseScope->parent;
        parent->reference();
        functionBaseScope->dereference();
        functionBaseScope = parent;
    }

}

void CPPFunction::exec(FunctionArguments args){
   auto ret_val = this->impl_func(env, args);
   env->stack->push(ret_val);
   for (auto &arg : args.all_arguments){
       arg->dereference();
   }
}
