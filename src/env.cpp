#include "utils.hpp"

#include "env.hpp"
#include "scope.hpp"
#include "reference.hpp"
#include "heapobjects.hpp"
#include "interpreter.hpp"
#include "parser.hpp"

Env::Env(){
    heap = new Heap();
    stack = new Stack();
    root_scope = new Scope(this);
    root_scope->reference();
    _nothing = new Nothing(this);
    _nothing->reference();
}

void Env::dereference(HeapObject *obj){
    heap->dereference(obj);
}

void Env::add(HeapObject *obj){
    heap->add(obj);
}

Reference<Int>* Env::createInt(int_type val, bool reference){
    Int *integer = new Int(this, val);
    return new Reference<Int>(this, integer, reference);
}

Reference<Nothing>* Env::createNothing(bool reference){
    return new Reference<Nothing>(this, _nothing, reference);
}

Reference<Array>* Env::createArray(std::vector<Reference<HeapObject>*> value, bool reference){
    Array* arr = new Array(this, value, reference);
    return new Reference<Array>(this, arr, reference);
}

Reference<Map>* Env::createMap(std::map<HeapObject*, Reference<HeapObject>*> value, bool reference){
    Map* map = new Map(this, value, reference);
    return new Reference<Map>(this, map, reference);
}

Reference<Boolean>* Env::createBoolean(bool isTrue, bool reference){
    Boolean* boolean = new Boolean(this, isTrue);
    return new Reference<Boolean>(this, boolean, reference);
}

Reference<String>* Env::createString(std::string value, bool reference){
    String* str = new String(this, value);
    return new Reference<String>(this, str, reference);
}

Reference<CodeFunction>* Env::createFunction(ExceptionContext context, Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines, bool reference){
    CodeFunction *func = new CodeFunction(this, context, parent_scope, parameters, lines);
    return new Reference<CodeFunction>(this, func, reference);
}

void Env::addFunction(ExceptionContext context, std::string name, size_t parameter_count, std::function<Reference<HeapObject>*(Env*, FunctionArguments)> implFunc, Scope *parent_scope){
    if (parent_scope == 0){
        parent_scope = root_scope;
    }
    auto *obj = new CPPFunction(this, context, parent_scope, parameter_count, implFunc);
    root_scope->set(name, obj);
}

Exception *Env::exception(Reference<HeapObject> *type, Reference<HeapObject> *message, LangContext context, Reference<HeapObject> *misc, std::string functionName){
    if (type->value->is(Type::STRING) && message->value->is(Type::STRING)){
        return new Exception(type->as<String>()->value, message->as<String>()->value, ExceptionContext(context, functionName), misc);
    }
    return new Exception("wrong type", "wrong argument types for throw", ExceptionContext(context, functionName), misc);
}

void Env::interpret(Scope *function_base_scope, std::vector<Line*> code){
    Interpreter inter(this, function_base_scope, code);
    inter.interpret();
}

void Env::interpret(Parser *parser){
    interpret(root_scope, parser->lines());
}
