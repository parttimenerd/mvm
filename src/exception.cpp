#include "exception.hpp"
#include "env.hpp"
#include "reference.hpp"
#include "utils.hpp"
#include "scope.hpp"

ExceptionContext::ExceptionContext(LineContext context)
    : _context(context) {}

ExceptionContext::ExceptionContext(LineContext context, std::string functionName)
    : _context(context){
    L
    if (functionName != ""){
        this->functionName = Optional<std::string>(functionName);
    }
    L
}

ExceptionContext::ExceptionContext(LangContext context, std::string functionName)
    : ExceptionContext(LineContext(context.fileName(), context.lineNumber, context.columnNumber), functionName)
{}

std::string ExceptionContext::str(){
    std::ostringstream stream;
    if (functionName.has()){
        stream << functionName.value() << "|";
    }
    stream << _context.str();
    return stream.str();
}

Exception::Exception(std::string type, std::string message,
                     ExceptionContext origin)
    : type(type), message(message) {
    trace.push_back(origin);
}

Exception::Exception(std::string type, std::string message,
                     ExceptionContext origin, rref misc)
    : Exception(type, message, origin){
    this->misc = Optional<rref>(misc);
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
