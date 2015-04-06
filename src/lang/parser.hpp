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
        {"==", std::make_tuple(41, LEFT_ASSOC, _funcInfixOperatorNode("equal?") )},
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
        } else if (is(LEFT_ANGLE_BRACKET)){
            node = parseArrayLiteral();
        } else if (is(LEFT_CURLY_BRACKET)){
            node = parseMapLiteral(context());
        } else if (is(FUNCTION)){
            next();
            node = parseFunction();
        } else {
            std::ostringstream stream;
            stream << "Unexpected token " << current()->str();
            stream << " expected atom expression instead";
            error(stream.str());
        }
        std::vector<ArgumentedToken<std::string>*> postfixOps;
        for (auto &pre : prefixOps){
            node = prefixOperators[pre->argument](pre->context, node);
        }
        bool somethingChanged = true;
        while (somethingChanged){
            somethingChanged = false;
            if (is(LEFT_BRACE)){
                node = parseCall(node);
                somethingChanged = true;
            } else if (is(DOT)){
                node = parseVarAccessViaDot(node);
                somethingChanged = true;
            } else if (is(LEFT_ANGLE_BRACKET)){
                node = parseVarAccessViaBrackets(node);
                somethingChanged = true;
            }
        }
        while (isPostfixOperator(current())){
            postfixOps.push_back((ArgumentedToken<std::string>*)current());
            next();
        }
        for (auto &pre : postfixOps){
            node = postfixOperators[pre->argument](pre->context, node);
        }
        return node;
    }

    Node* parseID(){
        Context con = context();
        Node *node;
        std::string name = getArgument<std::string>();
        next();
        if (name == "break"){
            node = new BreakNode(con);
        } else if (name == "continue"){
            node = new ContinueNode(con);
        } else if (name == "if"){
            node = parseIf();
        } else if (name == "while"){
            node = parseWhile();
        } else {
            node = new VariableNode(con, name);
        }
        return node;
    }

    Node *parseIf(){
        Context con = context();
        parse(LEFT_BRACE);
        Node *condition = parseExpression();
        parse(RIGHT_BRACE);
        Node *ifBlock = 0;
        Node *elseBlock = 0;
        ifBlock = parseCodeBlock();
        if (is(ID) && getArgument<std::string>() == "else"){
            next();
            elseBlock = parseBlock();
        }
        return new IfNode(con, condition, ifBlock, elseBlock);
    }

    Node *parseWhile(){
        Context con = context();
        parse(LEFT_BRACE);
        Node *condition = parseExpression();
        parse(RIGHT_BRACE);
        Node *body = parseCodeBlock();
        return new WhileNode(con, condition, body);
    }

    Node *parseCodeBlock(){
        Context con = context();
        if (is(LEFT_CURLY_BRACKET)){
            std::vector<Node*> nodes;
            parse(LEFT_CURLY_BRACKET);
            if (is(RIGHT_CURLY_BRACKET)){
                next();
                return createMapNode(con);
            }
            auto firstExpression = parseExpression();
            if (isMapKey(firstExpression)){
                if (is(COLON)){
                    //next();
                    return parseMapLiteral(con, firstExpression);
                }
            }
            nodes.push_back(firstExpression);
            while (isNot(RIGHT_CURLY_BRACKET)){
                nodes.push_back(parseExpression());
                if (is(RIGHT_CURLY_BRACKET)){
                    next();
                    break;
                } else {
                    parse(LINE_BREAK);
                }
            }
            next();
            return new BlockNode(con, nodes);
        }
        Node *node = parseExpression();
        if (!is(LINE_BREAK) && !is(END)){
            parse(LINE_BREAK);
        }
        return node;
    }

    bool isMapKey(Node*){
        return true; //node->type() == LEAF
    }

    Node *parseMapKeyNode(Node *key){
        if (key->type() == LEAF){
            auto node = static_cast<Leaf<std::string>*>(key);
            if (node->isVariableNode()){
                return new StringNode(node->context, node->value);
            }
        }
        return key;
    }

    Node *parseMapLiteral(Context context, Node *firstKey = 0){
        std::vector<Node*> arguments;
        if (firstKey == 0){
            next();
            auto expr = parseExpression();
            if (isMapKey(expr)){
                arguments.push_back(parseMapKeyNode(expr));
            } else {
                error("Expected map key but got something else");
            }
        } else {
            arguments.push_back(parseMapKeyNode(firstKey));
        }
        parse(COLON);
        arguments.push_back(parseExpression());
        while (isNot(RIGHT_CURLY_BRACKET)){
            parse(COMMA);
            auto expr = parseExpression();
            if (isMapKey(expr)){
                arguments.push_back(parseMapKeyNode(expr));
            } else {
                error("Expected map key but got something else");
            }
            parse(COLON);
            expr = parseExpression();
            arguments.push_back(expr);
        }
        next();
        return createMapNode(context, arguments);
    }

    Node *parseArrayLiteral(){
        Context con = context();
        std::vector<Node*> arguments;
        next();
        if (isNot(RIGHT_ANGLE_BRACKET)){
            arguments.push_back(parseExpression());
            while (isNot(RIGHT_ANGLE_BRACKET)){
                parse(COMMA);
                arguments.push_back(parseExpression());
            }
        }
        next();
        return createArrayNode(con, arguments);
    }

    Node *parseCall(Node *func){
        Context con = func->context;
        std::vector<Node*> arguments;
        next();
        while (isNot(RIGHT_BRACE)){
            arguments.push_back(parseExpression());
            if (is(COMMA)){
                next();
            } else {
                parse(RIGHT_BRACE);
                break;
            }
        }
        if (arguments.empty()){
            next();
        }
        return new CallNode(con, func, arguments);
    }

    Node *parseFunction(){
        Context con = context();
        std::vector<std::string> parameters;
        std::string name = "";
       if (is(ID)){
            name = getArgument<std::string>();
            next();
        }
        parse(LEFT_BRACE);
        parameters = parseFunctionParameters();
        auto body = parseCodeBlock();
        if (name.empty()){
            return new ClousureNode(con, parameters, body);
        } else {
            return new FunctionNode(con, name, parameters, body);
        }
    }

    std::vector<std::string> parseFunctionParameters(){
        std::vector<std::string> params;
        if (is(RIGHT_BRACE)){
            return params;
        }
        parse(ID, false);
        params.push_back(getArgument<std::string>());
        next();
        while (isNot(RIGHT_BRACE)){
            parse(COMMA);
            parse(ID, false);
            params.push_back(getArgument<std::string>());
            next();
        }
        next();
        return params;
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
        throw stream.str();
    }
};

}
