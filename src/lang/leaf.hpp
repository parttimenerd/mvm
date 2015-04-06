#pragma once

#include "../utils.hpp"
#include "node.hpp"

namespace lang {

template<typename T>
struct Leaf : Node {
    T value;

    Leaf(Context context, T value): Node(context), value(value) {}

    std::string str(){
        std::ostringstream stream;
        stream << "[" << value << "]";
        return stream.str();
    }

    virtual NodeType type(){
        return LEAF;
    }

    virtual bool isVariableNode(){
        return false;
    }
};

struct IntNode : Leaf<int_type> {
    using Leaf::Leaf;

    void _compile(Target &target){
        target.PUSH_INT(value);
    }
};

struct FloatNode : Leaf<float_type> {
    using Leaf::Leaf;

    void _compile(Target &target){
        target.PUSH_FLOAT(value);
    }
};

struct StringNode : Leaf<std::string> {
    using Leaf::Leaf;

    void _compile(Target &target){
        target.PUSH_STRING(value);
    }
};

struct VariableNode : Leaf<std::string> {
    using Leaf::Leaf;

    void _compile(Target &target){
        target.PUSH_VAR(value);
    }

    bool isVariableNode(){
        return true;
    }

};

struct InitVarNode : Leaf<std::string> {
    using Leaf::Leaf;

    void _compile(Target &target){
        target.INIT_VAR(value);
    }
};

struct BooleanNode : Leaf<bool> {
    using Leaf::Leaf;

    void _compile(Target &target){
        target.PUSH_BOOLEAN(value);
    }
};

struct NothingNode : Node {
    bool isLeaf = true;

    using Node::Node;

    void _compile(Target &target){
        target.PUSH_NOTHING();
    }

    std::string str(){
        return "[nothing]";
    }
};

CallNode *createMapNode(Context context, std::vector<Node*> arguments = std::vector<Node*>()){
    return new CallNode(context, new StringNode(context, "create_map"), arguments);
}

CallNode *createArrayNode(Context context, std::vector<Node*> arguments = std::vector<Node*>()){
    return new CallNode(context, new StringNode(context, "create_array"), arguments);
}

}
