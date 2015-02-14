#ifndef NOTHING_HPP
#define NOTHING_HPP

#include "utils.hpp"

#include "heapobject.hpp"

struct Nothing : HeapObject {
	Nothing(Env *env) : HeapObject(Type::NOTHING, env) {}

	std::string str(){
		return "nothing";
	}

	bool operator==(HeapObject *obj){
		return obj->type == type;
	}
};

#endif
