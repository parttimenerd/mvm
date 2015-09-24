#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "utils.hpp"
#include "reference.hpp"
#include "parser.hpp"
#include "function_arguments.hpp"

class Function;

class LineContext {
public:
    std::string _fileName;
    size_t lineNumber;
    size_t columnNumber;

    LineContext(std::string fileName, size_t lineNumber, size_t columnNumber)
        : _fileName(fileName), lineNumber(lineNumber), columnNumber(columnNumber) {}

    std::string str(){
        std::ostringstream stream;
        stream << _fileName << ":" << lineNumber << "." << columnNumber;
        return stream.str();
    }

    std::string fileName(){
        return _fileName;
    }
};

/** Use this to create a context object for CPPFunctions */
#define CONTEXT(name) ExceptionContext(LineContext(__FILE__, __LINE__, 0), #name)

class Env;
class Scope;

class ExceptionContext {
    LineContext _context;
    Optional<std::string> functionName;
    Optional<bref> this_obj;
    Optional<SmartReference<Function>> self;
    Optional<SmartReference<Scope>> function_parent_scope;

public:

    ExceptionContext(LineContext context);

    ExceptionContext(LineContext context, std::string functionName);

    ExceptionContext(LangContext context, std::string functionName);

    void setThis(bref this_obj){
        this->this_obj = Optional<bref>(this_obj.copy());
    }

    void setSelf(SmartReference<Function> new_self){
        this->self = new_self.copy();
    }

    void setFunctionParentScope(SmartReference<Scope> scope){
        this->function_parent_scope = scope.copy();
    }

    std::string str();

    LineContext context(){
        return _context;
    }
};

class Exception {

    std::string type;
    std::string message;
    std::vector<ExceptionContext> trace;
    Optional<rref> misc;

public:
    Exception(std::string type, std::string message,
              ExceptionContext origin, rref misc);

    Exception(std::string type, std::string message,
              ExceptionContext origin);

    void addContext(ExceptionContext context);

    std::string str();

};

#endif
