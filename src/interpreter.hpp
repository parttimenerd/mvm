#pragma once

#include "utils.hpp"
#include "env.hpp"
#include "parser.hpp"
#include "reference.hpp"
#include "smartreference.hpp"

class Scope;

class Interpreter {

    Env *env;
    SmartReference<Scope> scope;
    SmartReference<Scope> baseScope;
    std::vector<Line*> code;
    size_t currentPos = 0;

public:

    Interpreter(Env *env, SmartReference<Scope> currentScope, std::vector<Line*> code);

    void interpret();

    void interpret(Line* line);

protected:

    template<typename T>
    ArgumentedLine<T>* toArgLine(Line *line){
        return (ArgumentedLine<T>*)line;
    }

    /**
     * @brief Call with default number of arguments
     */
    void callDefault();

    void call(size_t numberOfArguments = 99999999999);

    void pushScope();

    void popScope();

    rref functionHeader(FunctionHeaderLine *line);


    Line* currentLine();

    void error(std::string msg, std::string msg1 = "");
};
