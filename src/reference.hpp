#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include "utils.hpp"
#include "heapobject.hpp"
#include "smartreference.hpp"

class Env;

/**
 * A simple reference containing a SmartReference to an object.
 */
class Reference : public HeapObject {

    bref _value;

public:
    Reference(bref value);

    std::string str();

    std::string escapedStr();

    template<typename S>
    SmartReference<S> as(){
        return SmartReference<S>((S*)(_value.get()));
    }

    void set(bref other){
        _value = other;
    }

    virtual std::vector<href> getReferencedObjects();

    bref value(){
        return _value;
    }
};

/**
 * @brief Creates a new T object and places it into a rref.
 */
template<typename T, typename... Args>
rref make_rref(Args... parameters){
    return rref(new Reference(SmartReference<BaseObject>(new T(parameters...))));
}


#endif
