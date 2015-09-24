#pragma once

#include "utils.hpp"
#include "scope.hpp"
#include "reference.hpp"
#include "exception.hpp"
#include "function_arguments.hpp"
#include "baseobject.hpp"


class Line;
class Env;
class Function;

class Function : public BaseObject {

protected:
    ExceptionContext context;
    size_t parameter_count;

public:
    Function(Env *env, ExceptionContext &context, size_t parameter_count)
        : BaseObject(env, "Function"), context(context), parameter_count(parameter_count) {}

    virtual rref exec(std::vector<rref> &arguments);

    ExceptionContext getContext(){
        return context;
    }

    bool isFunction(){
        return true;
    }

    virtual rref exec(FunctionArguments&);

    size_t parameterCount(){
        return parameter_count;
    }
};


class BoundFunction : public Function {
protected:
    SmartReference<Function> function;
    rref this_obj;
    Optional<std::string> name;

public:
    BoundFunction(Env *env, ExceptionContext &context,
                  SmartReference<Function> function, rref this_obj, std::string name = "");

    rref exec(FunctionArguments &args);
};

class Functional : public Function {
protected:
    SmartReference<Scope> parent_scope;
    std::string name;
public:
    Functional(Env *env, ExceptionContext &context,
             size_t parameter_count, SmartReference<Scope> parent_scope);

    virtual std::string str();

    virtual bref bind(rref this_obj, std::string name = ""){
        return make_bref<BoundFunction>(env, context, SmartReference<Function>(this), this_obj.copy(), name);
    }

    SmartReference<Scope> getParentScope(){
        return parent_scope.copy();
    }
};

class CodeFunction : public Functional {
    std::vector<Line*> lines;
    std::vector<std::string> parameters;

public:
    CodeFunction(Env *env, ExceptionContext context, SmartReference<Scope> parent_scope,
                 std::vector<std::string> parameters, std::vector<Line*> lines);

    SmartReference<Scope> initFunctionScope(FunctionArguments &args);

    rref exec(FunctionArguments &args);
};

class CPPFunction : public Functional {

    std::function<rref(FunctionArguments&)> impl_func;

public:
    CPPFunction(Env *env, ExceptionContext context, SmartReference<Scope> parent_scope, size_t parameter_count,
                std::function<rref(FunctionArguments&)> impl_func)
        : Functional(env, context, parameter_count, parent_scope), impl_func(impl_func) {}

    rref exec(FunctionArguments &arg);
};

struct FunctionTuple {
    ExceptionContext context;
    std::string name;
    size_t param_count;
    std::function<rref(FunctionArguments &)> impl_func;

    FunctionTuple(ExceptionContext context, std::string name, size_t param_count,
                  std::function<rref(FunctionArguments &)> impl_func)
        : context(context), name(name), param_count(param_count), impl_func(impl_func) {}
};
