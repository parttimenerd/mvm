#pragma once

#include "../utils.hpp"
#include <cctype>

/*void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()){
        return;
    }
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}*/

namespace lang {

enum TokenType {
    ID,
    INT,
    FLOAT,
    STRING,
    EQUAL_SIGN,
    DOUBLE_EQUAL_SIGN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    LEFT_ANGLE_BRACKET,
    RIGHT_ANGLE_BRACKET,
    COMMA,
    COLON,
    PLUS,
    LEFT_DOUBLE_PLUS,
    RIGHT_DOUBLE_PLUS,
    MINUS,
    LEFT_DOUBLE_MINUS,
    RIGHT_DOUBLE_MINUS,
    DIVIDE_SIGN,
    MULTIPLY_SIGN,
    MOD_SIGN,
    NEGATE,
    NOT,
    NOT_EQUAL,
    AND,
    OR,
    END,
    BOOLEAN,
    NOTHING,
    VAR,
    FUNCTION,
    LINE_BREAK /** Line break or semi colon */
};

std::string lineTypeToString(TokenType type){
    switch (type){
        case ID: return "ID";
        case LINE_BREAK: return "LINE_BREAK";
        case END: return "END";
        case EQUAL_SIGN: return "=";
        case DOUBLE_EQUAL_SIGN: return "==";
        case LEFT_BRACE: return "(";
        case RIGHT_BRACE: return ")";
        case LEFT_CURLY_BRACKET: return "{";
        case RIGHT_CURLY_BRACKET: return "}";
        case LEFT_ANGLE_BRACKET: return "[";
        case RIGHT_ANGLE_BRACKET: return "]";
        case COMMA: return ",";
        case COLON: return ":";
        case INT: return "INT";
        case FLOAT: return "FLOAT";
        case DIVIDE_SIGN: return "/";
        case MULTIPLY_SIGN: return "*";
        case MOD_SIGN: return "%";
        case PLUS: return "+";
        case MINUS: return "-";
        case STRING: return "STRING";
        case LEFT_DOUBLE_MINUS: return "--b";
        case LEFT_DOUBLE_PLUS: return "++b";
        case RIGHT_DOUBLE_MINUS: return "b--";
        case RIGHT_DOUBLE_PLUS: return "b++";
        case NEGATE: return "NEGATE";
        case NOT: return "!";
        case NOT_EQUAL: return "!=";
        case AND: return "&&";
        case OR: return "||";
        case BOOLEAN: return "BOOLEAN";
        case NOTHING: return "nothing";
        case VAR: return "var";
        case FUNCTION: return "function";
    }
    return "[unknown token]";
}

struct Context {
    size_t lineNumber;

    Context(size_t lineNumber){
        this->lineNumber = lineNumber;
    }

    std::string str(){
        std::ostringstream stream;
        stream << lineNumber;
        return stream.str();
    }
};

struct Token {
    TokenType type;
    Context context;

    Token(TokenType type, Context context) : context(context){
        this->type = type;
        //this->context = context;
    }

    virtual std::string str(){
        return lineTypeToString(type);
    }

    virtual ~Token() = default;
};

template<typename T>
struct ArgumentedToken : Token {
    T argument;

    ArgumentedToken(TokenType type, Context context, T argument) : Token(type, context){
        this->argument = argument;
    }
    std::string str(){
        std::ostringstream stream;
        stream << lineTypeToString(type) << "[" << argument << "]";
        return stream.str();
    }
};

/**
 * @brief A simple lexer
 *
 * Omits whitespace. Supports commented lines with "#" or "//" and multiline strings
 */
struct Lexer {

    std::istream *input;
    char currentChar;
    size_t lineNumber = 1;
    size_t columnNumber = 0;
    Token *lastToken = 0;
    bool wsAfterLastToken = false;

    Lexer(std::istream *input){
        this->input = input;
        next();
    }

