#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP

#include "utils.hpp"

#include "heapobject.hpp"

struct Boolean : HeapObject {

    bool isTrue;

    Boolean(Env *env, bool isTrue) : HeapObject(Type::BOOLEAN, env) {
        this->isTrue = isTrue;
	}

	std::string str(){
		return isTrue ? "true" : "false";
	}

    bool operator==(HeapObject &obj){
        return obj.type == type && ((Boolean*)&obj)->isTrue == this->isTrue;
	}

    bool toBool(){
        return isTrue;
    }

    virtual Reference<HeapObject>* copy(){
        return (Reference<HeapObject>*)new Reference<Boolean>(env, new Boolean(env, isTrue));
    }

    virtual void _set(HeapObject *other){
        isTrue = ((Boolean*)other)->isTrue;
    }
};

#endif
