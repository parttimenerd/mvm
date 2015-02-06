#include "env.hpp"
#include "scope.hpp"

Env::Env(){
    heap = new Heap();
    stack = new Stack();
    currentScope = new Scope(heap);
}

Env::~Env(){
    delete heap;
    delete stack;
}