    char next(){
        if (!input->get(currentChar)){
            currentChar = 0;
            return 0;
        }
        if (currentChar == '\n'){
            lineNumber++;
            columnNumber = 0;
        } else {
            columnNumber++;
        }
        return currentChar;
    }

    Token* nextToken(){
        if (lastToken != 0){
            delete lastToken;
        }
        lastToken = _nextToken();
        wsAfterLastToken = false;
        return lastToken;
    }

    Token* _nextToken(){
        while (isWhiteSpace()){          // omit whitespace
            next();
            wsAfterLastToken = true;
        }
        if (ended()){                    // end of stream
            return token(TokenType::END);
        }
        if (isLetter() || is('_') || is('$') || is('@')){
            return parseID();
        }
        if (isDigit() || isSign()){
            return parseNumericAndSign();
        }
        switch(currentChar){
            case ';':
            case '\n':
                return parseLineBreak();
            case '=':
                return parseEqualSigns();
            case '(':
                next();
                return token(LEFT_BRACE);
            case ')':
                next();
                return token(RIGHT_BRACE);
            case '{':
                next();
                return token(LEFT_CURLY_BRACKET);
            case '}':
                next();
                return token(RIGHT_CURLY_BRACKET);
            case '[':
                next();
                return token(LEFT_ANGLE_BRACKET);
            case ']':
                next();
                return token(RIGHT_ANGLE_BRACKET);
            case ',':
                next();
                return token(COMMA);
            case ':':
                next();
                return token(COLON);
            case '/':
                next();
                if (is('/')){
                    omitRestOfLine(); // omit comments
                    return nextToken();
                }
                return token(DIVIDE_SIGN);
            case '#':
                omitRestOfLine();
                return nextToken();
            case '*':
                next();
                return token(MULTIPLY_SIGN);
            case '%':
                next();
                return token(MOD_SIGN);
            case '"':
            case '\'':
                return parseString(currentChar);
            case '!':
                next();
                if (is('=')){
                    next();
                    return token(NOT_EQUAL);
                }
                return token(NOT);
            case '|':
                next();
                if (is('|')){
                    next();
                    return token(OR);
                }
                error("Don't know what to do with char ", currentChar);
            case '&':
                next();
                if (is('&')){
                    next();
                    return token(AND);
                }
                error("Don't know what to do with char ", currentChar);
            default:
                error("Don't know what to do with char ", currentChar);
        }
        return 0;
    }

    void omitRestOfLine(){
        while (isNot('\n')) next();
    }

    Token* parseID(){
        std::ostringstream stream;
        stream << currentChar;
        next();
        while (isAlphaNumeric() || is('_') || is('$') || is('?') || is('@')){
            stream << currentChar;
            next();
        }
        auto str = stream.str();
        if (str == std::string("nothing")){
            return token(NOTHING);
        } else if (str == std::string("true")){
            return token(BOOLEAN, true);
        } else if (str == std::string("false")){
            return token(BOOLEAN, false);
        } else if (str == std::string("function")){
            return token(FUNCTION);
        } else if (str == std::string("var")){
            return token(VAR);
        }
        return token(ID, stream.str());
    }

    Token* parseString(char delimiter){
        std::ostringstream stream;
        next();
        while (currentChar != delimiter){
            stream << currentChar;
            next();
        }
        next();
        std::string str = stream.str();
        std::vector<std::string> replacements = {
            "\\t", "\t",
            "\\n", "\n",
            "\\r", "\r",
            "\\'", "'",
            "\\\"", "\""
        };
        for (size_t i = 0; i < replacements.size(); i += 2){
            replaceAll(str, replacements[i], replacements[i + 1]);
        }
        return token(STRING, str);
    }

    bool lastTokenWasExpression(){
        auto t = lastToken->type;
        return t == ID || t == RIGHT_BRACE || t == RIGHT_ANGLE_BRACKET ||
               t == INT || t == FLOAT || t == STRING;
    }

