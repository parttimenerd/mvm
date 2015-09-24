#include "heapobject.hpp"
#include "class.hpp"
#include "env.hpp"
#include "exception.hpp"
#include "function_arguments.hpp"
#include "classes.hpp"
#include "string.hpp"
#include "function.hpp"

Class::Class(Env *env, SmartReference<Scope> parent_scope, std::string name)
    : BaseObject(env), name(name),
      parent_scope(parent_scope) {
    type = Type::CLASS;
    setFunctionMembers({
                           FunctionTuple(CONTEXT(create), "create", 0, [this](FunctionArguments &args) -> rref {
                               auto obj = call("_create", args);
                               copyInstanceMembers(obj);
                               obj.getVal()->maybeCall("init", args);
                               return obj;
                           }),
                           FunctionTuple(CONTEXT(getMember), "getMember", 1, [this](FunctionArguments &args) -> rref {
                               args.check({"String"});
                               std::string str = to_string(args[0]);
                               return to_rref(getMember(str));
                           }),
                           FunctionTuple(CONTEXT(setMember), "setMember", 2, [this](FunctionArguments &args) -> rref {
                               args.check({"String", "*"});
                               std::string str = to_string(args[0]);
                               return to_rref(setMember(str, args.at(1).getVal()));
                           }),
                           FunctionTuple(CONTEXT(parent), "parent", 0, [this](FunctionArguments&) -> rref {
                               return parent.value().toRRef();
                           }),
                           FunctionTuple(CONTEXT(setParent), "setParent", 1, [this](FunctionArguments &args) -> rref {
                               args.check({"Class"});
                               setParent(args.as<Class>(0));
                               return args.env()->rnothing();
                           })
                       });
    if (name != "BaseClass"){
        class_obj = Optional<SmartReference<Class>>(env->getClass("BaseClass"));//does this work this way???
    } else {
        class_obj = Optional<SmartReference<Class>>(SmartReference<Class>(this));
    }
}

bool Class::isClass(std::string class_name){
    return name == class_name || (hasParent() && parent.value()->isClass(class_name));
}

bref Class::create(FunctionArguments &args){
    return call("create", args).getVal();
}

bool Class::hasMember(std::string member){
    return instance_members.find(member) != instance_members.end();
}

bref Class::getMember(std::string member){
    return instance_members[member];
}

bref Class::setMember(std::string member, bref value){
    instance_members.emplace(member, value);
    return value;
}

void Class::setParent(SmartReference<Class> parent){
    if (hasParent()){
        throw std::string("Set parent the second time");
    }
    this->parent = Optional<SmartReference<Class>>(parent);
    for (auto &it : parent->instance_members){
        if (!hasMember(it.first)){
            setMember(it.first, it.second->clone());
        }
    }
}

bool Class::hasParent(){
    return parent.has();
}

void Class::setClassFunctions(std::vector<FunctionTuple> funcs){
    for (size_t i = 0; i < funcs.size() - 2; i++){
        auto tup = funcs.at(i);
        ExceptionContext context = tup.context;
        std::string name = tup.name;
        size_t param_count = tup.param_count;
        std::function<rref(FunctionArguments &)> func = tup.impl_func;
        set(name, env->cppFunction(CONTEXT(name), parent_scope, param_count, func));
    }
}

bool Class::equalsClass(SmartReference<Class> other){
    return _id == other->id();
}

void Class::copyInstanceMembers(rref obj){
    for (auto &it : instance_members){
        obj->value()->set(it.first, it.second->bind(obj, it.first));
    }
}

void Class::setFunctionMembers(std::vector<FunctionTuple> funcs){
    for (size_t i = 0; i < funcs.size() - 2; i++){
        auto tup = funcs.at(i);
        ExceptionContext context = tup.context;
        std::string name = tup.name;
        size_t param_count = tup.param_count;
        std::function<rref(FunctionArguments &)> func = tup.impl_func;
        setMember(name, env->cppFunction(context, parent_scope, param_count, func));
    }
}
