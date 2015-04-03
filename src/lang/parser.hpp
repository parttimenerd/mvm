#pragma once
#include "../utils.hpp"
#include "lexer.hpp"
#include "node.hpp"
#include "leaf.hpp"

namespace lang {

enum Associativity {
    LEFT_ASSOC,
    RIGHT_ASSOC
};

/**
 * @brief A simple parser for a JavaScript like language
 */
struct Parser {

    typedef std::tuple<size_t, Associativity, std::function<Node*(Node*, Node*)>> binOp;

    /**
     * Contains tuples (priority/precedence, associativity, function to create a node) for
     * each operator
     *
     * @see https://en.wikipedia.org/wiki/Order_of_operations
     */
    std::unordered_map<std::string, binOp>  binaryOperators = {
        {"||", std::make_tuple(30, LEFT_ASSOC, [](Node* l, Node* r){ return new OrNode(l, r); })},
        {"&&", std::make_tuple(40, LEFT_ASSOC, [](Node* l, Node* r){ return new AndNode(l, r); })},
        {"!=", std::make_tuple(41, LEFT_ASSOC, [](Node* l, Node* r){ return new NotEqualNode(l, r); })},
        {"==", std::make_tuple(41, LEFT_ASSOC, [](Node* l, Node* r){ return new EqualNode(l, r); })},
        {"<", std::make_tuple(42, LEFT_ASSOC, [](Node* l, Node* r){ return new LowerNode(l, r); })},
        {">", std::make_tuple(42, LEFT_ASSOC, [](Node* l, Node* r){ return new GreaterNode(l, r); })},
        {"<=", std::make_tuple(42, LEFT_ASSOC, [](Node* l, Node* r){ return new LowerEqualNode(l, r); })},
        {">=", std::make_tuple(42, LEFT_ASSOC, [](Node* l, Node* r){ return new GreaterEqualNode(l, r); })},
        {"+", std::make_tuple( 50, LEFT_ASSOC, [](Node* l, Node* r){ return new AddNode(l, r); })},
        {"-", std::make_tuple( 50, LEFT_ASSOC, [](Node* l, Node* r){ return new SubNode(l, r); })},
        {"*", std::make_tuple(100, LEFT_ASSOC, [](Node* l, Node* r){ return new MulNode(l, r); })},
        {"/", std::make_tuple(100, LEFT_ASSOC, [](Node* l, Node* r){ return new DivNode(l, r); })},
        {"%", std::make_tuple(100, LEFT_ASSOC, [](Node* l, Node* r){ return new ModNode(l, r); })}
    };

    Lexer *lexer;

    Parser(Lexer *lexer){
        this->lexer = lexer;
        next();
    }

    Node* parse(){
        return parseBlock();
    }

    Node* parseBlock(){
        std::vector<Node*> block;
        if (isNot(LINE_BREAK)){
            block.push_back(parseExpression());
        }
        while (!ended()){
            parse(LINE_BREAK);
            if (isNot(LINE_BREAK)){
                block.push_back(parseExpression());
            }
        }
        return new BlockNode(block);
    }

    Node* parseExpression(){
        return parseBinOpExpression();
    }

    Node* parseBinOpExpression(){
        Node* first = parseAtom();
        return parseBinOpExpression(first, 0);
    }

    Node* parseBinOpExpression(Node* left, size_t minPrecedence){
        while (isBinOp(current()) && binOpPrecedence(current()) >= minPrecedence){
            auto op = *current();
            next();
            auto *right = parseAtom();
            while (isBinOp(current()) &&
                   (binOpPrecedence(current()) > binOpPrecedence(&op)
                    || (binOpPrecedence(current()) == binOpPrecedence(&op)
                        && isRightAssocBinOp(current()))) ){
                right = parseBinOpExpression(right, binOpPrecedence(current()));
            }
            left = createBinOpNode(&op, left, right);
        }
        return left;
    }

    bool isRightAssocBinOp(Token *op){
        return isBinOp(op) && std::get<1>(getBinOp(op)) == Associativity::RIGHT_ASSOC;
    }

    Node* createBinOpNode(Token *op, Node* left, Node* right){
        return std::get<2>(getBinOp(op))(left, right);
    }

    size_t binOpPrecedence(Token *op){
         return std::get<0>(getBinOp(op));
    }

    binOp getBinOp(Token *op){
        return binaryOperators[tokenTypeToString(op->type)];
    }

    bool isBinOp(Token *op){
        std::string key = tokenTypeToString(op->type);
        return binaryOperators.find(key) != binaryOperators.end();
    }


    Node* parseAtom(){
        if (is(INT)){
            return parseInt();
        } else if (is(BOOLEAN)){
            return parseBoolean();
        } else if (is(STRING)){
            return parseString();
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
        next();
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
