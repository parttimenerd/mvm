#include "interpreter.hpp"
#include "utils.hpp"
#include "env.hpp"
#include "parser.hpp"
#include "reference.hpp"
#include "scope.hpp"
#include "baseobject.hpp"
#include "boolean.hpp"
#include "function.hpp"

Interpreter::Interpreter(Env *env, SmartReference<Scope> currentScope, std::vector<Line*> code)
    : env(env), scope(currentScope), baseScope(currentScope.copy()), code(code) {}

void Interpreter::interpret(){
    while (currentPos < code.size() && currentLine()->type != LineType::RETURN){
        interpret(currentLine());
        currentPos++;
    }
}

void Interpreter::interpret(Line* line){
    try {
        //std::cout << currentPos << " " << baseScope->get("abc")->str() << "\n";
        switch (line->type){
        case LineType::PUSH_STRING:
            env->stack()->push(env->rstring(toArgLine<std::string>(line)->argument).copy());
            break;
        case LineType::PUSH_NOTHING:
            env->stack()->push(env->rnothing().copy());
            break;
        case LineType::PUSH_BOOLEAN:
            env->stack()->push(env->rboolean(toArgLine<bool>(line)->argument).copy());
            break;
        case LineType::PUSH_INT:
        {
            //int_type value = toArgLine<int_type>(line)->argument;
            //env->stack()->push(env->rinteger(value).copy());
        }
            break;
        case LineType::PUSH_FLOAT:
        {
            //float_type value = toArgLine<float_type>(line)->argument;
            //env->stack()->push(env->rfloating(value).copy());
        }
            break;
        case LineType::PUSH_ARRAY:
            //env->stack()->push(env->rarray().copy());
            break;
        case LineType::PUSH_MAP:
            //env->stack()->push(env->rmap().copy());
            break;
        case LineType::PUSH_VAR:
        {
            //std::cerr << scope->str_large();
            auto var = scope->get(static_cast<ArgumentedLine<std::string>*>(line)->argument);
            env->stack()->push(var.copy());
            //r << var->str() << "\n";
        }
            break;
        case LineType::SET_VAR:
        {
            auto obj = env->stack()->pop();
            scope->set(static_cast<ArgumentedLine<std::string>*>(line)->argument, obj.copy());
            env->stack()->push(obj);
        }
            break;
        case LineType::SET_OBJ:
        {
            auto new_val = env->stack()->pop();
            auto obj = env->stack()->pop();
            obj->value()->set(static_cast<ArgumentedLine<std::string>*>(line)->argument, new_val.copy());
            env->stack()->push(new_val.copy());
        }
        case LineType::GET_OBJ:
        {
            //std::cerr << scope->str_large();
            auto obj = env->stack()->pop();
            auto var = obj->value()->get(static_cast<ArgumentedLine<std::string>*>(line)->argument);
            env->stack()->push(var.copy());
            //r << var->str() << "\n";
        }
            break;
        case LineType::INIT_VAR:
        {
            auto varName = toArgLine<std::string>(line)->argument;
            scope->initVar(varName);
            env->stack()->push(env->rnothing().copy());
        }
            break;
        case LineType::CALL_N:
            call(toArgLine<size_t>(line)->argument);
            break;
        case LineType::CALL:
            callDefault();
            break;
        case LineType::JUMP:
            currentPos = toArgLine<size_t>(line)->argument - 1;
            break;
        case LineType::JUMP_IF:
        case LineType::JUMP_IF_NOT:
        {
            rref obj = env->stack()->pop();
            if (obj->value()->is("Boolean")){
                if ((LineType::JUMP_IF == line->type) ==  obj->as<Boolean>()->value()){
                    currentPos = toArgLine<size_t>(line)->argument - 1;
                }
            } else {
                throw std::string("Can't use non booleans as conditional value");
            }
        }
            break;
        case LineType::POP:
            env->stack()->pop();
            break;
        case LineType::DUP:
            env->stack()->dup();
            break;
        case LineType::PUSH_SCOPE:
            pushScope();
            break;
        case LineType::POP_SCOPE:
            popScope();
            break;
        case LineType::FUNCTION_HEADER:
            env->stack()->push(functionHeader((FunctionHeaderLine*)line));
        case LineType::PRINT_STACK:
            std::cout << env->stack()->str();
            break;
        case LineType::COMMENT:
        case LineType::NOP:
            break;
        default:
            throw std::string("Unsupported command ") + line->typeString();
        }
    } catch (std::string msg){
        error(std::string(msg));
        exit(1);
    }


    //std::cout << env->stack()->str() << "+++#############" << line->str() << "\n";
    //std::cout << scope->str_large() << "\n";
    //std::cout << env->stack()->str();
}

void Interpreter::callDefault(){
    rref obj = env->stack()->pop();
    SmartReference<Function> func = obj->as<Function>();
    std::vector<rref> args;
    while (args.size() < func->parameterCount()){
        rref argObj = env->stack()->pop();
        args.push_back(argObj.copy());
    }
    func->exec(args);
}

void Interpreter::call(size_t numberOfArguments){
    rref obj = env->stack()->pop();
    if (!obj->value()->is("Function")){
        env->stack()->push(obj.copy());
        std::cerr << "No function\n";
    } else {
        std::vector<rref> args;
        while (args.size() < numberOfArguments){
            rref argObj = env->stack()->pop();
            //std::cerr << "######\n" << env->stack()->str() ;
            //std::cerr << "a   " << argObj->reference_count << "\n";
            args.push_back(argObj.copy());
            //std::cout << "ARG " << args[args.size() - 1]->str() << "\n";
        }
        //std::cout << func->str();
        obj->as<Function>()->exec(args);
        //std::cout << env->stack()->str();
    }
}

void Interpreter::pushScope(){
    scope = scope->createChild();
}

void Interpreter::popScope(){
    if (scope != baseScope){
        scope = scope->parent();
    } else {
        throw std::string("Cannot pop base scope");
    }
}

rref Interpreter::functionHeader(FunctionHeaderLine *line){
    std::vector<Line*> body;
    size_t openHeaderCount = 1;
    currentPos++;
    while (currentPos < code.size() && openHeaderCount != 0){
        switch(currentLine()->type){
        case LineType::FUNCTION_HEADER:
        case LineType::CLOSURE_HEADER:
            openHeaderCount++;
            break;
        case LineType::FUNCTION_END:
            openHeaderCount--;
            break;
        default:
            break;
        }
        body.push_back(currentLine());
        currentPos++;
    }
    return env->rcodeFunction(ExceptionContext(line->context, line->name), scope, line->arguments, body);
}

Line* Interpreter::currentLine(){
    return code.at(currentPos);
}

void Interpreter::error(std::string msg, std::string msg1){
    std::ostringstream stream;
    stream << "Error in line " << currentPos
           << "[" << currentLine()->context.str()
           << "]: " << msg << msg1;
    std::cerr << stream.str() << "\n";
    throw stream.str();
}
