#include "classes.hpp"

#include "utils.hpp"
#include "env.hpp"
#include "baseobject.hpp"
#include "function_arguments.hpp"
#include "string.hpp"
#include "boolean.hpp"
#include "nothing.hpp"
#include "function.hpp"
#include "class.hpp"

#define CONTEXT_NAME(name) CONTEXT(name), #name

FunctionTuple t(ExceptionContext con, std::string name, size_t param_count,
                   std::function<rref(FunctionArguments&)> func){
    return FunctionTuple(con, name, param_count, func);
}

void initBaseClasses(Env *env){
    L
    env->createClass("BaseObject", {
                         t(CONTEXT_NAME(_create), 0, [env](FunctionArguments&){
                                        return make_rref<BaseObject>(env);
                                    })
                     }, {
                         t(CONTEXT_NAME(str), 0, [env](FunctionArguments &args){
                             return env->rstring(args.getThis()->str());
                         }),
                         t(CONTEXT_NAME(getMember), 1, [](FunctionArguments &args){
                             args.check({"String"});
                             return to_rref(args.getRThis()->as<Class>()->getMember(to_string(args.first())).copy());
                         }),
                         t(CONTEXT_NAME(equals), 1, [env](FunctionArguments &args){
                             return env->rboolean(args.getThis()->equals(args.first()));
                         }),
                         t(CONTEXT_NAME(str), 0, [env](FunctionArguments &args){
                             return env->rstring(args.getThis()->str());
                         }),
                         t(CONTEXT_NAME(_setDirect), 1, [env](FunctionArguments &args){
                             args.getThis()->setDirect(args.first()->value().copy());
                             return args.getRThis();
                         })
                     });
    L
    env->createClass("BaseClass", {

                     }, { });
    env->createClass("Nothing", {t(CONTEXT_NAME(_create), 0, [env](FunctionArguments&){
                                     return make_rref<Nothing>(env);
                                 })}, {});
    env->createClass("String", {t(CONTEXT_NAME(_create), 1, [env](FunctionArguments&){
                                     return make_rref<String>(env);
                                 })}, {});
    env->createClass("Boolean", {t(CONTEXT_NAME(_create), 0, [env](FunctionArguments&){
                                     return make_rref<Boolean>(env);
                                 })}, {});
}

void initClasses(Env *env){
    initBaseClasses(env);
}
