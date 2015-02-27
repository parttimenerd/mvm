#pragma once

#include "../utils.hpp"
#include "target.hpp"

namespace lang {

/**
 * @brief Node of an AST
 */
struct Node {

    bool isLeaf;

    virtual std::vector<Node*> getChildren(){
        return std::vector<Node*>();
    }

    virtual void compile(Target&) {}

    virtual std::string str(){
        return "";
    }
};

struct InnerNode : Node {
    std::vector<Node*> children;

    InnerNode(std::vector<Node*> children) : children(children) {}

    std::vector<Node*> getChildren(){
        return children;
    }
};

template<const char* functionName>
struct ArithmeticNode : InnerNode {

    using InnerNode::InnerNode;

    void compile(Target &target){
        for (size_t i = 0; i < children.size(); i++){
            children[i]->compile(target);
        }
        target.CALL_N(std::string(functionName), children.size());
    }

};

static constexpr char addFuncName[] = "add";
static constexpr char subFuncName[] = "sub";
static constexpr char mulFuncName[] = "mul";
static constexpr char divFuncName[] = "div";
static constexpr char modFuncName[] = "mod";

Node* createAddNode(std::vector<Node*> &children){
    return new ArithmeticNode<addFuncName>(children);
}

Node* createSubNode(std::vector<Node*> &children){
    return new ArithmeticNode<subFuncName>(children);
}

Node* createMulNode(std::vector<Node*> &children){
    return new ArithmeticNode<mulFuncName>(children);
}

Node* createDivNode(std::vector<Node*> &children){
    return new ArithmeticNode<divFuncName>(children);
}

Node* createModNode(std::vector<Node*> &children){
    return new ArithmeticNode<modFuncName>(children);
}

struct CallNode : InnerNode {

    std::string name;
    CallNode(std::string name, std::vector<Node*> arguments) : InnerNode(arguments), name(name) {}

    void compile(Target &target){
        for (size_t i = 0; i < children.size(); i++){
            children[i]->compile(target);
        }
        target.CALL_N(name, children.size());
    }
};

}
