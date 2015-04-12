#pragma once

#include "utils.hpp"

href_ptr __arithmetic_chained(Env *env, FunctionArguments &args,
                              std::function<int_type(int_type, int_type)> int_func,
                              std::function<float_type(float_type, float_type)> float_func){
    int_type ret_int = 0;
    float_type ret_float = 0;
    bool currently_float = true;
    if (args.first()->value->isInt()){
        ret_int = args.first()->as<Int>()->value;
        currently_float = false;
    } else {
        ret_float = args.first()->as<Float>()->value;
    }
    for (size_t i = 1; i < args.all_arguments.size(); i++){
        auto arg = args.at(i);
        if (arg->value->isFloat() || currently_float){
            float_type arg_float;
            if (arg->value->isFloat()){
                arg_float = arg->as<Float>()->value;
            } else {
                arg_float = (float_type)arg->as<Int>()->value;
            }
            if (!currently_float){
                ret_float = (float_type)ret_int;
                currently_float = true;
            }
            ret_float = float_func(ret_float, arg_float);
        } else {
            ret_int = int_func(ret_int, arg->as<Int>()->value);
        }
    }
    if (currently_float){
        return env->floating(ret_float);
    }
    return env->integer(ret_int);
}

href_ptr __arithmetic_set(Env *env, FunctionArguments &args, std::string function){
    auto res = env->call(function, {args.first(), args.second()});
    args.first()->set(res);
    return res;
}

//function plus, 2 | var | numeric

href_ptr plus(Env *env, FunctionArguments &args){
    return __arithmetic_chained(env, args, [](int_type a, int_type b){
        return a + b;
    }, [](float_type a, float_type b){
        return a + b;
    });
}

//function plus! | plus_set, 2 | numeric -> numeric

href_ptr plus_set(Env *env, FunctionArguments &args){
    return __arithmetic_set(env, args, "add");
}


//function minus, 2 | var | numeric

href_ptr minus(Env *env, FunctionArguments &args){
    return __arithmetic_chained(env, args, [](int_type a, int_type b){
        return a - b;
    }, [](float_type a, float_type b){
        return a - b;
    });
}

//function minus! | minus_set, 2 | numeric -> numeric

href_ptr minus_set(Env *env, FunctionArguments &args){
    return __arithmetic_set(env, args, "minus");
}

//function multiply, 2 | var | numeric

href_ptr multiply(Env *env, FunctionArguments &args){
    return __arithmetic_chained(env, args, [](int_type a, int_type b){
        return a * b;
    }, [](float_type a, float_type b){
        return a * b;
    });
}

//function multiply! | multiply_set, 2 | numeric -> numeric

href_ptr multiply_set(Env *env, FunctionArguments &args){
    return __arithmetic_set(env, args, "multiply");
}

//function divide, 2 | var | numeric

href_ptr divide(Env *env, FunctionArguments &args){
    return __arithmetic_chained(env, args, [](int_type a, int_type b){
        return a / b;
    }, [](float_type a, float_type b){
        return a / b;
    });
}

//function divide! | divide_set, 2 | numeric -> numeric

href_ptr divide_set(Env *env, FunctionArguments &args){
    return __arithmetic_set(env, args, "divide");
}

//function modulo, 2 | var | numeric

href_ptr modulo(Env *env, FunctionArguments &args){
    return __arithmetic_chained(env, args, [](int_type a, int_type b){
        return a % b;
    }, [](float_type a, float_type b){
        return (float_type)(((int_type)a) % ((int_type)b));
    });
}

//function modulo! | modulo_set, 2 | numeric -> numeric

href_ptr modulo_set(Env *env, FunctionArguments &args){
    return __arithmetic_set(env, args, "modulo");
}

//function pow, 2 | var | numeric

href_ptr pow(Env *env, FunctionArguments &args){
    return __arithmetic_chained(env, args, [](int_type a, int_type b){
        return pow(a, b);
    }, [](float_type a, float_type b){
        return pow(a, b);
    });
}

//function pow! | pow_set, 2 | numeric -> numeric

href_ptr pow_set(Env *env, FunctionArguments &args){
    return __arithmetic_set(env, args, "pow");
}
