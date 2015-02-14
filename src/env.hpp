#pragma once

#include "utils.hpp"
#include "heap.hpp"
#include "stack.hpp"
//#include "scope.hpp"

struct CodeFunction;
struct HeapObject;
struct Scope;
struct Int;
struct Nothing;
struct Array;
struct Map;
struct Boolean;
struct Function;

struct Env {
    Heap *heap;
    Stack *stack;
    Scope *root_scope;

    Env();

    /**
	 * Dereference the object and delete it if its reference count is zero
	 */
	void dereference(HeapObject *object);

	void add(HeapObject *obj);

    Int* createInt(int_type val, bool reference = false);

    Nothing* createNothing(bool reference = false);

    Array* createArray(std::vector<HeapObject*> value, bool reference = false);

    Map* createMap(bool reference = false);

    Boolean* createBoolean(bool isTrue, bool reference = false);

    /**
     * Add the passed function to the root scope.
     */
    void addFunction(std::string name, size_t parameter_count,
        std::function<HeapObject*(Env *env, std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments)> implFunc);

};
