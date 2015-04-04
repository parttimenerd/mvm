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
    Reference<HeapObject>* integer(int_type val){
        return (Reference<HeapObject>*)createInt(val);
    }

    Reference<Nothing>* createNothing(bool reference = true);

    Reference<HeapObject>* nothing(){
        return (Reference<HeapObject>*)createNothing();
    }

    Reference<Array>* createArray(std::vector<Reference<HeapObject>*> value = std::vector<Reference<HeapObject>*>(), bool reference = true);

    Reference<HeapObject>* array(std::vector<Reference<HeapObject>*> value = std::vector<Reference<HeapObject>*>()){
        return (Reference<HeapObject>*)createArray(value);
    }

    Reference<Map>* createMap(std::map<HeapObject*, Reference<HeapObject>*> value = std::map<HeapObject*, Reference<HeapObject>*>(), bool reference = true);

    Reference<HeapObject>* map(std::map<HeapObject*, Reference<HeapObject>*> value = std::map<HeapObject*, Reference<HeapObject>*>()){
        return (Reference<HeapObject>*)createMap(value);
    }

    Reference<Boolean>* createBoolean(bool isTrue, bool reference = true);

    Reference<HeapObject>* boolean(bool isTrue){
        return (Reference<HeapObject>*)createBoolean(isTrue);
    }

    Reference<String>* createString(std::string value, bool reference = true);

    Reference<HeapObject>* string(std::string value){
        return (Reference<HeapObject>*)createString(value);
    }

    Reference<CodeFunction>* createFunction(Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines, bool reference = true);

    Reference<HeapObject>* codefunction(Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines){
        return (Reference<HeapObject>*)createFunction(parent_scope, parameters, lines);
    }

    void interpret(Scope *function_base_scope, std::vector<Line*> code);

    void interpret(Parser *parser);

    /**
     * Add the passed function to the root scope.
     */
    void addFunction(std::string name, size_t parameter_count,
        std::function<Reference<HeapObject>*(Env*, FunctionArguments)> implFunc);

};
