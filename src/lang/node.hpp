#pragma once

#include "../utils.hpp"
#include "target.hpp"

namespace lang {

/**
 * @brief Node of an AST
 */
struct Node {

    bool isLeaf;

    virtual std::vector<Node*> children(){
        return std::vector<Node*>();
    }

    virtual void compile(Target&) {}

    virtual std::string str(){
        return "";
    }
};

struct InnerNode : Node {
    std::vector<Node*> _children;

    std::vector<Node*> children(){
        return _children;
    }
};

}
