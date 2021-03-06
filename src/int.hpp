#ifndef INT_HPP
#define INT_HPP

#include "utils.hpp"

#include "heapobject.hpp"

/**
 * Int class for integer objects.
 */
struct Int : HeapObject {
	int_type value;


    Int(Env *env, int_type value) : HeapObject(Type::INT, env) {
		this->value = value;
	}

	std::string str(){
		std::ostringstream stringStream;
		stringStream << this->value;
		return stringStream.str();
	}

    bool operator==(HeapObject &obj) override {
        return obj.type == type && value == static_cast<Int&>(obj).value;
	}

    bool operator<(HeapObject &obj) override {
        return obj.type == type && value < dynamic_cast<Int&>(obj).value;
	}

    bool toBool(){
        return value != 0;
    }

    virtual Reference<HeapObject>* copy(){
        return new Reference<HeapObject>(env, new Int(env, value));
    }

    virtual void _set(HeapObject *other){
        value = ((Int*)other)->value;
    }

};

#endif
