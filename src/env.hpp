#pragma once

#include "utils.hpp"
#include "heap.hpp"
#include "stack.hpp"
#include "reference.hpp"
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
struct FunctionArguments;

struct Env {
    Heap *heap;
    Stack *stack;
    Scope *root_scope;
    Nothing*_nothing;

    Env();

    /**
	 * Dereference the object and delete it if its reference count is zero
	 */
	void dereference(HeapObject *object);

    void add(HeapObject *obj);

    Reference<Int>* createInt(int_type val, bool reference = true);
    Reference<Int>* integer(int_type val){
        return createInt(val);
    }

    Reference<Nothing>* createNothing(bool reference = true);

    Reference<Nothing>* nothing(){
        return createNothing();
    }

    Reference<Array>* createArray(std::vector<Reference<HeapObject>*> value = std::vector<Reference<HeapObject>*>(), bool reference = true);

    Reference<Array>* array(std::vector<Reference<HeapObject>*> value = std::vector<Reference<HeapObject>*>()){
        return createArray(value);
    }

    Reference<Map>* createMap(std::map<HeapObject*, Reference<HeapObject>*> value = std::map<HeapObject*, Reference<HeapObject>*>(), bool reference = true);

    Reference<Map>* map(std::map<HeapObject*, Reference<HeapObject>*> value = std::map<HeapObject*, Reference<HeapObject>*>()){
        return createMap(value);
    }

    Reference<Boolean>* createBoolean(bool isTrue, bool reference = true);

    Reference<Boolean>* boolean(bool isTrue){
        return createBoolean(isTrue);
    }

    Reference<String>* createString(std::string value, bool reference = true);

    Reference<String>* string(std::string value){
        return createString(value);
    }

    Reference<CodeFunction>* createFunction(Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines, bool reference = true);

    Reference<CodeFunction>* codefunction(Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines){
        return createFunction(parent_scope, parameters, lines);
    }

    void interpret(Scope *function_base_scope, std::vector<Line*> code);

    void interpret(Parser *parser);

    /**
     * Add the passed function to the root scope.
     */
    void addFunction(std::string name, size_t parameter_count,
        std::function<Reference<HeapObject>*(Env*, FunctionArguments)> implFunc);

};
