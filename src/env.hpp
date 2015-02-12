#ifndef ENV
#define ENV

#include "utils.hpp"
#include "heap.hpp"
#include "stack.hpp"
//#include "scope.hpp"

struct CodeFunction;
struct HeapObject;
struct Scope;

struct Env {
  Heap *heap;
  Stack *stack;
  Scope *currentScope;

  Env();

};

#endif
