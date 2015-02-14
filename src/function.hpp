#include "utils.hpp"
#include "scope.hpp"
#include "heapobject.hpp"


struct Line;
struct Env;

struct Function : HeapObject {
	size_t parameter_count;

	Function(Env *env, size_t parameter_count) : HeapObject(Type::FUNCTION, env) {
        this->parameter_count = parameter_count;
	}

    virtual void exec(std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments) = 0;

    void exec(std::vector<HeapObject*> arguments){
        if (parameter_count > arguments.size()){
            throw "not enough arguments";
        } else {
            std::vector<HeapObject*> args(arguments.begin(), arguments.begin() + parameter_count);
            std::vector<HeapObject*> misc_args(arguments.begin() + parameter_count, arguments.end());
        }
    }

    std::string str(){
        std::ostringstream stream;
        stream << "function with " << parameter_count << " parameters";
        return stream.str();
    }
};

struct CodeFunction : Function {
	Scope *parent_scope;
	std::vector<Line*> lines;
	std::vector<std::string> parameters;

    CodeFunction(Env *env, Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines);

	Scope* initFunctionScope(std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments);

    void exec(std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments);
};

struct CPPFunction : Function {

    std::function<HeapObject*(Env *env, std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments)> impl_func;

    CPPFunction(Env *env, size_t parameter_count, std::function<HeapObject*(Env *env, std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments)> impl_func) : Function(env, parameter_count){
        this->impl_func = impl_func;
    }

    void exec(std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments);
};