    bool isFirstToken(){
        return lastToken == 0;
    }

    Token* parseNumericAndSign(){
        bool minus = false;
        if (is('-')){
            if (!isFirstToken() && lastTokenWasExpression()){ //e.g. a - b
                next();
                if (is('-') && !wsAfterLastToken){ //e.g. a-- b
                    next();
                    return token(RIGHT_DOUBLE_MINUS);
                } else {
                    return token(MINUS); // the '-' seems to be a 'real' subtraction sign
                }
            } else {
                next();
                if (is('-')){ //e.g. a, -- b
                    next();
                    return token(LEFT_DOUBLE_MINUS);
                }
                minus = true;
            }
        } else if (is('+')){
            if (!isFirstToken() && lastTokenWasExpression()){ //e.g. a + b
                next();
                if (is('+') && !wsAfterLastToken){ //e.g. a ++ b
                    next();
                    return token(RIGHT_DOUBLE_PLUS);
                } else {
                    return token(PLUS);
                }
            } else { //e.g. a, +b
                next();
                if (is('+')){ //e.g. a, ++b
                    next();
                    if (!isWhiteSpace()){
                        return token(LEFT_DOUBLE_PLUS);
                    } else {
                        error("Didn't expect whitespace here, expected start of expression");
                    }
                } else {
                    return token(PLUS);
                }
            }
        }
        if (!isDigit()){
            return token(NEGATE);
        }
        int_type res = 0;
        while (isDigit()){
            res = res * 10 + currentChar - '0';
            next();
        }
        if (minus){
            res = -res;
        }
        if (is('.')){
            error("Floats are currently not supported");
        }
        return token(INT, res);
    }

    Token* parseLineBreak(){
        next();
        if (is('\r')) next();
        return token(TokenType::LINE_BREAK);
    }

    Token* parseEqualSigns(){
        if (is('=')){
            next();
            return token(DOUBLE_EQUAL_SIGN);
        }
        return token(EQUAL_SIGN);
    }

    Token* token(TokenType type){
        return new Token(type, context());
    }

    template<typename T>
    ArgumentedToken<T>* token(TokenType type, T argument){
        return new ArgumentedToken<T>(type, context(), argument);
    }

    bool is(char expected){
        return expected == currentChar;
    }

    bool isNot(char expected){
        return expected != currentChar;
    }

    /**
     * @brief current char \in [begin, end] ?
     */
    bool inRange(char begin, char end){
        return begin <= currentChar && currentChar <= end;
    }

    /**
     * @brief current char \notin [begin, end] ?
     */
    bool notInRange(char begin, char end){
        return begin > currentChar || currentChar > end;
    }

    bool isWhiteSpace(){
        return is(' ') || is('\t') || is('\r');
    }

    bool isLetter(){
        return isalpha(currentChar);
    }

    bool isDigit(){
        return isdigit(currentChar);
    }

    bool isAlphaNumeric(){
        return isalnum(currentChar);
    }

    bool isSign(){
        return is('+') || is('-');
    }

    bool ended(){
        return currentChar == 0;
    }

    Context context(){
        return Context(lineNumber);
    }

    template<typename T>
    T strToNum(std::string str){
        T number;
        if (!(std::istringstream(str) >> number)) {
            error("Expected Number, got ", str);
        }
        return number;
    }

    void error(std::string msg, char msg1){
        std::ostringstream stream;
        char arr[2];
        arr[0] = msg1;
        arr[1] = '\0';
        stream << std::string(arr) << " (" << (unsigned int)msg1 << ")";
        error(msg, stream.str());
    }

    void error(std::string msg, std::string msg1 = ""){
        std::ostringstream stream;
        stream << "Error in line " << context().str()
               << ", column " << columnNumber << ": " << msg << msg1;
        std::cerr << stream.str() << "\n";
        throw stream.str();
    }

};

}
