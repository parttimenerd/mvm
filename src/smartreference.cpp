#include "smartreference.hpp"
#include "heapobject.hpp"


SmartReference::SmartReference(T *t) : obj(t) {
    static_cast<HeapObject>(*obj)->reference();
}

SmartReference::SmartReference(SmartReference<T> r){
    SmartReference(r.get());
}

T *SmartReference::get(){
    return obj;
}

SmartReference<HeapObject> SmartReference::getVal(){
    auto ref = static_cast<HeapObject*>(obj);
    if (ref.isReference()){
        return static_cast<Reference>(*ref).get();
    }
    return static_cast<SmartReference<HeapObject>>(*this);
}

T &SmartReference::operator*(){
    return *get();
}

T *SmartReference::operator->(){
    return get();
}

SmartReference<T> SmartReference::operator=(SmartReference<T> r){
    return set(r);
}

SmartReference<T> SmartReference::set(SmartReference<T> r){
    static_cast<HeapObject>(*obj).dereference();
    static_cast<HeapObject>(*r).reference();
    obj = r;
    return *this;
}

SmartReference<T> SmartReference::copy(){
    return SmartReference<T>(obj);
}


SmartReference<T> make_sref(Args arguments){
    return SmartReference<T>(new T(arguments...));
}
