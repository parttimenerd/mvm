#ifndef ENV_HPP
#define ENV_HPP

#include "utils.hpp"
#include "heap.hpp"
#include "stack.hpp"
#include "reference.hpp"
#include "exception.hpp"
//#include "scope.hpp"

struct CodeFunction;
struct HeapObject;
struct Scope;
struct Int;
struct Float;
struct Nothing;
struct Array;
struct Map;
struct Boolean;
struct Function;
struct String;
struct Line;
struct Parser;
struct FunctionArguments;

/** e.g. GFUNCTION("func", 1, (){…}) */
#define GFUNCTION(name, parameter_count, implFunction) env->addFunction(CONTEXT(name), name, parameter_count, implFunction);

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

    SmartReference<HeapObject> sinteger(int_type value){
        return make_sref(integer(value));
    }

    Reference<Float>* createFloat(float_type val, bool reference = true);
    Reference<HeapObject>* floating(float_type val){
        return (Reference<HeapObject>*)createFloat(val);
    }

    SmartReference<HeapObject> sfloating(float_type value){
        return make_sref(floating(value));
    }

    Reference<Nothing>* createNothing(bool reference = true);

    Reference<HeapObject>* nothing(){
        return (Reference<HeapObject>*)createNothing();
    }

    SmartReference<HeapObject> snothing(){
        return make_sref(nothing());
    }

    Reference<Array>* createArray(std::vector<Reference<HeapObject>*> value = std::vector<Reference<HeapObject>*>(), bool reference = true);

    Reference<HeapObject>* array(std::vector<Reference<HeapObject>*> value = std::vector<Reference<HeapObject>*>()){
        return (Reference<HeapObject>*)createArray(value);
    }

    SmartReference<HeapObject> sarray(std::vector<Reference<HeapObject>*> value = std::vector<Reference<HeapObject>*>()){
        return make_sref(array(value));
    }

    Reference<Map>* createMap(std::map<HeapObject*, Reference<HeapObject>*> value = std::map<HeapObject*, Reference<HeapObject>*>(), bool reference = true);

    Reference<HeapObject>* map(std::map<HeapObject*, Reference<HeapObject>*> value = std::map<HeapObject*, Reference<HeapObject>*>()){
        return (Reference<HeapObject>*)createMap(value);
    }

    SmartReference<HeapObject> smap(std::map<HeapObject*, Reference<HeapObject>*> value = std::map<HeapObject*, Reference<HeapObject>*>()){
        return make_sref(map(value));
    }

    Reference<Boolean>* createBoolean(bool isTrue, bool reference = true);

    Reference<HeapObject>* boolean(bool isTrue){
        return (Reference<HeapObject>*)createBoolean(isTrue);
    }

    SmartReference<HeapObject> sboolean(bool isTrue){
        return make_sref(boolean(isTrue));
    }

    Reference<String>* createString(std::string value, bool reference = true);

    Reference<HeapObject>* string(std::string value){
        return (Reference<HeapObject>*)createString(value);
    }

    SmartReference<HeapObject> sstring(std::string value){
        return make_sref(string(value));
    }

    Reference<CodeFunction>* createFunction(ExceptionContext context, Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines, bool reference = true);

    Reference<HeapObject>* codefunction(ExceptionContext context, Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines){
        return (Reference<HeapObject>*)createFunction(context, parent_scope, parameters, lines);
    }

    SmartReference<HeapObject> scodefunction(ExceptionContext context, Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines){
        return make_sref(codefunction(context, parent_scope, parameters, lines));
    }

    void interpret(Scope *function_base_scope, std::vector<Line*> code);

    void interpret(Parser *parser);

    Reference<HeapObject> * call(std::string functionName, std::vector<Reference<HeapObject>*> args, Scope *scope = 0);

    /**
     * Add the passed function to the root scope.
     */
    void addFunction(ExceptionContext context, std::string name, size_t parameter_count,
        std::function<Reference<HeapObject>*(Env*, FunctionArguments&)> implFunc, Scope *parent_scope = 0);

    Exception* exception(std::string type, std::string message,
                         LangContext context, Reference<HeapObject>* misc = 0, std::string functionName = ""){
        return new Exception(type, message, ExceptionContext(context, functionName), misc);
    }

    Exception* exception(Reference<HeapObject>* type, Reference<HeapObject>* message,
                        LangContext context, Reference<HeapObject>* misc = 0, std::string functionName = "");
};

#endif
