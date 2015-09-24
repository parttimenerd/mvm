#pragma once

#include "utils.hpp"
#include "reference.hpp"
//#include "function.hpp"

class Function;
class Env;

/**
 * @brief Argument for a function or method
 */
class FunctionArguments {
    Env *_env;
    SmartReference<Function> self;
    std::vector<rref> arguments;
    std::vector<rref> misc_arguments;
    std::vector<rref> all_arguments;
    Optional<rref> thisObj;

public:
    FunctionArguments(Env* env, SmartReference<Function> self, std::vector<rref> arguments,
        std::vector<rref> misc_arguments,
        std::vector<rref> all_arguments)
        : _env(env), self(self), arguments(arguments), misc_arguments(misc_arguments),
          all_arguments(all_arguments), thisObj(Optional<rref>()) {}

    bool hasMiscArguments(){
        return !misc_arguments.empty();
    }

    bool hasTooMuchArguments(){
        return hasMiscArguments();
    }

    rref first(){
        return at(0);
    }

    rref second(){
        return at(1);
    }

    rref third(){
        return at(2);
    }

    rref at(size_t index);

    rref operator[](size_t i){
        return at(i);
    }

    bool hasThis(){
        return thisObj.has();
    }

    void setThis(rref new_this);

    bref getThis();

    rref getRThis();

    SmartReference<Function> getSelf();

    Env* env(){
        return _env;
    }

    template<typename S>
    SmartReference<S> as(size_t i){
        return at(i)->as<S>();
    }

    size_t size(){
        return all_arguments.size();
    }

    /**
     * @brief Return all arguments as vector.
     * Copies the references.
     */
    std::vector<rref> toVector();

    void check(std::vector<std::string> expected_types);
};
