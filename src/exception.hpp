#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "utils.hpp"
#include "reference.hpp"
#include "parser.hpp"

/** Use this to create a context object for CPPFunctions */
#define CONTEXT(name) ExceptionContext(LangContext(__FILE__, __LINE__, 0), #name)

struct Env;

struct ExceptionContext {
    LangContext context;
    std::string functionName;

    ExceptionContext(LangContext context, std::string functionName = "");

    std::string str();

    Reference<HeapObject>* toHeapObject(Env *env);

    bool hasFunctionName(){
        return !functionName.empty();
    }
};

struct Exception {

    std::string type;
    std::string message;
    std::vector<ExceptionContext> trace;
    Reference<HeapObject>* misc;

    Exception(std::string type, std::string message,
              ExceptionContext origin, Reference<HeapObject>* misc = 0);

    void addContext(ExceptionContext context);

    std::string str();

    Reference<HeapObject>* toHeapObject(Env *env);
};

#endif
