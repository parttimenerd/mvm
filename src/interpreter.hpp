#include "utils.hpp"
#include "env.hpp"
#include "heapobjects.hpp"
#include "parser.hpp"

struct Interpreter {
    Env *env;
    Scope *scope;
    std::vector<Line*> code;
    size_t currentPos = 0;

    Interpreter(Env *env, Scope *currentScope, std::vector<Line*> code){
        this->env = env;
        this->scope = currentScope;
        this->code = code;
    }

    void interpret(){
        while (currentPos < code.size() && currentLine()->type != LineType::RETURN){
            interpret(currentLine());
        }
    }

    void interpret(Line* line){
        switch (line->type){
            case LineType::PUSH_STRING:
                env->stack->push(env->createString(toArgLine<std::string>(line)->argument)->transfer());
                break;
            case LineType::PUSH_INT:
                env->stack->push(env->createInt(toArgLine<int_type>(line)->argument)->transfer());
                break;
            case LineType::CALL_N:
                call(toArgLine<size_t>(line)->argument);
                break;
            case LineType::CALL:
                call();
                break;
            case LineType::DUP:
                env->stack->dup();
                break;
            default:
                throw std::string("Unsupported command ") + line->typeString();
        }
    }

    template<typename T>
    ArgumentedLine<T>* toArgLine(Line *line){
        return static_cast<ArgumentedLine<T>*>(line);
    }

    void call(){
        HeapObject* num = env->stack->pop();
        size_t arg_num = 0;
        if (num->type == Type::INT){
            arg_num = (size_t) static_cast<Int*>(num)->value;
        }
        num->dereference();
        call(arg_num);
    }

    void call(size_t numberOfArguments){
        std::vector<HeapObject*> args;
        while (args.size() < numberOfArguments){
            args.push_back(env->stack->pop());
        }
        HeapObject *obj = env->stack->pop();
        if (obj->type != Type::FUNCTION){
            for (size_t i = 0; i < args.size(); i++){
                args[i]->dereference();
            }
            env->stack->push(obj->transfer());
        } else {
            for (size_t i = 0; i < args.size(); i++){
                args[i]->transfer();
            }
            Function *func = static_cast<Function*>(obj);
            func->exec(args);
        }
    }

    Line* currentLine(){
        return code.at(currentPos);
    }
};
