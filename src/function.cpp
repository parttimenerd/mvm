#include "utils.hpp"

#include "function.hpp"
#include "scope.hpp"
#include "heapobjects.hpp"
#include "heap.hpp"
#include "parser.hpp"

CodeFunction::CodeFunction(Heap *heap, Scope *parent_scope,
    std::vector<std::string> parameters, std::vector<Line*> lines) : Function(heap, 0) {
    this->parent_scope = parent_scope;
    this->parameter_count = parameters.size();
    this->parameters = parameters;
    this->lines = lines;
}

Scope* CodeFunction::initFunctionScope(std::vector<HeapObject*> arguments,
    std::vector<HeapObject*> miscArguments){
    Scope *scope = parent_scope->createChild();
    for (size_t i = 0; i < parameters.size(); i++){
        scope->setHere(parameters.at(i), arguments.at(i));
    }
    scope->setHere("arguments", heap->createArray(miscArguments, true));
    return scope;
}

void CodeFunction::exec(Env *env, std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments){
    Scope *functionBaseScope = initFunctionScope(arguments, miscArguments);
    env->stack->pushFrame();

    // execute the code lines and wrap it with a try catch

    HeapObject *returnVal = heap->createNothing(); //= env->interpreter->interpret(env, functionBaseScope, lines);

    env->stack->popFrameAndAddReturn(returnVal);
    while (functionBaseScope != parent_scope){
        Scope *parent = functionBaseScope->parent;
        parent->reference();
        functionBaseScope->dereference();
        functionBaseScope = parent;
    }

}
