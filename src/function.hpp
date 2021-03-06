#include "utils.hpp"
#include "scope.hpp"
#include "reference.hpp"
#include "map.hpp"
#include "exception.hpp"
#include "function_arguments.hpp"


struct Line;
struct Env;
struct Function;

struct Function : Map {

    ExceptionContext location;
    Scope *parent_scope;
    size_t parameter_count;

    Function(Env *env, ExceptionContext location, Scope *parent_scope, size_t parameter_count);

    virtual void exec(FunctionArguments&){}

    void exec(std::vector<Reference<HeapObject>*> &arguments);

    std::string str(){
        std::ostringstream stream;
        stream << "function " << location.context.str() << " with " << parameter_count << " parameters";
        return stream.str();
    }

    std::vector<HeapObject*> getReferencedObjects() {
        auto vec = Map::getReferencedObjects();
        vec.push_back(parent_scope);
        return vec;
    }
};

struct CodeFunction : Function {
	std::vector<Line*> lines;
	std::vector<std::string> parameters;

    CodeFunction(Env *env, ExceptionContext location, Scope *parent_scope,
                 std::vector<std::string> parameters, std::vector<Line*> lines);

    Scope* initFunctionScope(FunctionArguments &args);

    void exec(FunctionArguments &args);
};

struct CPPFunction : Function {

    std::function<Reference<HeapObject>*(Env*, FunctionArguments&)> impl_func;

    CPPFunction(Env *env, ExceptionContext location, Scope *parent_scope, size_t parameter_count,
                std::function<Reference<HeapObject>*(Env *env, FunctionArguments&)> impl_func)
        : Function(env, location, parent_scope, parameter_count){
        this->impl_func = impl_func;
    }

    void exec(FunctionArguments &arg);
};
