#include "utils.hpp"
#include "scope.hpp"
#include "heapobject.hpp"
#include "env.hpp"

struct Line;

struct Function : HeapObject {
	size_t parameter_count;

	Function(Heap *heap, size_t parameter_count) : HeapObject(Type::FUNCTION, heap) {
        this->parameter_count = parameter_count;
	}

    virtual void exec(Env *env, std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments) = 0;

    void exec(Env *env, std::vector<HeapObject*> arguments){
        if (parameter_count > arguments.size()){
            throw "not enough arguments";
        } else {
            std::vector<HeapObject*> args(arguments.begin(), arguments.begin() + parameter_count);
            std::vector<HeapObject*> misc_args(arguments.begin() + parameter_count, arguments.end());
            env->stack->pushFrame();
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

    CodeFunction(Heap *heap, Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines);

	Scope* initFunctionScope(std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments);

    void exec(Env *env, std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments);
};
