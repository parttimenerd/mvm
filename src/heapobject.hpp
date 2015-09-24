#pragma once

#include "utils.hpp"

class Env;
class Reference;

enum class Type {
    OBJECT,
    CLASS,
    META_CLASS,
    META_META_CLASS,
    REFERENCE,
    SCOPE
};

/**
 * @brief The base class for objects placed on the heap.
 *
 * It implements basic methods needed for garbage collection and provides
 * helper methods for extending classes.
 */
class HeapObject {

protected:
    Type type;
    id_type _id;
    counter_type ref_count = 0;

    Env *env; /**< using a pointer here is okay, as this object doesn't own env */

public:

    HeapObject(Env *env, Type type = Type::OBJECT);

    /**
     * @brief Decrement the reference count and do some garbage collection.
     */
    void dereference();

    /**
     * @brief Increase the reference count
     */
    void reference();

    /**
     * @brief Get the id of this object.
     * @return id
     */
    id_type id(){
        return _id;
    }

    /**
     * @brief Get the reference count of this object.
     * @return reference count
     */
    counter_type refCount(){
        return ref_count;
    }

    counter_type decrRefCount(){
        return --ref_count;
    }

    virtual std::string str(){
        return "";
    }

    virtual std::string escapedStr(){
        return str();
    }

    bool isReference(){
        return type == Type::REFERENCE;
    }

    virtual std::vector<href> getReferencedObjects() {
        return {};
    }

    /**
     * @brief Is this Object a real Object instance?
     */
    bool isObject(){
        return type == Type::OBJECT;
    }

    bool isClass(){
        return type == Type::CLASS;
    }

    bool isMetaClass(){
        return type == Type::META_CLASS;
    }

    virtual bool isFunction(){
        return false;
    }

    Env* getEnv();

    virtual ~HeapObject() = default;

    void setEnv(Env *env);
};
