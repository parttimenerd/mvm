#include "utils.hpp"

#include "heap.hpp"
#include "heapobjects.hpp"

void Heap::dereference(HeapObject *object){
	if (object->reference_count == 0 || --(object->reference_count) == 0){
		for (auto *obj : object->getReferencedObjects()){
			dereference(obj);
		}
		heap.erase(object->id);
	}
}

std::string Heap::str(){
    std::ostringstream stream;
    stream << "heap count=" << heap.size() << "\n";
    for (auto t : this->heap){
        stream << " " << t.first << ":"
               << "value=" << t.second->str()
               << ", ref count=" << t.second->reference_count << "\n";
    }
    return stream.str();

}

void Heap::add(HeapObject *obj){
    obj->id = max_id++;
    heap.emplace(obj->id, obj);
}

Int* Heap::createInt(int_type val, bool reference){
    Int *integer = new Int(this, val);
    if (reference) {
        integer->reference();
    }
    return integer;
}

Nothing* Heap::createNothing(bool reference){
    Nothing *nothing = new Nothing(this);
    if (reference) {
        nothing->reference();
    }
    return nothing;
}

Array* Heap::createArray(std::vector<HeapObject*> value, bool reference){
    Array* arr = new Array(this, value);
    if (reference) {
        arr->reference();
    }
    return arr;
}

Map* Heap::createMap(bool reference){
    Map* map = new Map(this);
    if (reference) {
        map->reference();
    }
    return map;
}

Boolean* Heap::createBoolean(bool isTrue, bool reference){
    Boolean* boolean = new Boolean(this, isTrue);
    if (reference) {
        boolean->reference();
    }
    return boolean;
}



Heap::~Heap(){
    //for (auto t : this->heap){
        //heap.erase(t.second->id);
    //}
}
