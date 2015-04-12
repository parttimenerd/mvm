#pragma once

#include "utils.hpp"

href_ptr __logic_chained(Env *env, FunctionArguments &args,
                         std::function<bool(HeapObject&, HeapObject&)> condition){
    auto first = args.first();
    for (size_t i = 1; i < args.all_arguments.size(); i++){
        if (!condition(*first->value, *args.at(i)->value)){
            return env->boolean(false);
        }
    }
    return env->boolean(true);
}


//function not_equal? | not_equal, 2 | var

href_ptr not_equal(Env *env, FunctionArguments &args){
    return __logic_chained(env, args, [](HeapObject &a, HeapObject &b){
        return a != b;
    });
}

//function equal? | equal, 2 | var

href_ptr equal(Env *env, FunctionArguments &args){
    return __logic_chained(env, args, [](HeapObject &a, HeapObject &b){
        return a == b;
    });
}

//function less? | less, 2 | var

href_ptr less(Env *env, FunctionArguments &args){
    return __logic_chained(env, args, [](HeapObject &a, HeapObject &b){
        return a < b;
    });
}

//function greater? | greater, 2 | var

href_ptr greater(Env *env, FunctionArguments &args){
    return __logic_chained(env, args, [](HeapObject &a, HeapObject &b){
        return a > b;
    });
}

//function less_equal? | less_equal, 2 | var

href_ptr less_equal(Env *env, FunctionArguments &args){
    return __logic_chained(env, args, [](HeapObject &a, HeapObject &b){
        return a <= b;
    });
}

//function greater_equal? | greater_equal, 2 | var

href_ptr greater_equal(Env *env, FunctionArguments &args){
    return __logic_chained(env, args, [](HeapObject &a, HeapObject &b){
        return a >= b;
    });
}
