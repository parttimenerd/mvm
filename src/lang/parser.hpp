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

    typedef std::function<Node*(Context, Node*, Node*)> infixOpFunc;
    typedef std::tuple<size_t, Associativity, infixOpFunc> infixOp;

    /**
     * Contains tuples (priority/precedence, associativity, function to create a node) for
     * each operator
     *
     * @see https://en.wikipedia.org/wiki/Order_of_operations
     */
    std::unordered_map<std::string, infixOp>  infixOperators = {
        {"=", std::make_tuple(20, LEFT_ASSOC, [](Context c, Node *l, Node *r){ return new SetVarNode(c, l, r); })},
        {":=", std::make_tuple(20, LEFT_ASSOC, _funcInfixOperatorNode("set_direct"))},
        {"||", std::make_tuple(30, LEFT_ASSOC, [](Context c, Node *l, Node *r){ return new OrNode(c, l, r); })},
        {"&&", std::make_tuple(40, LEFT_ASSOC, [](Context c, Node *l, Node *r){ return new AndNode(c, l, r); })},
        {"!=", std::make_tuple(41, LEFT_ASSOC, _funcInfixOperatorNode("not_equal?"))},
        {"==", std::make_tuple(41, LEFT_ASSOC, _funcInfixOperatorNode("eq?") )},
        {"<", std::make_tuple(42, LEFT_ASSOC, _funcInfixOperatorNode("less?"))},
        {">", std::make_tuple(42, LEFT_ASSOC, _funcInfixOperatorNode("greater?"))},
        {"<=", std::make_tuple(42, LEFT_ASSOC, _funcInfixOperatorNode("greater_equal?"))},
        {">=", std::make_tuple(42, LEFT_ASSOC, _funcInfixOperatorNode("greater_equal?"))},
        {"+", std::make_tuple( 50, LEFT_ASSOC, _funcInfixOperatorNode("plus"))},
        {"-", std::make_tuple( 50, LEFT_ASSOC, _funcInfixOperatorNode("minus"))},
        {"*", std::make_tuple(100, LEFT_ASSOC, _funcInfixOperatorNode("multiply"))},
        {"/", std::make_tuple(100, LEFT_ASSOC, _funcInfixOperatorNode("divide"))},
        {"%", std::make_tuple(100, LEFT_ASSOC, _funcInfixOperatorNode("modulo"))},
        {"**", std::make_tuple(100, RIGHT_ASSOC, _funcInfixOperatorNode("power"))}
    };

    infixOpFunc _funcInfixOperatorNode(std::string name){
        return [name](Context context, Node *left, Node *right){
            return new FuncInfixOperator(context, left, right, name);
        };
    }

    typedef std::function<Node*(Context, Node*)> unaryOp;

    std::unordered_map<std::string, unaryOp>  prefixOperators = {
        {"!", [](Context c, Node* r){ return new UnaryFuncNode(c, r, "not"); }}
    };

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
            auto op = new ArgumentedToken<std::string>(TokenType::OPERATOR, current()->context, getArgument<std::string>(current()));
            next();
            auto *right = parseAtom();
            while (isInfixOp(current()) &&
                   (infixOpPrecedence(current()) > infixOpPrecedence(op)
                    || (infixOpPrecedence(current()) == infixOpPrecedence(op)
                        && isRightAssocInfixOp(current()))) ){
                right = parseInfixOpExpression(right, infixOpPrecedence(current()));
            }
            left = createInfixOpNode(op, left, right);
        }
        return left;
    }

    bool isRightAssocInfixOp(Token *op){
        return isInfixOp(op) && std::get<1>(getInfixOp(op)) == Associativity::RIGHT_ASSOC;
    }

    Node* createInfixOpNode(Token *op, Node* left, Node* right){
        return std::get<2>(getInfixOp(op))(op->context, left, right);
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
        std::vector<ArgumentedToken<std::string>*> prefixOps;
        while (isPrefixOperator(current())){
            prefixOps.push_back((ArgumentedToken<std::string>*)current());
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
            node = parseID();
        } else if (is(VAR)){
            node = parseVar();
        }  else {
            std::ostringstream stream;
            stream << "Unexpected token " << current()->str();
            stream << " expected atom expression instead";
            error(stream.str());
        }
        std::vector<ArgumentedToken<std::string>*> postfixOps;
        while (isPostfixOperator(current())){
            postfixOps.push_back((ArgumentedToken<std::string>*)current());
            next();
        }
        for (auto &pre : prefixOps){
            node = prefixOperators[pre->argument](pre->context, node);
        }
        for (auto &pre : postfixOps){
            node = postfixOperators[pre->argument](pre->context, node);
        }
        if (is(LEFT_BRACE)){
            node = parseCall(node);
        } else if (is(DOT)){
            node = parseVarAccessViaDot(node);
        } else if (is(LEFT_ANGLE_BRACKET)){
            node = parseVarAccessViaBrackets(node);
        }
        return node;
    }

    Node* parseID(){
       Context con = context();
       Node *node = new PushVarNode(con, getArgument<std::string>());
       next();
       return node;
    }

    Node* parseCall(Node *func){
        Context con = func->context;
        std::vector<Node*> arguments;
        next();
        while (isNot(RIGHT_BRACE)){
            arguments.push_back(parseExpression());
            if (is(COMMA)){
                next();
            } else {
                parse(RIGHT_BRACE);
            }
        }
        next();
        return new CallNode(con, func, arguments);
    }

    Node* parseVar(){
        Context con = context();
        next();
        parse(ID, false);
        std::string name = getArgument<std::string>();
        next();
        return new InitVarNode(con, name);
    }

    Node* parseVarAccessViaDot(Node *map){
        Context con = context();
        next();
        parse(ID, false);
        std::string name = getArgument<std::string>();
        next();
        return new VarAccessViaDotOperator(con, map, name);
    }

    Node* parseVarAccessViaBrackets(Node *map){
        Context con = context();
        next();
        auto arg = parseExpression();
        parse(RIGHT_ANGLE_BRACKET);
        return new FuncInfixOperator(con, map, arg, "get");
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

    void parse(TokenType expectedType, bool callNext = true){
        if (is(expectedType)){
            if (callNext){
                next();
            }
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
        //std::cerr << stream.str() << "\n";
        throw stream.str();
    }
};

}
