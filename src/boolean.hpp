#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP

#include "utils.hpp"

#include "heapobject.hpp"

struct Boolean : HeapObject {

    bool isTrue;

	Boolean(Heap *heap, bool isTrue) : HeapObject(Type::BOOLEAN, heap) {
        this->isTrue = isTrue;
	}

	std::string str(){
		return isTrue ? "true" : "false";
	}

	bool operator==(HeapObject *obj){
		return obj->type == type && ((Boolean*)obj)->isTrue == this->isTrue;
	}
};

#endif
