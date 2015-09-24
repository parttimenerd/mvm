#ifndef NOTHING_HPP
#define NOTHING_HPP

#include "utils.hpp"
#include "baseobject.hpp"

/**
 * @brief The Nothing class
 * The 'nothing' is comparable to 'nil' in ruby or 'undefined' in JavaScript.
 */
class Nothing : public BaseObject {
public:
    Nothing(Env *env) : BaseObject(env, "Nothing") {}

    std::string str(){
        return "nothing";
    }

protected:
    bool _equals(bref){
        return true;
    }
};

#endif // NOTHING_HPP
