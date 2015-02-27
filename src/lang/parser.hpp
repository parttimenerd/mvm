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
        } else if (is(LEFT_BRACE)){
            next();
            Node* expr = parseExpression();
            parse(RIGHT_BRACE);
            return expr;
        } else {
            std::ostringstream stream;
            stream << "Unexpected token " << current()->str();
            stream << " expected atom expression instead";
            error(stream.str());
        }
        return 0;
    }

    Node* parseInt(){
        //std::cout << "hey, I'm parserInt\n";
        if (is(INT)){
            Node *ret = new IntNode(getArgument<int_type>());
            next();
            return ret;
        }
        error("Expected INT but got ", current()->str());
        return 0;
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
