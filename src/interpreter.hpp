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
            case LineType::PUSH_NOTHING:
                env->stack->push(env->createNothing()->transfer());
                break;
            case LineType::PUSH_BOOLEAN:
                env->stack->push(env->createBoolean(toArgLine<bool>(line)->argument)->transfer());
                break;
            case LineType::PUSH_INT:
                env->stack->push(env->createInt(toArgLine<int_type>(line)->argument)->transfer());
                break;
            case LineType::PUSH_ARRAY:
                env->stack->push(env->createArray()->transfer());
                break;
            case LineType::PUSH_MAP:
                env->stack->push(env->createMap()->transfer());
            case LineType::PUSH_VAR:
                env->stack->push(scope->get(toArgLine<std::string>(line)->argument)->transfer());
                break;
            case LineType::SET_VAR:
                scope->set(toArgLine<std::string>(line)->argument, env->stack->pop()->transfer());
                break;
            case LineType::SET_VAR2:
                {
                    auto *obj = env->stack->pop();
                    if (obj->type == Type::STRING){
                        scope->set(static_cast<String*>(obj)->value, env->stack->pop()->transfer());
                    } else {
                        throw std::string("Expected string on stack for SET_VAR2 command");
                    }
                    obj->dereference();
                }
                break;
            case LineType::CALL_N:
                call(toArgLine<size_t>(line)->argument);
                break;
            case LineType::CALL:
                call();
                break;
            case LineType::JUMP_IF:
                {
                    HeapObject *obj = env->stack->pop();
                    if (obj->type == Type::BOOLEAN && static_cast<Boolean*>(obj)->isTrue){
                        currentPos += toArgLine<int_type>(line)->argument;
                    }
                    obj->dereference();
                }
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

    void call(size_t numberOfArguments = 99999999999){
        if (numberOfArguments == 99999999999){
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
