#pragma once

//#define L std::cerr << __LINE__ << "\n";

#include "../utils.hpp"
#include "target.hpp"
#include "lexer.hpp"

namespace lang {

enum NodeType {
    BASE,
    BINARY,
    BINARY_C,
    INFIX,
    LEAF,
    BINARY_L
};


/**
 * @brief Node of an AST
 */
struct Node {

    Context context;

    Node(Context context) : context(context) {}

    virtual std::vector<Node*> getChildren(){
        return std::vector<Node*>();
    }

    virtual void compile(Target &target) {
        context.compile(target);
        _compile(target);
    }

    virtual void _compile(Target&){}

    virtual std::string str(){
        return "";
    }

    virtual NodeType type(){
        return BINARY;
    }
};

struct InnerNode : Node {
    std::vector<Node*> children;

    InnerNode(Context context, std::vector<Node*> children)
        : Node(context), children(children) {}

    std::vector<Node*> getChildren(){
        return children;
    }

    virtual ~InnerNode(){
        for (auto &node : children){
            delete node;
        }
    }
};

struct InfixOperator : Node {
    Node* left;
    Node* right;

    InfixOperator(Context context, Node* left, Node* right)
        : Node(context), left(left), right(right) {}

    std::vector<Node*> getChildren() {
        return {left, right};
    }

    virtual NodeType type(){
        return BINARY;
    }

    virtual std::string str(){
        return std::string("(") + left->str() + right->str()
                + std::string(")");
    }

    ~InfixOperator(){
        delete left;
        delete right;
    }
};

struct CollectableInfixOperator : InfixOperator {

    using InfixOperator::InfixOperator;

    virtual void compile(Target &target){
        auto vec = collectSame();
        compile(target, vec);
    }

    virtual void compile(Target &target, std::vector<Node*> &children) {}

    void compileNodes(Target &target, std::vector<Node*> &children){
        for (auto *child : children){
            child->compile(target);
        }
    }

    bool isSame(Node* node){
        return node->type() == type() && static_cast<CollectableInfixOperator*>(node)->op() == op();
    }

    virtual std::string op(){
        return "";
    }

    std::vector<Node*> collectSame(){
        auto leftVec = collectSame(left);
        auto rightVec = collectSame(right);
        leftVec.insert(leftVec.end(), rightVec.begin(), rightVec.end());
        return leftVec;
    }

    std::vector<Node*> collectSame(Node* node){
        if (isSame(node)){
            return static_cast<CollectableInfixOperator*>(left)->collectSame();
        }
        return {node};
    }

    virtual NodeType type(){
        return BINARY_C;
    }
};

struct FuncInfixOperator : CollectableInfixOperator {

    std::string funcName;

    FuncInfixOperator(Context context, Node* left, Node* right, std::string funcName)
        : CollectableInfixOperator(context, left, right), funcName(funcName) {}

    virtual void compile(Target &target, std::vector<Node*> &children){
        compileNodes(target, children);
        context.compile(target);
        target.CALL_N(funcName, children.size());
    }

    std::string op(){
        return funcName;
    }

    std::string str(){
         return std::string("{") + funcName + std::string("}") + CollectableInfixOperator::str();
    }
};


struct AndNode : CollectableInfixOperator {

    using CollectableInfixOperator::CollectableInfixOperator;

    void compile(Target &target, std::vector<Node*> &children){
        size_t endLabel = target.inventLabel();
        context.compile(target);
        for (size_t i = 0; i < children.size() - 1; i++){
            children[i]->compile(target);
            target.DUP();
            target.JUMP_IF_NOT(endLabel);
        }
        children[children.size() - 1]->compile(target);
        target.placeLabel(endLabel);
    }

    std::string op(){
        return "AND";
    }

    std::string str(){
        return std::string("{AND}") + CollectableInfixOperator::str();
    }
};

struct OrNode : CollectableInfixOperator {

    using CollectableInfixOperator::CollectableInfixOperator;

    void compile(Target &target, std::vector<Node*> &children){
        size_t endLabel = target.inventLabel();
        context.compile(target);
        for (size_t i = 0; i < children.size() - 1; i++){
            children[i]->compile(target);
            target.DUP();
            target.JUMP_IF(endLabel);
        }
        children[children.size() - 1]->compile(target);
        target.placeLabel(endLabel);
    }

    std::string op(){
        return "OR";
    }

    std::string str(){
        return std::string("{OR}") + CollectableInfixOperator::str();
    }
};

struct ChainedLogicalNode : CollectableInfixOperator {

    std::string funcName;

    ChainedLogicalNode(Context context, Node* left, Node* right, std::string &funcName)
            : CollectableInfixOperator(context, left, right), funcName(funcName) {}

    void compile(Target &target, std::vector<Node*> &children){
        size_t falseLabel = target.inventLabel();
        context.compile(target);
        for (size_t i = 0; i < children.size() - 1; i++){
            children[i]->compile(target);
            children[i + 1]->compile(target);
            target.CALL_N(funcName, 2);
            target.JUMP_IF_NOT(falseLabel);
        }
        size_t endLabel = target.inventLabel();
        target.PUSH_BOOLEAN(true);
        target.JUMP(endLabel);
        target.placeLabel(falseLabel);
        target.PUSH_BOOLEAN(false);
        target.placeLabel(endLabel);
    }

    std::string op(){
        return funcName;
    }
};

struct CallNode : InnerNode {

    std::string name;

    CallNode(Context context, std::string name, std::vector<Node*> arguments)
        : InnerNode(context, arguments), name(name) {}

    void _compile(Target &target){
        for (size_t i = 0; i < children.size(); i++){
            children[i]->compile(target);
        }
        target.CALL_N(name, children.size());
    }
};

struct BlockNode : InnerNode {

    using InnerNode::InnerNode;

    void compile(Target &target){
        for (auto *child : children){
            child->compile(target);
        }
    }
};

struct SetVarNode : InfixOperator {
    using InfixOperator::InfixOperator;

    void _compile(Target &target){
        left->compile(target);
        right->compile(target);
        target.SET_VAR();
    }
};

struct UnaryOperator : Node {

    Node *child;

    UnaryOperator(Context context, Node *child): Node(context), child(child) {}

    virtual ~UnaryOperator(){
        delete child;
    }
};

struct UnaryFuncNode : UnaryOperator {
    std::string funcName;

    UnaryFuncNode(Context context, Node *child, std::string funcName)
        : UnaryOperator(context, child), funcName(funcName) {}

    void _compile(Target &target){
        child->compile(target);
        target.CALL_N(funcName, 1);
    }

};

template<typename T>
struct UnaryInstructionNode : Node {

    T value;

    UnaryInstructionNode(Context context, T value) : Node(context), value(value) {}

};

struct InitVarNode : UnaryInstructionNode<std::string> {
    using UnaryInstructionNode::UnaryInstructionNode;

    void _compile(Target &target){
        target.INIT_VAR(value);
    }
};

struct PushVarNode : UnaryInstructionNode<std::string> {
    using UnaryInstructionNode::UnaryInstructionNode;

    void _compile(Target &target){
        target.PUSH_VAR(value);
    }
};
}
