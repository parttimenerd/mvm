#include "utils.hpp"
#include "scope.hpp"
#include "reference.hpp"
#include "map.hpp"


struct Line;
struct Env;
struct Function;

struct FunctionArguments {
    Function *self;
    std::vector<Reference<HeapObject>*> arguments;
    std::vector<Reference<HeapObject>*> misc_arguments;
    std::vector<Reference<HeapObject>*> all_arguments;

    FunctionArguments(Function *self, std::vector<Reference<HeapObject>*> arguments,
        std::vector<Reference<HeapObject>*> misc_arguments,
        std::vector<Reference<HeapObject>*> all_arguments){
        this->self = self;
        this->arguments = arguments;
        this->misc_arguments = misc_arguments;
        this->all_arguments = all_arguments;
    }

    bool hasMiscArguments(){
        return !misc_arguments.empty();
    }

    bool hasTooMuchArguments(){
        return hasMiscArguments();
    }
};

struct Function : Map {
	size_t parameter_count;
	std::string name = "";

    Function(Env *env, size_t parameter_count);

    virtual void exec(FunctionArguments&){}

    void exec(std::vector<Reference<HeapObject>*> &arguments);

    std::string str(){
        std::ostringstream stream;
        stream << "function " << name << " with " << parameter_count << " parameters";
        return stream.str();
    }
};

struct CodeFunction : Function {
	Scope *parent_scope;
	std::vector<Line*> lines;
	std::vector<std::string> parameters;

    CodeFunction(Env *env, Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines);

    Scope* initFunctionScope(FunctionArguments &args);

    void exec(FunctionArguments &args);
};

struct CPPFunction : Function {

    std::function<Reference<HeapObject>*(Env*, FunctionArguments)> impl_func;

    CPPFunction(Env *env, size_t parameter_count, std::function<Reference<HeapObject>*(Env *env, FunctionArguments)> impl_func) : Function(env, parameter_count){
        this->impl_func = impl_func;
    }

    void exec(FunctionArguments &arg);
};
