#include "exception.hpp"
#include "parser.hpp"
#include "heapobjects.hpp"
#include "env.hpp"
#include "reference.hpp"

ExceptionContext::ExceptionContext(LangContext context, std::string functionName)
    : context(context), functionName(functionName) {}

std::string ExceptionContext::str(){
    std::ostringstream stream;
    if (hasFunctionName()){
        stream << functionName << "|";
    }
    stream << context.str();
    return stream.str();
}

Reference<HeapObject> *ExceptionContext::toHeapObject(Env *env){
    std::map<HeapObject*, Reference<HeapObject>*> obj;
    obj[env->sstring("line").value]
            = env->sinteger((int_type)context.lineNumber).reference;
    obj[env->sstring("column").value]
            = env->sinteger((int_type)context.columnNumber).reference;
    obj[env->sstring("functionName").value]
            = env->sstring(context.fileName()).reference;
    if (hasFunctionName()){
        obj[env->sstring("functionName").value]
                = env->sstring(functionName).reference;
    }
    return env->map(obj);
}

Exception::Exception(std::string type, std::string message,
                     ExceptionContext origin, Reference<HeapObject>* misc)
    : type(type), message(message), misc(misc){
    trace.push_back(origin);
    if (misc != 0){
        misc->reference();
    }
}

void Exception::addContext(ExceptionContext context){
    trace.push_back(context);
}

std::string Exception::str(){
    std::ostringstream stream;
    stream << type << " exception: " << message;
    for (auto item : trace){
        stream << "\n\t at " + item.str();
    }
    return stream.str();
}

Reference<HeapObject> *Exception::toHeapObject(Env *env){
    std::map<HeapObject*, Reference<HeapObject>*> obj;
    obj[env->sstring("type").value]
            = env->sstring(type).reference;
    obj[env->sstring("message").value]
            = env->sstring(message).reference;
    if (misc != 0){
        obj[env->sstring("misc").value] = misc;
    }
    std::vector<Reference<HeapObject>*> traceObjs;
    for (auto &item : trace){
        traceObjs.push_back(item.toHeapObject(env));
    }
    obj[env->sstring("trace").value] = env->sarray(traceObjs).reference;
    return env->map(obj);
}
