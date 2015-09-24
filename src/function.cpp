#include "utils.hpp"

#include "function.hpp"
#include "scope.hpp"
#include "heapobject.hpp"
#include "baseobject.hpp"
#include "heap.hpp"
#include "parser.hpp"
#include "nothing.hpp"
#include "string.hpp"
#include "exception.hpp"
#include "env.hpp"
#include "stack.hpp"


rref Function::exec(std::vector<rref> &arguments){
    if (parameter_count > arguments.size()){
        throw std::string("not enough arguments");
    } else {
        std::vector<rref> args(arguments.begin(), arguments.begin() + parameter_count);
        std::vector<rref> misc_args(arguments.begin() + parameter_count, arguments.end());
        FunctionArguments argss(env, SmartReference<Function>(this), args, misc_args, arguments);
        try {
            return exec(argss);
        } catch (Exception *ex){
            ex->addContext(context);
            throw ex;
        }
    }
}

BoundFunction::BoundFunction(Env *env, ExceptionContext &context,
                             SmartReference<Function> function, rref this_obj,
                             std::string name)
    : Function(env, context, function->parameterCount()), function(function), this_obj(this_obj) {
    this->context.setThis(this_obj->value().copy());
    this->context.setSelf(function.copy());
    if (name != ""){
        this->name = Optional<std::string>(name);
    } else {
        this->name = Optional<std::string>();
    }
}

rref BoundFunction::exec(FunctionArguments &args){
    args.setThis(this_obj.copy());
    try {
        return function->exec(args);
    } catch (Exception *ex){
        ex->addContext(context);
        throw ex;
    }
}


Functional::Functional(Env *env, ExceptionContext &context,
                       size_t parameter_count, SmartReference<Scope> parent_scope)
    : Function(env, context, parameter_count), parent_scope(parent_scope) {
    this->context.setFunctionParentScope(parent_scope);
    this->context.setSelf(SmartReference<Function>(this));
}

std::string Functional::str(){
    std::ostringstream stream;
    stream << "function " << context.context().str()
           << " with " << parameter_count << " parameters";
    return stream.str();
}


CodeFunction::CodeFunction(Env *env, ExceptionContext context,
                           SmartReference<Scope> parent_scope,
                           std::vector<std::string> parameters,
                           std::vector<Line*> lines)
    : Functional(env, context, 0, parent_scope),
      lines(lines), parameters(parameters) {
    parameter_count = parameters.size();
}

SmartReference<Scope> CodeFunction::initFunctionScope(FunctionArguments &args){
    SmartReference<Scope> scope = parent_scope->createChild();
    for (size_t i = 0; i < parameters.size(); i++){
        scope->setHere(parameters.at(i), args.at(i).copy());
    }
    //scope->setHere(std::string("arguments"), env->array(args.misc_arguments));
    scope->setHere(std::string("self"), to_rref(bref(this)));
    if (args.hasThis()){
        scope->setHere(std::string("this"), args.getRThis().copy());
    }
    return scope.copy();
}

rref CodeFunction::exec(FunctionArguments &args){
    auto functionBaseScope = initFunctionScope(args);
    env->stack()->pushFrame();
    Exception *exception = 0;
    try {
        env->interpret(functionBaseScope, lines);
    } catch (Exception *ex){
        exception = ex;
    }

    rref returnVal = env->stack()->pop();
    env->stack()->popFrame();
    while (functionBaseScope->id() != parent_scope->id()){
        SmartReference<Scope> parent = functionBaseScope->parent();
        functionBaseScope = parent;
    }

    if (exception != 0){
        throw exception;
    }

    return returnVal;
}

rref CPPFunction::exec(FunctionArguments &args){
    auto ret_val = this->impl_func(args);
    Exception *exception = 0;
    try {
        return ret_val;
    } catch (Exception *ex){
        exception = ex;
    }
    if (exception != 0){
        throw exception;
    }
}


rref Function::exec(FunctionArguments &){
    return env->rnothing();
}
