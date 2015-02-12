#ifndef HEAP
#define HEAP

#include "utils.hpp"

struct HeapObject;
struct Int;
struct Nothing;
struct Array;
struct Map;
struct Boolean;

struct Heap {
	std::unordered_map<id_type, HeapObject*> heap;
	id_type max_id = 0;


	std::string str();

	/**
	 * Dereference the object and delete it if its reference count is zero
	 */
	void dereference(HeapObject *object);

	void add(HeapObject *obj);

	Int* createInt(int_type val, bool reference = false);

	Nothing* createNothing(bool reference = false);

	Array* createArray(std::vector<HeapObject*> value, bool reference = false);

    Map* createMap(bool reference = false);

    Boolean* createBoolean(bool isTrue, bool reference = false);

	~Heap();
};

#endif
