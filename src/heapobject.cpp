#include "heapobject.hpp"

#include "utils.hpp"

#include "env.hpp"
#include "heap.hpp"
#include "reference.hpp"


HeapObject::HeapObject(Type type, Env *env){
    this->type = type;
    this->env = env;
    this->env->add(this);
}

Reference<HeapObject>* HeapObject::copy(){
    return new Reference<HeapObject>(env, this);
}

void HeapObject::set(HeapObject *other){
    if (other->isReference()){
        other = ((Reference<HeapObject>*)other)->value;
    }
    if (other->type != type){
        throw std::string("Can't set ") + str() + std::string(" direct to ") + other->str() + std::string(" because of type mismatch");
    }
    return _set(other);
}


void HeapObject::dereference(){
    env->dereference(this);
}

HeapObject* HeapObject::transfer(){
    if (reference_count == 0){
        throw std::string("Ref count of ") + escapedStr() + std::string(" is 0, can't reduce it");
    }
    reference_count--;
    return this;
}
