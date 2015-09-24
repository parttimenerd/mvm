#pragma once

class HeapObject;
class Reference;
class BaseObject;
template<class T>
class SmartReference;

typedef SmartReference<Reference> rref;
typedef SmartReference<BaseObject> bref;
typedef SmartReference<HeapObject> href;


id_type __id_of_ho(HeapObject *obj);
void __ref_ho(HeapObject *obj);
void __deref_ho(HeapObject *obj);
BaseObject *__get_val(Reference *ref);
BaseObject* __get_val(BaseObject *obj);
rref __ato_rref(bref ref);

/**
 * A smart pointer comparable to std::shared_ptr,
 * but works with the garbage collection.
 */
template<class T>
class SmartReference {

private:
    T* obj = 0;

public:

    /**
     * @brief Creates a new reference mangaging the passed object.
     * Increments its ref count.
     *
     * @param t pointer to managed object
     */
    SmartReference(T *t) : obj(t) {
        L;
        if (t == 0){
            //throw std::string("created unitialized SmartReference");
        }
        L;
        this->reference();
        L;
    }

    SmartReference() { throw std::string("created unitialized SmartReference to ")
                + std::string(typeid(T).name());
    }

    /**
     * @brief Creates a new reference for the object managed be r.
     * Increments the ref count of the object.
     *
     * @param r reference to managed object
     */
    /*SmartReference(SmartReference<T> r){
        SmartReference(r.get());
    }*/


    /**
     * @brief Get a pointer to the managed object.
     *
     * \warning Don't store the returned pointer anywhere!
     *
     * @return pointer to the managed object.
     */
    T* get(){
        LOG_VAR(obj)
        if (obj == 0){
            throw std::string("Unitialized SmartReference access");
        }
        return obj;
    }

    /**
     * @brief Get the enclosed HeapObject reference if the managed object is a Reference.
     *
     * @return the enclosed HeapObject or the managed object, if it's not a reference.
     */
    bref getVal(){
        return __get_val(get());
    }

    /**
     * @brief Get the managed object.
     *
     * @return a reference to the managed object.
     */
    T& operator*(){
        return *get();
    }

    /**
     * @brief Alias to get()
     * @return a pointer to the managed object
     */
    T* operator->(){
        return get();
    }

    /**
     * @brief Alias for set()
     * Changes the managed object, but decrements the ref count of the old one first.
     * Increments the ref count of the new one.
     *
     * @param r reference to the new managed object
     * @return the current reference
     */
    SmartReference<T> operator=(SmartReference<T> r){
        return set(r);
    }

    /**
     * @brief Change the managed object.
     * Decrements the ref count of the old one first.
     * Increments the ref count of the new one.
     *
     * @param r reference to the new managed object
     * @return the current reference
     */
    SmartReference<T> set(SmartReference<T> r){
        if (obj != 0){
        __deref_ho((HeapObject*)get());
        }
        if (r != 0){
        __ref_ho((HeapObject*)r.get());
        obj = r.get();
        } else {
            obj = 0;
        }
        return *this;
    }

    /**
     * @brief Creates a new reference to the currently managed object.
     *
     * @return new reference
     */
    SmartReference<T> copy(){
        return SmartReference<T>(obj);
    }

    bool operator==(SmartReference<T> other){
        return __id_of_ho((HeapObject*)get())
                == __id_of_ho((HeapObject*)other.get());
    }

    bool operator!=(SmartReference<T> other){
        return !operator==(other);
    }

    SmartReference<HeapObject> toHRef(){
        return SmartReference<HeapObject>(obj);
    }

    rref toRRef(){
        return __ato_rref(SmartReference<BaseObject>(obj));
    }

    ~SmartReference(){
        __deref_ho((HeapObject*)obj);
    }

private:
    void reference(){
        if (obj == 0) return;
        L
        __ref_ho((HeapObject*)get());
        L
    }
};

/**
 * @brief Creates a new T object and places it into a SmartReference.
 */
template<typename T, typename... Args>
SmartReference<T> make_sref(Args... arguments){
    return SmartReference<T>(new T(arguments...));
}

template<typename T, typename... Args>
bref make_bref(Args... arguments){
    return bref(new T(arguments...));
}
