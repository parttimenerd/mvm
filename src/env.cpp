#include "utils.hpp"

#include "env.hpp"
#include "classes.hpp"
#include "heap.hpp"
#include "scope.hpp"
#include "reference.hpp"
#include "interpreter.hpp"
#include "parser.hpp"
#include "string.hpp"
#include "function.hpp"
#include "nothing.hpp"
#include "exception.hpp"
#include "boolean.hpp"
#include "class.hpp"

Env::Env() {
    L
    _heap = std::make_unique<Heap>(this);
    L
    _stack = std::make_unique<Stack>();
    L
    _root_scope = new Scope(this);
    _root_scope->reference();
    L
    initClasses(this);
    L
    _nothing = new Nothing(this);
    L
    _nothing->reference();
    L

    L
}

bref Env::codeFunction(ExceptionContext context, SmartReference<Scope> parent_scope, std::vector<std::string> parameters, std::vector<Line *> lines){
    return make_bref<CodeFunction>(this, context, parent_scope, parameters, lines);
}

rref Env::rcppFunction(ExceptionContext context, size_t parameter_count, std::function<rref (FunctionArguments &)> implFunc, SmartReference<Scope> parent_scope){
    return to_rref(cppFunction(context, parent_scope, parameter_count, implFunc));
}

bref Env::cppFunction(ExceptionContext context, SmartReference<Scope> parent_scope, size_t parameter_count, std::function<rref (FunctionArguments &)> implFunc){
    return make_bref<CPPFunction>(this, context, parent_scope, parameter_count, implFunc);
}

/*Exception *Env::exception(rref type, rref message, LangContext context, rref misc, std::string functionName){
    if (type->value->is(Type::STRING) && message->value->is(Type::STRING)){
        return new Exception(type->as<String>()->value, message->as<String>()->value, ExceptionContext(context, functionName), misc);
    }
    return new Exception("wrong type", "wrong argument types for throw", ExceptionContext(context, functionName), misc);
}*/

void Env::interpret(SmartReference<Scope> function_base_scope, std::vector<Line*> code){
    Interpreter inter(this, function_base_scope, code);
    inter.interpret();
}

void Env::interpret(Parser *parser){
    interpret(rootScope(), parser->lines());
}

void Env::addToRootScope(std::string name, rref obj){
    rootScope()->set(name, obj);
}

rref Env::rnothing(){
    return to_rref(SmartReference<BaseObject>(_nothing)).copy();
}

bref Env::nothing(){
    return bref(_nothing);
}

rref Env::rstring(std::string str){
    return make_rref<String>(this, str);
}

rref Env::rboolean(bool boolean){
    return make_rref<Boolean>(this, boolean);
}

bref Env::createClass(std::string name, std::vector<FunctionTuple> class_funcs, std::vector<FunctionTuple> member_funcs, std::string parent){
    L
    SmartReference<Class> ref = make_sref<Class>(this, rootScope().copy(), name, std::map<std::string, bref>(), getClass(parent));
    L
    ref->setFunctionMembers(member_funcs);
    ref->setClassFunctions(class_funcs);
    addToRootScope(name, ref.toRRef());
    return ref.toRRef().getVal();
}

SmartReference<Class> Env::getClass(std::string name){
    return getClass(name, rootScope());
}

SmartReference<Class> Env::getClass(std::string name, SmartReference<Scope> scope){
    return scope->get(name)->as<Class>();
}
