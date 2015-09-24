#include "baseobject.hpp"
#include "class.hpp"
#include "reference.hpp"
#include "env.hpp"
#include "function.hpp"

BaseObject::BaseObject(Env *env, SmartReference<Class> obj_class, Type type)
    : HeapObject(env, type), class_obj(obj_class) {}

BaseObject::BaseObject(Env *env, std::string class_name) : HeapObject(env, Type::OBJECT){
    class_obj = env->getClass(class_name);
}

BaseObject::BaseObject(Env *env) : HeapObject(env, Type::OBJECT){
    class_obj = env->getClass("BaseClass");
}

SmartReference<Class> BaseObject::getClass(){
    if (class_obj.has()){
        return class_obj.value();
    }
    throw std::string("no parent of this class");
}

rref BaseObject::get(std::string member_name){
    auto val = members[member_name];
    return val.copy();
}

rref BaseObject::set(std::string member_name, rref value){
    members.emplace(member_name, value.copy());
    return value.copy();
}

rref BaseObject::set(std::string member_name, bref value){
    members.emplace(member_name, to_rref(value).copy());
    return members[member_name].copy();
}

bool BaseObject::has(std::string member_name){
    return members.find(member_name) != members.end();
}

bool BaseObject::is(std::string type){
    return getClass()->is(type);
}

rref BaseObject::call(std::string method, std::vector<rref> args){
    rref val = get(method);
    if (val->value()->is("Function")){
        return val->as<Function>()->exec(args);
    }
    throw std::string("No method");
}

rref BaseObject::call(std::string method, FunctionArguments& args){
    return call(method, args.toVector());
}

bref BaseObject::bind(rref this_obj, std::string){
    return this_obj->value().copy();
}

void BaseObject::setDirect(bref other){
    if (hasSameClass(other)){
        for (auto &it : members){
            if (other->has(it.first)){
                members.emplace(it.first, other->get(it.first));
            } else {
                members.emplace(it.first, env->rnothing());
            }
        }
        _setDirect(other.copy());
    } else {
        throw std::string("can't set object direct to object of other class");
    }
}

bref BaseObject::clone(){
    bref cloned = make_bref<BaseObject>(env, class_obj.value().copy());
    copyMembers(cloned);
    return cloned;
}

bool BaseObject::equals(bref other){
    return hasSameClass(other.copy()) && _equals(other.copy());
}

bool BaseObject::equals(rref other){
    return equals(other->value());
}

bool BaseObject::hasSameClass(bref other){
    return other->getClass()->equalsClass(this->getClass());
}

rref BaseObject::call(std::vector<rref>&){
    return env->rnothing();
}

void BaseObject::copyMembers(bref other){
    for (auto &it : members){
        other->set(it.first, it.second.copy());
    }
}

rref BaseObject::__createNothing(){
    return env->rnothing();
}
