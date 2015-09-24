#include "heapobject.hpp"
#include "baseobject.hpp"
#include "reference.hpp"

id_type __id_of_ho(HeapObject *obj){
    return obj->id();
}


void __ref_ho(HeapObject *obj){
    L
    if (obj != 0){
        L
        obj->reference();
        L
    }
    L
}


void __deref_ho(HeapObject *obj){
    obj->dereference();
}

BaseObject *__get_val(Reference *ref){
    return ref->value().get();
}

BaseObject *__get_val(BaseObject *obj){
    return obj;
}


rref __ato_rref(bref ref){
    return rref(new Reference(ref));
}
