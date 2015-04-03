#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include "utils.hpp"
#include "heapobject.hpp"


/**
 * A simple reference.
 */
template<class T>
struct Reference : HeapObject {

    //typedef typename std::enable_if<std::is_base_of<HeapObject, T>::value>::type check;

    T *value;

    Reference(Env *env, T *value, bool reference = true) : HeapObject(Type::REFERENCE, env){
        this->value = value;
        if (reference){
            value->reference();
        }
    }

    std::string str(){
        std::ostringstream stream;
        stream << "[ref to:" << value->str() << stream << "]";
        return stream.str();
    }

    bool operator==(T&){
		return false;
	}

    bool operator>(T&){
		return false;
	}

    void foo(){ // only needed to tell the compiler, that T should be a subclass of HeapObject
        (void)static_cast<HeapObject>(*value);
    }

    Reference<T>* transfer(){
        if (reference_count == 0){
            throw std::string("Ref count of ") + escapedStr() + std::string(" is 0, can't reduce it");
        }
        reference_count--;
        return this;
    }

    virtual std::vector<HeapObject*> getReferencedObjects(){
        return {value};
    }
};

/**
 * A reference wrapper derefencing the wrapped reference if it's going out of scope.
 */
template<class T>
struct SmartReference {
    Reference<T> *reference;
    T* value;

    SmartReference(Reference<T> *ref){
        reference = ref;
        value = ref->value;
    }

    ~SmartReference(){
        reference->dereference();
    }


};

template<class T>
SmartReference<T> make_sref(Reference<T> *ref){
    return SmartReference<T>(ref);
}

template<class T>
SmartReference<T> make_sref(T *_obj){
    HeapObject *obj = static_cast<HeapObject*>(_obj);
    if (obj->isReference()){
        return SmartReference<T>((Reference<T>*)_obj);
    } else {
        return SmartReference<T>(new Reference<T>(obj->env, _obj));
    }
}

/**
 * @brief Wrap an object into a reference object, if it isn't already a reference
 *
 * @param obj
 * @return
 */
template<class T>
Reference<T>* toReference(T* obj){
    while(obj->isReference() && ((Reference<T>*)obj)->value->isReference()){
        obj = ((Reference<T>*)obj)->value;
    }
    if (!obj->isReference()){
        obj = new Reference<T>(obj->env, obj);
    }
    return (Reference<T>*)obj;
}


template<class T>
SmartReference<T>* toSmartReference(T* obj){
    return make_sref(toReference(obj));
}

#endif
