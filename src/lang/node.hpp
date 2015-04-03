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

    virtual std::vector<Node*> getChildren(){
        return std::vector<Node*>();
    }

    virtual void compile(Target&) {}

    virtual std::string str(){
        return "";
    }

    virtual NodeType type(){
        return BINARY;
    }
};

struct InnerNode : Node {
    std::vector<Node*> children;

    InnerNode(std::vector<Node*> children) : children(children) {}

    std::vector<Node*> getChildren(){
        return children;
    }
};

struct BinaryOperator : Node {
    Node* left;
    Node* right;

    BinaryOperator(Node* left, Node* right) : left(left), right(right) {}

    std::vector<Node*> getChildren() {
        return {left, right};
    }

    virtual NodeType type(){
        return BINARY;
    }

    virtual TokenType op() = 0;

    virtual std::string str(){
        return std::string("{") + tokenTypeToString(op())
                + std::string("}(") + left->str() + right->str()
                + std::string(")");
    }
};

struct BinaryCollectableOperator : BinaryOperator {

    using BinaryOperator::BinaryOperator;

    void compile(Target &target){
        auto vec = collectSame();
        return compile(target, vec);
    }

    virtual void compile(Target&, std::vector<Node*>&) {}

    void compileNodes(Target &target, std::vector<Node*> &children){
        for (auto *child : children){
            child->compile(target);
        }
    }

    bool isSame(Node* node){
        return node->type() == type() && static_cast<BinaryCollectableOperator*>(node)->op() == op();
    }

    std::vector<Node*> collectSame(){
        auto leftVec = collectSame(left);
        auto rightVec = collectSame(right);
        leftVec.insert(leftVec.end(), rightVec.begin(), rightVec.end());
        return leftVec;
    }

    std::vector<Node*> collectSame(Node* node){
        if (isSame(node)){
            return static_cast<BinaryCollectableOperator*>(left)->collectSame();
        }
        return {node};
    }

    virtual NodeType type(){
        return BINARY_C;
    }
};

struct AddNode : BinaryCollectableOperator {

    using BinaryCollectableOperator::BinaryCollectableOperator;

    void compile(Target &target, std::vector<Node*> &children){
        compileNodes(target, children);
        target.CALL_N("add", children.size());
    }

    TokenType op(){
        return PLUS;
    }
};

struct SubNode : BinaryCollectableOperator {

    using BinaryCollectableOperator::BinaryCollectableOperator;

    void compile(Target &target, std::vector<Node*> &children){
        compileNodes(target, children);
        target.CALL_N("plus", children.size());
    }

    TokenType op(){
        return MINUS;
    }
};

struct MulNode : BinaryCollectableOperator {

    using BinaryCollectableOperator::BinaryCollectableOperator;

    void compile(Target &target, std::vector<Node*> &children){
        compileNodes(target, children);
        target.CALL_N("mul", children.size());
    }

    TokenType op(){
        return MULTIPLY_SIGN;
    }
};

struct DivNode : BinaryCollectableOperator {

    using BinaryCollectableOperator::BinaryCollectableOperator;

    void compile(Target &target, std::vector<Node*> &children){
        compileNodes(target, children);
        target.CALL_N("div", children.size());
    }

    TokenType op(){
        return DIVIDE_SIGN;
    }
};

struct ModNode : BinaryOperator {

    using BinaryOperator::BinaryOperator;

    void compile(Target &target){
        left->compile(target);
        right->compile(target);
        target.CALL_N("mod", 2);
    }

    TokenType op(){
        return MOD_SIGN;
    }
};

struct AndNode : BinaryCollectableOperator {

    using BinaryCollectableOperator::BinaryCollectableOperator;

    void compile(Target &target, std::vector<Node*> &children){
        size_t endLabel = target.inventLabel();
        for (size_t i = 0; i < children.size() - 1; i++){
            children[i]->compile(target);
            target.DUP();
            target.JUMP_IF_NOT(endLabel);
        }
        children[children.size() - 1]->compile(target);
        target.placeLabel(endLabel);
    }

    TokenType op(){
        return AND;
    }
};

struct OrNode : BinaryCollectableOperator {

    using BinaryCollectableOperator::BinaryCollectableOperator;

    void compile(Target &target, std::vector<Node*> &children){
        size_t endLabel = target.inventLabel();
        for (size_t i = 0; i < children.size() - 1; i++){
            children[i]->compile(target);
            target.DUP();
            target.JUMP_IF(endLabel);
        }
        children[children.size() - 1]->compile(target);
        target.placeLabel(endLabel);
    }

    TokenType op(){
        return OR;
    }
};

struct ChainedLogicalNode : BinaryCollectableOperator {

    using BinaryCollectableOperator::BinaryCollectableOperator;

    void compile(Target &target, std::vector<Node*> &children){
        size_t falseLabel = target.inventLabel();
        for (size_t i = 0; i < children.size() - 1; i++){
            children[i]->compile(target);
            children[i + 1]->compile(target);
            target.CALL_N(methodName(), 2);
            target.JUMP_IF_NOT(falseLabel);
        }
        size_t endLabel = target.inventLabel();
        target.PUSH_BOOLEAN(true);
        target.JUMP(endLabel);
        target.placeLabel(falseLabel);
        target.PUSH_BOOLEAN(false);
        target.placeLabel(endLabel);
    }

    virtual std::string methodName(){
        return "";
    }

};

struct LowerNode : ChainedLogicalNode {

    using ChainedLogicalNode::ChainedLogicalNode;

    std::string methodName(){
        return "lower";
    }

    TokenType op(){
        return LOWER;
    }
};

struct LowerEqualNode : ChainedLogicalNode {

    using ChainedLogicalNode::ChainedLogicalNode;

    std::string methodName(){
        return "leq";
    }

    TokenType op(){
        return LOWER_EQUAL;
    }
};

struct GreaterNode : ChainedLogicalNode {

    using ChainedLogicalNode::ChainedLogicalNode;

    std::string methodName(){
        return "greater";
    }

    TokenType op(){
        return GREATER;
    }
};

struct GreaterEqualNode : ChainedLogicalNode {

    using ChainedLogicalNode::ChainedLogicalNode;

    std::string methodName(){
        return "geq";
    }

    TokenType op(){
        return GREATER_EQUAL;
    }
};

struct EqualNode : ChainedLogicalNode {

    using ChainedLogicalNode::ChainedLogicalNode;

    std::string methodName(){
        return "eq";
    }

    TokenType op(){
        return DOUBLE_EQUAL_SIGN;
    }
};

struct NotEqualNode : ChainedLogicalNode {

    using ChainedLogicalNode::ChainedLogicalNode;

    std::string methodName(){
        return "neq";
    }

    TokenType op(){
        return NOT_EQUAL;
    }
};

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

struct BlockNode : InnerNode {

    using InnerNode::InnerNode;

    void compile(Target &target){
        for (auto *child : children){
            child->compile(target);
        }
    }
};

}
