#include "stdlib.hpp"
#include "utils.hpp"
#include "heapobjects.hpp"
#include "env.hpp"

/**
 * This file contains the standard library of the vm.
 */


void loadIOLib(Env &env){
    auto print_env = [](Env* env, FunctionArguments){
        std::cout << env->heap->str();
        std::cout << env->stack->str();
        return env->createNothing()->transfer();
    };
    env.addFunction("print_env", 0, print_env);
    auto print = [](Env* env, FunctionArguments args){
        for (size_t i = 0; i < args.all_arguments.size(); i++){
            std::cout << args.all_arguments[i]->str();
        }
        return env->createNothing()->transfer();
    };
    env.addFunction("print", 0, print);
    auto println = [](Env* env, FunctionArguments args){
        for (size_t i = 0; i < args.all_arguments.size(); i++){
            std::cout << args.all_arguments[i]->str();
        }
        std::cout << "\n";
        return env->createNothing()->transfer();
    };
    env.addFunction("println", 0, println);
}

void loadMathLib(Env &env){
    auto binary_math_op = [](std::function<int_type(int_type, int_type)> math_op, int_type neutral_elem, Env* env, FunctionArguments args){
        int_type res = neutral_elem;
        for (size_t i = 0; i < args.all_arguments.size(); i++){
            if (args.all_arguments[i]->type == Type::INT){
                res = math_op(res, static_cast<Int*>(args.all_arguments[i])->value);
            }
        }
        return env->createInt(res)->transfer();
    };
    auto add = [binary_math_op](Env* env, FunctionArguments args){
        return binary_math_op([](int_type x, int_type y){ return x + y; }, 0, env, args);
    };
    env.addFunction("add", 2, add);
    auto mul = [binary_math_op](Env* env, FunctionArguments args){
        return binary_math_op([](int_type x, int_type y){ return x * y; }, 1, env, args);
    };
    env.addFunction("mul", 2, mul);
    auto sub = [binary_math_op](Env* env, FunctionArguments args){
        return binary_math_op([](int_type x, int_type y){ return x - y; }, 0, env, args);
    };
    env.addFunction("sub", 2, sub);
    auto div = [binary_math_op](Env* env, FunctionArguments args){
        return binary_math_op([](int_type x, int_type y){ return x / y; }, 1, env, args);
    };
    env.addFunction("div", 2, div);
}

void loadLogicalLib(Env &env){
    auto equal = [](Env* env, FunctionArguments args){
        bool res = true;
        HeapObject *first = args.all_arguments[0];
        for (auto *elem : args.all_arguments){
            res = res && (*first) == (*elem);
            if (!res){
                break;
            }
        }
        return env->createBoolean(res);
    };
    env.addFunction("equal", 2, equal);
}

void loadStdLib(Env &env){
    loadMathLib(env);
    loadIOLib(env);
    loadLogicalLib(env);
}
