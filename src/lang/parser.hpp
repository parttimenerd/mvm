#pragma once
#include "../utils.hpp"
#include "lexer.hpp"
#include "node.hpp"
#include "leaf.hpp"

namespace lang {

/**
 * @brief A simple parser for a JavaScript like language
 */
struct Parser {

    Lexer *lexer;

    Parser(Lexer *lexer){
        this->lexer = lexer;
        next();
    }

    Node* parse(){
        return parseExpression();
    }

    Node* parseExpression(){
        return parseAdd();
    }

    Node* parseAdd(){
        std::vector<Node*> children;
        children.push_back(parseSub());
        while (is(PLUS)){
            next();
            children.push_back(parseSub());
        }
        if (children.size() == 1){
            return children[0];
        }
        return createAddNode(children);
    }

    Node* parseSub(){
        std::vector<Node*> children;
        children.push_back(parseMul());
        while (is(MINUS)){
            next();
            children.push_back(parseMul());
        }
        if (children.size() == 1){
            return children[0];
        }
        return createSubNode(children);
    }

    Node* parseMul(){
        std::vector<Node*> children;
        children.push_back(parseDiv());
        while (is(MULTIPLY_SIGN)){
            next();
            children.push_back(parseDiv());
        }
        if (children.size() == 1){
            return children[0];
        }
        return createMulNode(children);
    }

    Node* parseDiv(){
        std::vector<Node*> children;
        children.push_back(parseMod());
        while (is(TokenType::DIVIDE_SIGN)){
            next();
            children.push_back(parseMod());
        }
        if (children.size() == 1){
            return children[0];
        }
        return createDivNode(children);
    }

    Node* parseMod(){
        std::vector<Node*> children;
        children.push_back(parseAtom());
        while (is(TokenType::MOD_SIGN)){
            next();
            children.push_back(parseAtom());
        }
        if (children.size() == 1){
            return children[0];
        }
        return createModNode(children);
    }

    Node* parseAtom(){
        if (is(INT)){
            return parseInt();
        } else if (is(BOOLEAN)){
            return parseString();
        } else if (is(STRING)){
            return parseBoolean();
        } else if (is(NOTHING)){
            return parseNothing();
        } else if (is(LEFT_BRACE)){
            return parseBraced();
        } else if (is(ID)){
            return parseIDandCall();
        } else {
            std::ostringstream stream;
            stream << "Unexpected token " << current()->str();
            stream << " expected atom expression instead";
            error(stream.str());
        }
        return 0;
    }

    Node* parseIDandCall(){
        std::string id = getArgument<std::string>();
        next();
        if (isNot(LEFT_BRACE)){
            return new VariableNode(id);
        }
        next();
        //std::cout << "|" << current()->str() << "\n";
        std::vector<Node*> arguments;
        while(isNot(RIGHT_BRACE)){
            if (arguments.size() > 0){
                if (is(COMMA)){
                    next();
                }
            }
            //std::cout << "||" << current()->str() << "\n";
            arguments.push_back(parseExpression());
            //std::cout << "||>" << current()->str() << "\n";
        }
        //std::cout << "|||" << current()->str() << "\n";
        return new CallNode(id, arguments);
    }

    Node* parseInt(){
        Node *ret = new IntNode(getArgument<int_type>());
        next();
        return ret;
    }

    Node* parseString(){
        Node *ret = new StringNode(getArgument<std::string>());
        next();
        return ret;
    }

    Node* parseBoolean(){
        Node *ret = new BooleanNode(getArgument<bool>());
        next();
        return ret;
    }

    Node* parseNothing(){
        Node *ret = new NothingNode();
        next();
        return ret;
    }

    Node* parseBraced(){
        next();
        Node* expr = parseExpression();
        parse(RIGHT_BRACE);
        return expr;
    }

    bool is(TokenType type){
        return current()->type == type;
    }

    bool isNot(TokenType type){
        return current()->type != type;
    }

    void parse(TokenType expectedType){
        if (is(expectedType)){
            next();
        } else {
            std::ostringstream stream;
            stream << "Expected " << tokenTypeToString(expectedType)
                   << " but got " << current()->str();
            error(stream.str());
        }
    }

    template<typename T>
    T getArgument(){
        return static_cast<ArgumentedToken<T>*>(current())->argument;
    }

    Token* next(){
        Token *nextToken = lexer->nextToken();
        //std::cout << "next token " << nextToken->str() << "\n";
        return nextToken;
    }

    Token* current(){
        return lexer->lastToken;
    }

    bool ended(){
        return lexer->ended();
    }

    void error(std::string msg, std::string msg1 = ""){
        std::ostringstream stream;
        stream << "Error in line " << lexer->lineNumber
               << ", around column " << lexer->columnNumber << ": " << msg << msg1;
        std::cerr << stream.str() << "\n";
        throw stream.str();
    }

};

}
