#pragma once

#include "../utils.hpp"
#include "node.hpp"

namespace lang {

template<typename T>
struct Leaf : Node {
    bool isLeaf = true;
    T value;

    Leaf(T value): value(value) {}

    std::string str(){
        std::ostringstream stream;
        stream << "[" << value << "]";
        return stream.str();
    }
};

struct IntNode : Leaf<int_type> {
    using Leaf::Leaf;

    void compile(Target &target){
        target.PUSH_INT(value);
    }
};

}
