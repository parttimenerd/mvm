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
            currentPos++;
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
            case LineType::PUSH_VAR:
                env->stack->push(scope->get(toArgLine<std::string>(line)->argument)->transfer());
                break;
            case LineType::CALL_N:
                call(toArgLine<size_t>(line)->argument);
                break;
            case LineType::CALL:
                call();
                break;
            case LineType::POP:
                env->stack->popAndDeref();
                break;
            case LineType::DUP:
                env->stack->dup();
                break;
            default:
                throw std::string("Unsupported command ") + line->typeString();
        }
        //std::cout << env->stack->str() << "+++#############" << line->str() << "\n";
        //std::cout << scope->str_large() << "\n";
        //std::cout << env->stack->str();
    }

    template<typename T>
    ArgumentedLine<T>* toArgLine(Line *line){
        return static_cast<ArgumentedLine<T>*>(line);
    }

    void call(size_t numberOfArguments = (size_t)-1){
        if (numberOfArguments == (size_t)-1){
            HeapObject* num = env->stack->pop();
            if (num->type == Type::INT){
                numberOfArguments = (size_t) static_cast<Int*>(num)->value;
            }
            num->dereference();
        }
        HeapObject *obj = env->stack->pop();
        if (obj->type != Type::FUNCTION){
            env->stack->push(obj->transfer());
            std::cerr << "No function\n";
        } else {
            std::vector<HeapObject*> args;
            while (args.size() < numberOfArguments){
                args.push_back(env->stack->pop()->transfer());
                //std::cout << "ARG " << args[args.size() - 1]->str() << "\n";
            }
            Function *func = static_cast<Function*>(obj);
            //std::cout << func->str();
            func->exec(args);
        }
    }

    Line* currentLine(){
        return code.at(currentPos);
    }
};
