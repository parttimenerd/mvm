#include "utils.hpp"
#include "scope.hpp"
#include "heapobject.hpp"


struct Line;
struct Env;

struct FunctionArguments {
    std::vector<HeapObject*> arguments;
    std::vector<HeapObject*> misc_arguments;
    std::vector<HeapObject*> all_arguments;

    FunctionArguments(std::vector<HeapObject*> arguments,
        std::vector<HeapObject*> misc_arguments,
        std::vector<HeapObject*> all_arguments){
        this->arguments = arguments;
        this->misc_arguments = misc_arguments;
        this->all_arguments = all_arguments;
    }

    bool hasMiscArguments(){
        return !misc_arguments.empty();
    }
};

struct Function : HeapObject {
	size_t parameter_count;
	std::string name = "";

	Function(Env *env, size_t parameter_count) : HeapObject(Type::FUNCTION, env) {
        this->parameter_count = parameter_count;
	}

    virtual void exec(FunctionArguments args) = 0;

    void exec(std::vector<HeapObject*> arguments){
        if (parameter_count > arguments.size()){
            throw std::string("not enough arguments");
        } else {
            std::vector<HeapObject*> args(arguments.begin(), arguments.begin() + parameter_count);
            std::vector<HeapObject*> misc_args(arguments.begin() + parameter_count, arguments.end());
            exec(FunctionArguments(args, misc_args, arguments));
        }
    }

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

    Scope* initFunctionScope(FunctionArguments args);

    void exec(FunctionArguments args);
};

struct CPPFunction : Function {

    std::function<HeapObject*(Env*, FunctionArguments)> impl_func;

    CPPFunction(Env *env, size_t parameter_count, std::function<HeapObject*(Env *env, FunctionArguments)> impl_func) : Function(env, parameter_count){
        this->impl_func = impl_func;
    }

    void exec(FunctionArguments arg);
};
