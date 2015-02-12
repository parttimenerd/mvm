#pragma once

#include "utils.hpp"

#include "env.hpp"
#include "heap.hpp"

/** Types */
enum class Type : id_type {
	NOTHING,
	INT,
	FLOAT,
	STRING,
	BOOLEAN,
	MAP,
	ARRAY,
	SCOPE,
	FUNCTION
};

/**
 * Base class for simple heap objects.
 */
struct HeapObject {
	Type type;
	id_type id;
	uint32_t reference_count = 0;

	Heap *heap;

	/**
	 * Initialize a new HeapObject on the passed heap
	 *
	 * @param type type of the object (@see Type)
	 * @param heap passed heap
	 */
	HeapObject(Type type, Heap *heap){
		this->type = type;
		this->heap = heap;
		this->heap->add(this);
		std::cout << heap->str();
	}

	virtual std::string str(){
		return "";
	}

	virtual std::vector<HeapObject*> getReferencedObjects() {
		return std::vector<HeapObject*>();
	}

	virtual id_type copy(){
		return 0;
	}

	virtual bool operator==(HeapObject *obj){
		return obj->id == id;
	}

	virtual bool operator<(HeapObject*){
        return false;
	}

	void reference(){
	  reference_count++;
	}

	void dereference(){
	  heap->dereference(this);
	}
};
