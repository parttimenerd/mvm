#ifndef ENV_HPP
#define ENV_HPP

#include "utils.hpp"
#include "heap.hpp"
#include "stack.hpp"
#include "reference.hpp"
#include "exception.hpp"
#include <memory>

class Scope;
class Class;
class FunctionTuple;
class Nothing;

/**
 * @brief Environment that includes the heap, the stack and the root scope.
 */
class Env {
    std::unique_ptr<Heap> _heap = nullptr;
    std::unique_ptr<Stack> _stack = nullptr;
    Scope* _root_scope;
    Nothing* _nothing;

public:
    /**
     * @brief Create a new Environment.
     */
    Env();

    Heap* heap() { return &(*_heap); }

    Stack* stack() { return &(*_stack); }

    SmartReference<Scope> rootScope() { return SmartReference<Scope>(_root_scope); }

    rref rcodeFunction(ExceptionContext context, SmartReference<Scope> parent_scope,
                       std::vector<std::string> parameters, std::vector<Line*> lines){
        return to_rref(codeFunction(context, parent_scope, parameters, lines).copy());
    }

    bref codeFunction(ExceptionContext context, SmartReference<Scope> parent_scope,
                      std::vector<std::string> parameters, std::vector<Line*> lines);

    rref rcppFunction(ExceptionContext context, size_t parameter_count,
                   std::function<rref(FunctionArguments&)> implFunc, SmartReference<Scope> parent_scope);

    bref cppFunction(ExceptionContext context, SmartReference<Scope> parent_scope, size_t parameter_count,
                  std::function<rref(FunctionArguments&)> implFunc);

    void interpret(SmartReference<Scope> function_base_scope, std::vector<Line*> code);

    void interpret(Parser *parser);

    /**
     * Add the passed object to the root scope.
     */
    void addToRootScope(std::string name, rref obj);

    //Exception* exception(std::string type, std::string message,
      //                   LangContext context, rref misc = 0, std::string functionName = "");

    rref rnothing();

    bref nothing();

    rref rstring(std::string str);

    rref rboolean(bool boolean);

    bref createClass(std::string name, std::vector<FunctionTuple> class_funcs,
                     std::vector<FunctionTuple> member_funcs, std::string parent = "BaseClass");

    SmartReference<Class> getClass(std::string name);

    SmartReference<Class> getClass(std::string name, SmartReference<Scope> scope);
};

#endif
