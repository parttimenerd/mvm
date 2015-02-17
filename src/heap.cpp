#include "utils.hpp"

#include "heap.hpp"
#include "heapobjects.hpp"

void Heap::dereference(HeapObject *object){
	if (object->reference_count == 0 || --(object->reference_count) == 0){
		for (auto *obj : object->getReferencedObjects()){
			dereference(obj);
		}
		delete object;
		heap.erase(object->id);
	}
}

std::string Heap::str(){
    std::ostringstream stream;
    stream << "heap count=" << heap.size() << "\n";
    for (auto t : this->heap){
        stream << " " << t.first << ":"
               << "value=" << t.second->escapedStr()
               << ", ref count=" << t.second->reference_count << "\n";
    }
    return stream.str();

}

void Heap::add(HeapObject *obj){
    obj->id = max_id;
    max_id++;
    heap.emplace(obj->id, obj);
}


Heap::~Heap(){
    //for (auto t : this->heap){
        //heap.erase(t.second->id);
    //}
}
