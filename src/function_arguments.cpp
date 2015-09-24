#include "function_arguments.hpp"

#include "utils.hpp"
#include "reference.hpp"
#include "function.hpp"
#include "env.hpp"
#include "exception.hpp"

rref FunctionArguments::at(size_t index){
    return all_arguments.at(index).copy();
}

void FunctionArguments::setThis(rref new_this){
    if (!hasThis()){
        thisObj = Optional<rref>(new_this);
    } else {
        throw std::string("'this' already set");
    }
}

bref FunctionArguments::getThis(){
    if (!hasThis()){
        throw std::string("this is uninitialized.");
    }
    return thisObj.value()->value().copy();
}

rref FunctionArguments::getRThis(){
    if (!hasThis()){
        throw std::string("this is uninitialized.");
    }
    return thisObj.value().copy();
}

SmartReference<Function> FunctionArguments::getSelf()
{
    return self.copy();
}

/**
     * @brief Return all arguments as vector.
     * Copies the references.
     */
std::vector<rref> FunctionArguments::toVector(){
    std::vector<rref> new_vec;
    for (auto &element : all_arguments){
        new_vec.push_back(element.copy());
    }
    return new_vec;
}

void FunctionArguments::check(std::vector<std::string> expected_types){
    bool faulty = false;
    size_t i = 0;
    if (size() >= expected_types.size()){
        for (;i < expected_types.size(); i++){
            std::string exp = expected_types[i];
            if (exp != "*" && !all_arguments.at(i)->value()->is(exp)){
                faulty = true;
                break;
            }
        }
    } else {
        faulty = true;
    }
    if (faulty){
        std::ostringstream stream;
        stream << "Argument " << i << "has wrong type, got "
               << all_arguments.at(i)->value()->str()
               << " but expected one of type " << expected_types.at(i);
        throw new Exception("wrong type", stream.str(), getSelf()->getContext());
    }
}
