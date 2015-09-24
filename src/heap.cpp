#include "utils.hpp"

#include "heap.hpp"
#include "heapobject.hpp"

void Heap::dereference(HeapObject *obj){
    L
    if (obj->refCount() == 0 || obj->decrRefCount() == 0){
        for (auto _obj : obj->getReferencedObjects()){
            dereference(_obj.get());
        }
        L
        std::cerr << "delete " << obj->id() << "[" << obj->refCount() << "]\n";
        delete obj;
        heap.erase(obj->id());
    }
}

std::string Heap::str(){
    std::ostringstream stream;
    stream << "heap count=" << heap.size() << "\n";
    for (auto &t : this->heap){
        stream << " " << t.first << ":"
               << "value=" << t.second->escapedStr()
               << ", ref count=" << t.second->refCount() << "\n";
    }
    return stream.str();

}

void Heap::add(HeapObject *obj){
    heap.emplace(obj->id(), std::unique_ptr<HeapObject>(obj));
}
