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

    typedef std::tuple<size_t, Associativity, std::function<Node*(Node*, Node*)>> infixOp;

    /**
     * Contains tuples (priority/precedence, associativity, function to create a node) for
     * each operator
     *
     * @see https://en.wikipedia.org/wiki/Order_of_operations
     */
    std::unordered_map<std::string, infixOp>  infixOperators; /*= {
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
    }*/

    typedef std::function<Node*(Node*)> unaryOp;

    std::unordered_map<std::string, unaryOp>  prefixOperators /*= {
        //{"!", [](Node* l, Node* r){ return new OrNode(l, r); }}
    }*/;

    std::unordered_map<std::string, unaryOp>  postfixOperators /*= {
        //{"!", [](Node* l, Node* r){ return new OrNode(l, r); }}
    }*/;

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
        return new BlockNode(block.empty() ? Context(0, 0) : block.at(0)->context, block);
    }

    Node* parseExpression(){
        return parseInfixOpExpression();
    }

    Node* parseInfixOpExpression(){
        Node* first = parseAtom();
        return parseInfixOpExpression(first, 0);
    }

    Node* parseInfixOpExpression(Node* left, size_t minPrecedence){
        while (isInfixOp(current()) && infixOpPrecedence(current()) >= minPrecedence){
            auto op = *current();
            next();
            auto *right = parseAtom();
            while (isInfixOp(current()) &&
                   (infixOpPrecedence(current()) > infixOpPrecedence(&op)
                    || (infixOpPrecedence(current()) == infixOpPrecedence(&op)
                        && isRightAssocInfixOp(current()))) ){
                right = parseInfixOpExpression(right, infixOpPrecedence(current()));
            }
            left = createInfixOpNode(&op, left, right);
        }
        return left;
    }

    bool isRightAssocInfixOp(Token *op){
        return isInfixOp(op) && std::get<1>(getInfixOp(op)) == Associativity::RIGHT_ASSOC;
    }

    Node* createInfixOpNode(Token *op, Node* left, Node* right){
        return std::get<2>(getInfixOp(op))(left, right);
    }

    size_t infixOpPrecedence(Token *op){
         return std::get<0>(getInfixOp(op));
    }

    infixOp getInfixOp(Token *op){
        return infixOperators[getArgument<std::string>(op)];
    }

    bool isInfixOp(Token *op){
        if (op->type == OPERATOR){
            std::string key = getArgument<std::string>(op);
            return infixOperators.find(key) != infixOperators.end();
        }
        return false;
    }

    bool isPrefixOperator(Token *op){
        if (op->type == OPERATOR){
            std::string key = getArgument<std::string>(op);
            return prefixOperators.find(key) != prefixOperators.end();
        }
        return false;
    }

    bool isPostfixOperator(Token *op){
        if (op->type == OPERATOR){
            std::string key = getArgument<std::string>(op);
            return postfixOperators.find(key) != postfixOperators.end();
        }
        return false;
    }


    Node* parseAtom(){
        std::vector<std::string> prefixOps;
        while (isPrefixOperator(current())){
            prefixOps.push_back(getArgument<std::string>());
            next();
        }
        Node *node;
        if (is(INT)){
            node = parseInt();
        } else if (is(BOOLEAN)){
            node = parseBoolean();
        } else if (is(STRING)){
            node = parseString();
        } else if (is(NOTHING)){
            node = parseNothing();
        } else if (is(LEFT_BRACE)){
            node = parseBraced();
        } else if (is(ID)){
            node = parseIDandCall();
        } else {
            std::ostringstream stream;
            stream << "Unexpected token " << current()->str();
            stream << " expected atom expression instead";
            error(stream.str());
        }
        std::vector<std::string> postfixOps;
        while (isPostfixOperator(current())){
            postfixOps.push_back(getArgument<std::string>());
            next();
        }
        for (auto &pre : prefixOps){
            node = prefixOperators[pre](node);
        }
        for (auto &pre : postfixOps){
            node = postfixOperators[pre](node);
        }
        return 0;
    }

    Node* parseIDandCall(){
     /*   std::string id = getArgument<std::string>();
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
        return new CallNode(id, arguments);*/
        return new Node(Context(0, 0));
    }

    Node* parseInt(){
        Node *ret = new IntNode(context(), getArgument<int_type>());
        next();
        return ret;
    }

    Node* parseFloat(){
        Node *ret = new FloatNode(context(), getArgument<float_type>());
        next();
        return ret;
    }

    Node* parseString(){
        Node *ret = new StringNode(context(), getArgument<std::string>());
        next();
        return ret;
    }

    Node* parseBoolean(){
        Node *ret = new BooleanNode(context(), getArgument<bool>());
        next();
        return ret;
    }

    Node* parseNothing(){
        Node *ret = new NothingNode(context());
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

    template<typename T>
    T getArgument(Token *token){
        return static_cast<ArgumentedToken<T>*>(token)->argument;
    }

    Token* next(){
        Token *nextToken = lexer->nextToken();
        //std::cout << "next token " << nextToken->str() << "\n";
        return nextToken;
    }

    Token* current(){
        return lexer->lastToken;
    }

    Context context(){
        return current()->context;
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
