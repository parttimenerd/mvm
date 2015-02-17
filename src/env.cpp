#include "utils.hpp"

#include "env.hpp"
#include "scope.hpp"
#include "heapobjects.hpp"
#include "interpreter.hpp"
#include "parser.hpp"

Env::Env(){
    heap = new Heap();
    stack = new Stack();
    root_scope = new Scope(this);
    root_scope->reference();
    nothing = new Nothing(this);
    nothing->reference();
}

void Env::dereference(HeapObject *obj){
    heap->dereference(obj);
}

void Env::add(HeapObject *obj){
    heap->add(obj);
}

Int* Env::createInt(int_type val, bool reference){
    Int *integer = new Int(this, val);
    if (reference) {
        integer->reference();
    }
    return integer;
}

Nothing* Env::createNothing(bool reference){
    if (reference) {
        nothing->reference();
    }
    return nothing;
}

Array* Env::createArray(std::vector<HeapObject*> value, bool reference){
    Array* arr = new Array(this, value);
    if (reference) {
        arr->reference();
    }
    return arr;
}

Map* Env::createMap(bool reference){
    Map* map = new Map(this);
    if (reference) {
        map->reference();
    }
    return map;
}

Boolean* Env::createBoolean(bool isTrue, bool reference){
    Boolean* boolean = new Boolean(this, isTrue);
    if (reference) {
        boolean->reference();
    }
    return boolean;
}

String* Env::createString(std::string value, bool reference){
    String* str = new String(this, value);
    if (reference) {
        str->reference();
    }
    return str;
}

CodeFunction* Env::createFunction(Scope *parent_scope, std::vector<std::string> parameters, std::vector<Line*> lines, bool reference){
    CodeFunction *func = new CodeFunction(this, parent_scope, parameters, lines);
    if (reference){
        func->reference();
    }
    return func;
}

void Env::addFunction(std::string name, size_t parameter_count, std::function<HeapObject*(Env *env, std::vector<HeapObject*> arguments, std::vector<HeapObject*> miscArguments)> implFunc){
    auto *obj = new CPPFunction(this, parameter_count, implFunc);
    obj->name = name;
    root_scope->setHere(name, obj);
}

void Env::interpret(Scope *function_base_scope, std::vector<Line*> code){
    Interpreter inter(this, function_base_scope, code);
    inter.interpret();
}

void Env::interpret(Parser *parser){
    interpret(root_scope, parser->lines());
}
