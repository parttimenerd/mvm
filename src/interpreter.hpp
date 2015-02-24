#include "utils.hpp"
#include "env.hpp"
#include "heapobjects.hpp"
#include "parser.hpp"

struct Interpreter {
    Env *env;
    Scope *scope;
    Scope *baseScope;
    std::vector<Line*> code;
    size_t currentPos = 0;

    Interpreter(Env *env, Scope *currentScope, std::vector<Line*> code){
        this->env = env;
        this->scope = currentScope;
        this->baseScope = currentScope;
        this->code = code;
    }

    void interpret(){
        while (currentPos < code.size() && currentLine()->type != LineType::RETURN){
            interpret(currentLine());
            currentPos++;
        }
    }

    void interpret(Line* line){
        try {
            //std::cout << currentPos << " " << baseScope->get("abc")->str() << "\n";
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
                {
                    int_type value = toArgLine<int_type>(line)->argument;
                    env->stack->push(env->createInt(value)->transfer());
                }
                break;
            case LineType::PUSH_ARRAY:
                env->stack->push(env->createArray()->transfer());
                break;
            case LineType::PUSH_MAP:
                env->stack->push(env->createMap()->transfer());
                break;
            case LineType::PUSH_VAR:
                env->stack->push(scope->get(toArgLine<std::string>(line)->argument));
                break;
            case LineType::SET_VAR_HERE:
                scope->setHere(toArgLine<std::string>(line)->argument, env->stack->pop());
                break;
            case LineType::SET_VAR_HERE2:
                {
                    auto *obj = env->stack->pop();
                    if (obj->type == Type::STRING){
                        scope->setHere(static_cast<String*>(obj)->value, env->stack->pop()->transfer());
                    } else {
                        error("Expected string on stack for SET_VAR2 command");
                    }
                    obj->dereference();
                }
                break;
            case LineType::SET_VAR:
                scope->set(toArgLine<std::string>(line)->argument, env->stack->pop());
                break;
            case LineType::SET_VAR2:
                {
                    auto *obj = env->stack->pop();
                    if (obj->type == Type::STRING){
                        scope->set(static_cast<String*>(obj)->value, env->stack->pop()->transfer());
                    } else {
                        error("Expected string on stack for SET_VAR2 command");
                    }
                    obj->dereference();
                }
                break;
            case LineType::CALL_N:
                call(toArgLine<size_t>(line)->argument);
                break;
            case LineType::CALL_N2:
                call();
                break;
            case LineType::CALL:
                callDefault();
                break;
            case LineType::JUMP_IF:
                {
                    HeapObject *obj = env->stack->pop();
                    if (obj->type == Type::BOOLEAN && static_cast<Boolean*>(obj)->isTrue){
                        //std::cout << currentPos << "\n";
                        currentPos = toArgLine<size_t>(line)->argument - 1;
                        //std::cout << currentPos << "\n";
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
            case LineType::PUSH_SCOPE:
                pushScope();
                break;
            case LineType::POP_SCOPE:
                popScope();
                break;
            case LineType::PRINT_STACK:
                std::cout << env->stack->str();
                break;
            case LineType::ASSERT_STACK_HEIGHT:
                {
                    size_t expectedHeight = toArgLine<size_t>(line)->argument;
                    if (env->stack->size() != expectedHeight){
                        std::ostringstream stream;
                        stream << "Expected stack of height " << expectedHeight
                            << " not " << env->stack->size();
                        throw stream.str();
                    }
                }
                break;
            case LineType::COMMENT:
                break;
            default:
                throw std::string("Unsupported command ") + line->typeString();
        }
        } catch (std::string msg){
            error(std::string(msg));
            exit(1);
        }


        //std::cout << env->stack->str() << "+++#############" << line->str() << "\n";
        //std::cout << scope->str_large() << "\n";
        //std::cout << env->stack->str();
    }

    template<typename T>
    ArgumentedLine<T>* toArgLine(Line *line){
        return static_cast<ArgumentedLine<T>*>(line);
    }

    /**
     * @brief call with default number of arguments
     */
    void callDefault(){
        HeapObject *obj = env->stack->pop();
        Function *func = static_cast<Function*>(obj);
        std::vector<HeapObject*> args;
        while (args.size() < func->parameter_count){
            HeapObject *argObj = env->stack->pop();
            args.push_back(argObj);
        }
        func->exec(args);
    }

    void call(size_t numberOfArguments = 99999999999){
        //std::cerr << currentPos << "aa\n";
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
                HeapObject *argObj = env->stack->pop();
                //std::cerr << "######\n" << env->stack->str() ;
                //std::cerr << "a   " << argObj->reference_count << "\n";
                args.push_back(argObj);
                //std::cout << "ARG " << args[args.size() - 1]->str() << "\n";
            }
            Function *func = static_cast<Function*>(obj);
            //std::cout << func->str();
            func->exec(args);
        }
    }

    void pushScope(){
        scope = scope->createChild();
    }

    void popScope(){
        if (scope != baseScope){
            auto *parent = scope->parent;
            parent->reference();
            scope->dereference();
            scope = parent;
        } else {
            throw std::string("Cannot pop base scope");
        }
    }

    Line* currentLine(){
        return code.at(currentPos);
    }

    void error(std::string msg, std::string msg1 = ""){
        std::ostringstream stream;
        stream << "Error in line " << currentPos << ": " << msg << msg1;
        //std::cerr << stream.str() << "\n";
        throw stream.str();
    }
};
