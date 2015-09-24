#include "heapobject.hpp"

#include "utils.hpp"

#include "env.hpp"
#include "heap.hpp"
#include "reference.hpp"


HeapObject::HeapObject(Env *env, Type type)
    : type(type), env(env) {
    _id = env->heap()->newId();
    env->heap()->add(this);
}

void HeapObject::dereference(){
    env->heap()->dereference(this);
}

void HeapObject::reference(){
    L
    this->ref_count++;
    L
}

Env* HeapObject::getEnv(){
    return env;
}

void HeapObject::setEnv(Env *env){
    this->env = env;
}
