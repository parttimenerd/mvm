#include "reference.hpp"

#include "utils.hpp"
#include "heapobject.hpp"
#include "baseobject.hpp"
#include "smartreference.hpp"
#include "env.hpp"

Reference::Reference(bref value)
    : HeapObject::HeapObject(value->getEnv(), Type::REFERENCE), _value(value) {
}

std::string Reference::str(){
    std::ostringstream stream;
    stream << "[ref to:" << _value->str() << "]";
    return stream.str();
}

std::string Reference::escapedStr(){
    std::ostringstream stream;
    stream << "[ref to:" << _value->escapedStr() << "]";
    return stream.str();
}

std::vector<href> Reference::getReferencedObjects(){
    return {_value.toHRef()};
}
