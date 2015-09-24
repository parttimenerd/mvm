#include "converters.hpp"
#include "boolean.hpp"
#include "string.hpp"
#include "reference.hpp"

void type_check(bref obj, std::string expected){
    if (!obj->is(expected)){
        throw std::string("Type mismatch");
    }
}

void type_check(rref obj, std::string expected){
    type_check(obj->value(), expected);
}

#define DEF(return_type, name, class_name)\
    return_type name(rref obj){\
        type_check(obj, #class_name);\
        return obj->as<class_name>()->value();\
    }\
    return_type name(bref obj){\
        type_check(obj, #class_name);\
        return ((class_name*)(obj.get()))->value();\
    }

DEF(bool, to_bool, Boolean)

DEF(std::string, to_string, String)

rref to_rref(bref obj){
    return SmartReference<Reference>(new Reference(obj));
}
