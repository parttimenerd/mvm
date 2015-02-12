#ifndef NOTHING_HPP
#define NOTHING_HPP

#include "utils.hpp"

#include "heapobject.hpp"

struct Nothing : HeapObject {
	Nothing(Heap *heap) : HeapObject(Type::NOTHING, heap) {}

	std::string str(){
		return "nothing";
	}

	bool operator==(HeapObject *obj){
		return obj->type == type;
	}
};

#endif
