#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP

#include "utils.hpp"
#include "baseobject.hpp"

/**
 * @brief The Boolean class
 */
class Boolean : public BaseObject {
protected:
    bool _value;

public:
    Boolean(Env *env) : BaseObject(env, "String"), _value(false) {}
    Boolean(Env *env, bool value) : BaseObject(env, "String"), _value(value) {}

    std::string str(){
        return _value ? "true" : "false";
    }

    bool value(){
        return _value;
    }

    bref virtual clone(){
        bref cloned = make_bref<Boolean>(env, _value);
        copyMembers(cloned);
        return cloned;
    }

protected:
    bool _equals(bref other) {
        return to_bool(other) == _value;
    }

    void _setDirect(bref other) {
        _value = to_bool(other);
    }
};

#endif // BOOLEAN_HPP
