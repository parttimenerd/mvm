#ifndef ENV
#define ENV

#include "heap.hpp"
#include "stack.hpp"
//#include "scope.hpp"

struct Scope;

struct Env {
  Heap *heap;
  Stack *stack;
  Scope *currentScope;

  Env();

  ~Env();
};

#endif
