#ifndef HEAP_OBJECT
#define HEAP_OBJECT

#include <sstream>
#include <iostream>

#include "env.hpp"
#include "heap.hpp"

/** Types */
enum class Type : id_type {
	NOTHING,
	INT,
	FLOAT,
	OBJECT,
	STRING,
	BOOLEAN,
	MAP,
	ARRAY,
	SSCOPE //SCOPE doesn't work, is probably predefinded somewhere
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

struct Nothing : HeapObject {
	Nothing(Heap *heap) : HeapObject(Type::NOTHING, heap) {}

	std::string str(){
		return "nothing";
	}

	bool operator==(HeapObject *obj){
		return obj->type == type;
	}
};

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

	bool operator>(HeapObject *obj){
		return obj->type == type && value > ((Int*)obj)->value;
	}

};

struct String : HeapObject {
	std::string value;

	String(Heap *heap, std::string value) : HeapObject(Type::STRING, heap) {
		this->value = value;
	}

	std::string str(){
        std::ostringstream stream;
        stream << "'" << value << "'";
		return stream.str();
	}

	bool operator==(HeapObject *obj){
		return obj->type == type && value == ((String*)obj)->value;
	}

	bool operator>(HeapObject *obj){
		return obj->type == type && value > ((String*)obj)->value;
	}
};

enum class LineType : uint8_t {

};

/**
 * A line of code
 */
struct Line {
	LineType type;

	Line(LineType type){
		this->type = type;
	}
};


struct Function : HeapObject {
	id_type parent_scope_id;
	std::vector<std::string> parameters;

	void exec(Env *env);
};

struct CodeFunction : Function {
	std::vector<Line> lines;

};

#endif
