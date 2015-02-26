#pragma once

#include "utils.hpp"
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
    MINUS,
    DIVIDE_SIGN,
    MULTIPLY_SIGN,
    MOD_SIGN,
    END,
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
    }
    return "[unknown token]";
}

struct Token {
    TokenType type;
    size_t lineNumber;

    Token(TokenType type, size_t lineNumber){
        this->type = type;
        this->lineNumber = lineNumber;
    }

    virtual std::string str(){
        return lineTypeToString(type);
    }

    virtual ~Token() = default;
};

template<typename T>
struct ArgumentedToken : Token {
    T argument;

    ArgumentedToken(TokenType type, size_t lineNumber, T argument) : Token(type, lineNumber){
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
        lastToken = _nextToken();
        return lastToken;
    }

    Token* _nextToken(){
        while (isWhiteSpace()){          // omit whitespace
            next();
        }
        if (ended()){                    // end of stream
            return token(TokenType::END);
        }
        if (isLetter() || is('_') || is('$')){
            return parseID();
        }
        if (isDigit() || is('-')){
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
            case '+':
                next();
                return token(PLUS);
            case '%':
                next();
                return token(MOD_SIGN);
            case '"':
            case '\'':
                return parseString(currentChar);
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
        while (isAlphaNumeric() || is('_') || is('$') || is('?')){
            stream << currentChar;
            next();
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
            "\\\t", "\t",
            "\\\n", "\n",
            "\\\r", "\r",
            "\\'", "'",
            "\\\"", "\""
        };
        for (size_t i = 0; i < replacements.size(); i += 2){
            replaceAll(str, replacements[i], replacements[i + 1]);
        }
        return token(STRING, str);
    }

    Token* parseNumericAndSign(){
        bool minus = false;
        if (is('-')){
            auto t = lastToken->type;
            if (t == ID || t == RIGHT_BRACE || t == RIGHT_ANGLE_BRACKET ||
                    t == INT || t == FLOAT || t == STRING){
                next();
                return token(MINUS); // the '-' seems to be a 'real' subtraction sign
            } else {
                next();
                minus = true;
            }
        }
        if (!isDigit()){
            error("Don't know what to do with char", currentChar);
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

    size_t context(){
        return lineNumber;
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
        stream << msg1 << " (" << (unsigned int)msg1 << ")";
        error(msg, stream.str());
    }

    void error(std::string msg, std::string msg1 = ""){
        std::ostringstream stream;
        stream << "Error in line " << lineNumber
               << ", column " << columnNumber << ": " << msg << msg1;
        std::cerr << stream.str() << "\n";
        throw stream.str();
    }

};
