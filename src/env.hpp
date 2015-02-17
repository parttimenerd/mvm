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
struct String;
struct Line;
struct Parser;

struct Env {
    Heap *heap;
    Stack *stack;
    Scope *root_scope;
    Nothing *nothing;

    Env();

    /**
	 * Dereference the object and delete it if its reference count is zero
	 */
	void dereference(HeapObject *object);

	void add(HeapObject *obj);

    Int* createInt(int_type val, bool reference = true);

    Nothing* createNothing(bool reference = true);

    Array* createArray(std::vector<HeapObject*> value = std::vector<HeapObject*>(), bool reference = true);

    Map* createMap(bool reference = true);

    Boolean* createBoolean(bool isTrue, bool reference = true);

    String* createString(std::string value, bool reference = true);

    CodeFunction* createFunction(Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines, bool reference = true);

    void interpret(Scope *function_base_scope, std::vector<Line*> code);

    void interpret(Parser *parser);

    /**
     * Add the passed function to the root scope.
     */
    void addFunction(std::string name, size_t parameter_count,
        std::function<HeapObject*(Env *env, std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments)> implFunc);

};
