#ifndef STRING_HPP
#define STRING_HPP

#include "utils.hpp"
#include "baseobject.hpp"

/**
 * @brief The String class
 */
class String : public BaseObject {
protected:
    std::string _value;

public:
    String(Env *env) : BaseObject(env, "String"), _value("") {}
    String(Env *env, std::string value) : BaseObject(env, "String"), _value(value) {}

    std::string str(){
        return _value;
    }

    std::string value(){
        return _value;
    }

    bref virtual clone(){
        bref cloned = make_bref<String>(env, _value);
        copyMembers(cloned);
        return cloned;
    }

protected:
    bool _equals(bref other) {
        return to_string(other) == _value;
    }

    void _setDirect(bref other) {
        _value = to_string(other);
    }
};
#endif // STRING_HPP
