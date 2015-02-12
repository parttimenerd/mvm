#ifndef INT_HPP
#define INT_HPP

#include "utils.hpp"

#include "heapobject.hpp"

/**
 * Int class for integer objects.
 */
struct Int : HeapObject {
	int_type value;


	Int(Heap *heap, int_type value) : HeapObject(Type::INT, heap) {
		this->value = value;
	}

	std::string str(){
		std::ostringstream stringStream;
		stringStream << this->value;
		return stringStream.str();
	}

	bool operator==(HeapObject *obj){
		return obj->type == type && value == ((Int*)obj)->value;
	}

	bool operator<(HeapObject *obj){
		return obj->type == type && value < ((Int*)obj)->value;
	}

};

#endif
