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
        lexer->nextToken();
    }

    Node* parse(){
        return parseInt();
    }

    Node* parseInt(){
        if (is(INT)){
            return new IntNode(getArgument<int_type>());
        }
        error("Expected INT got ", current()->str());
        return 0;
    }

    bool is(TokenType type){
        return current()->type == type;
    }

    bool isNot(TokenType type){
        return current()->type != type;
    }

    template<typename T>
    T getArgument(){
        return static_cast<ArgumentedToken<T>*>(current())->argument;
    }

    Token* next(){
        return lexer->nextToken();
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
