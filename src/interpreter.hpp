#include "utils.hpp"
#include "env.hpp"
#include "heapobjects.hpp"
#include "parser.hpp"

struct Interpreter {
    Env *env;
    Scope *currentScope;
    std::vector<Line*> code;
    size_t currentPos = 0;

    Interpreter(Env *env, Scope *currentScope, std::vector<Line*> code){
        this->env = env;
        this->scope = scope;
        this->code = code;
    }

    void interpret(){
        while (currentPos >= 0 && currentPos < code.size() || && currentLine()->type == LineType::RETURN){
            interpret(currentLine());
        }
    }

    void interpret(Line* line){
        switch (line->type){
            case LineType::PUSH_STRING:
                stack->push(env->createString());
            default:
                throw std::string("Unsupported command ") + lineTypeToString(type);
        }
    }

    Line* currentLine(){
        return code.at(currentPos);
    }
};
