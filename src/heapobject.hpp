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

	Env *env;

	/**
	 * Initialize a new HeapObject in the passed environment
	 *
	 * @param type type of the object (@see Type)
	 * @param env passed environment
	 */
	HeapObject(Type type, Env *env){
		this->type = type;
		this->env = env;
		this->env->add(this);
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
        env->dereference(this);
	}
};
