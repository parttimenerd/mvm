#pragma once

#include "utils.hpp"

struct Env;
template<class T>
struct Reference;

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
    FUNCTION,
    REFERENCE
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
    HeapObject(Type type, Env *env);

	virtual std::string str(){
		return "";
	}

    virtual std::string escapedStr(){
        return str();
    }

	virtual std::vector<HeapObject*> getReferencedObjects() {
		return std::vector<HeapObject*>();
	}

    virtual Reference<HeapObject>* copy();

    virtual void set(HeapObject *other);

    virtual void _set(HeapObject*){
    }

    virtual bool operator==(HeapObject &obj){
        return obj.id == id;
	}

    virtual bool operator<(HeapObject&){
        return false;
	}

    bool operator<=(HeapObject &obj){
        return *this < obj || *this == obj;
    }

    bool operator>=(HeapObject &obj){
        return !(*this < obj);
    }

    bool operator>(HeapObject &obj){
        return obj < *this;
    }

    virtual bool operator!=(HeapObject &obj){
        return !(*this == obj);
    }

    virtual bool toBool(){
        return true;
    }

	void reference(){
        reference_count++;
	}

    void dereference();

    bool is(Type type){
        return this->type == type;
    }

    bool isReference(){
        return this->type == Type::REFERENCE;
    }

    bool isMap(){
        return this->type == Type::MAP || this->type == Type::FUNCTION;
    }

    bool isNumeric(){
        return this->type == Type::INT || this->type == Type::FLOAT;
    }

    bool isFloat(){
        return this->type == Type::FLOAT;
    }

    bool isInt(){
        return this->type == Type::INT;
    }

    bool isFunction(){
        return this->type == Type::FUNCTION;
    }


    HeapObject* transfer();

	virtual ~HeapObject() = default;
};
