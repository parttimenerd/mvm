#ifndef FLOAT_HPP
#define FLOAT_HPP

#include "utils.hpp"

#include "heapobject.hpp"

/**
 * Int class for integer objects.
 */
struct Float : HeapObject {
    float_type value;


    Float(Env *env, float_type value) : HeapObject(Type::FLOAT, env) {
		this->value = value;
	}

	std::string str(){
		std::ostringstream stringStream;
		stringStream << this->value;
		return stringStream.str();
	}

    bool operator==(HeapObject &obj) override {
        return obj.type == type && value == static_cast<Float&>(obj).value;
	}

    bool operator<(HeapObject &obj) override {
        return obj.type == type && value < dynamic_cast<Float&>(obj).value;
	}

    bool toBool(){
        return value != 0;
    }

    virtual Reference<HeapObject>* copy(){
        return new Reference<HeapObject>(env, new Float(env, value));
    }

    virtual void _set(HeapObject *other){
        value = ((Float*)other)->value;
    }

};

#endif
