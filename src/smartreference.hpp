#pragma once

class HeapObject;

/**
 * A smart pointer comparable to std::shared_ptr,
 * but works with the garbage collection.
 */
template<class T>
class SmartReference {

    T* obj;

public:

    /**
     * @brief Creates a new reference mangaging the passed object.
     * Increments its ref count.
     *
     * @param t pointer to managed object
     */
    SmartReference(T *t);

    /**
     * @brief Creates a new reference for the object managed be r.
     * Increments the ref count of the object.
     *
     * @param r reference to managed object
     */
    SmartReference(SmartReference<T> r);

    /**
     * @brief Get a pointer to the managed object.
     *
     * \warning Don't store the returned pointer anywhere!
     *
     * @return pointer to the managed object.
     */
    T* get();

    /**
     * @brief Get the enclosed HeapObject reference if the managed object is a Reference.
     *
     * @return the enclosed HeapObject or the managed object, if it's not a reference.
     */
    SmartReference<HeapObject> getVal();

    /**
     * @brief Get the managed object.
     *
     * @return a reference to the managed object.
     */
    T& operator*();

    /**
     * @brief Alias to get()
     * @return a pointer to the managed object
     */
    T* operator->();

    /**
     * @brief Alias for set()
     * Changes the managed object, but decrements the ref count of the old one first.
     * Increments the ref count of the new one.
     *
     * @param r reference to the new managed object
     * @return the current reference
     */
    SmartReference<T> operator=(SmartReference<T> r);

    /**
     * @brief Change the managed object.
     * Decrements the ref count of the old one first.
     * Increments the ref count of the new one.
     *
     * @param r reference to the new managed object
     * @return the current reference
     */
    SmartReference<T> set(SmartReference<T> r);

    /**
     * @brief Creates a new reference to the currently managed object.
     *
     * @return new reference
     */
    SmartReference<T> copy();

};

/**
 * @brief Creates a new T object and places it into a SmartReference.
 */
template<typename T, typename... Args>
SmartReference<T> make_sref(Args... parameters);
