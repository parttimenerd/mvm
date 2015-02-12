#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "utils.hpp"
#include "heapobject.hpp"

/*
 * Resizable array
 */
struct Array : HeapObject {
    std::vector<HeapObject*> value;

	Array(Heap *heap, std::vector<HeapObject*> value) : HeapObject(Type::ARRAY, heap) {
		this->value = value;
	}

	std::string str(){
        std::ostringstream stream;
        stream << "[ ";
        for (size_t i = 0; i < value.size(); i++){
            stream << value.at(i)->str() << " ";
        }
        stream << "]";
		return stream.str();
	}

	HeapObject* at(size_t i){
        return value.at(i);
	}

	size_t size(){
        return value.size();
	}

	void set(size_t i, HeapObject *obj){
        value[i] = obj;
	}

	void add(HeapObject *obj){
        value.push_back(obj);
	}

	bool operator==(HeapObject*){
		return false;
	}

	bool operator>(HeapObject*){
		return false;
	}
};

#endif
