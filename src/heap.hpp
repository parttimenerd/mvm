#ifndef HEAP
#define HEAP

#include <string>
#include <unordered_map>
#include "utils.hpp"

struct HeapObject;
struct Int;
struct Nothing;

struct Heap {
	std::unordered_map<id_type, HeapObject*> heap;
	id_type max_id = 0;


	std::string str();

	/**
	 * Dereference the object and delete it if its reference count is zero
	 */
	void dereference(HeapObject *object);

	void add(HeapObject *obj);

	Int* createInt(int_type val);

	Nothing* createNothing();

	~Heap();
};

#endif
