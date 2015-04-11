#pragma once

#include "utils.hpp"
#include "reference.hpp"

struct Function;

struct FunctionArguments {
    Function *self;
    std::vector<Reference<HeapObject>*> arguments;
    std::vector<Reference<HeapObject>*> misc_arguments;
    std::vector<Reference<HeapObject>*> all_arguments;

    FunctionArguments(Function *self, std::vector<Reference<HeapObject>*> arguments,
        std::vector<Reference<HeapObject>*> misc_arguments,
        std::vector<Reference<HeapObject>*> all_arguments){
        this->self = self;
        this->arguments = arguments;
        this->misc_arguments = misc_arguments;
        this->all_arguments = all_arguments;
    }

    bool hasMiscArguments(){
        return !misc_arguments.empty();
    }

    bool hasTooMuchArguments(){
        return hasMiscArguments();
    }
};
