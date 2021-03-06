#pragma once

#include "../utils.hpp"
#include <cctype>
#include "target.hpp"

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
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    LEFT_ANGLE_BRACKET,
    RIGHT_ANGLE_BRACKET,
    OPERATOR,
    OPERATOR_FUNC,
    COMMA,
    COLON,
    END,
    BOOLEAN,
    NOTHING,
    VAR,
    FUNCTION,
    DOT,
    LINE_BREAK /** Line break or semi colon */
};

std::string tokenTypeToString(TokenType type){
    switch (type){
        case ID: return "ID";
        case LINE_BREAK: return "LINE_BREAK";
        case END: return "END";
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
        case STRING: return "STRING";
        case BOOLEAN: return "BOOLEAN";
        case NOTHING: return "nothing";
        case VAR: return "var";
        case FUNCTION: return "function";
        case OPERATOR: return "op";
        case OPERATOR_FUNC: return "\\op";
        case DOT: return ".";
    }
    return "[unknown token]";
}

struct Context {
    size_t lineNumber;
    size_t columnNumber;

    Context(size_t lineNumber, size_t columnNumber)
        : lineNumber(lineNumber), columnNumber(columnNumber) {}

    std::string str(){
        std::ostringstream stream;
        stream << lineNumber << "." << columnNumber;
        return stream.str();
    }

    void compile(Target &target){
        target.LINE_COLUMN_NUMBER(lineNumber, columnNumber);
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
        std::ostringstream stream;
        stream << tokenTypeToString(type) << "(" << context.str() << ")";
        return stream.str();
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
        stream << Token::str() << "[" << argument << "]";
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
        if (lastToken != 0){
            delete lastToken;
        }
        lastToken = _nextToken();
        return lastToken;
    }

    Token* _nextToken(){
        while (isWhiteSpace()){          // omit whitespace
            next();
        }
        if (ended()){                    // end of stream
            return token(TokenType::END, context());
        }
        if (isLetter() || is('_')){
            return parseID();
        }
        if (isDigit()){
            return parseNumeric();
        }
        if (isSpecialChar()){
            return parseOperator();
        }
        Context con = context();
        switch(currentChar){
            case ';':
            case '\n':
                return parseLineBreak();
            case '(':
                next();
                return token(LEFT_BRACE, con);
            case ')':
                next();
                return token(RIGHT_BRACE, con);
            case '{':
                next();
                return token(LEFT_CURLY_BRACKET, con);
            case '}':
                next();
                return token(RIGHT_CURLY_BRACKET, con);
            case '[':
                next();
                return token(LEFT_ANGLE_BRACKET, con);
            case ']':
                next();
                return token(RIGHT_ANGLE_BRACKET, con);
            case ',':
                next();
                return token(COMMA, con);
            case ':':
                next();
                return token(COLON, con);
            case '.':
                next();
                return token(DOT, con);
            case '#':
                omitRestOfLine();
                return nextToken();
            case '\\':
                {
                    next();
                    Token *tokenObj = parseID();
                    return token(OPERATOR_FUNC, tokenObj->context,
                                 static_cast<ArgumentedToken<std::string>*>(tokenObj)->argument);
                }
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
        Context con = context();
        std::ostringstream stream;
        stream << currentChar;
        next();
        while (isAlphaNumeric() || is('_') || is('$') || is('?') || is('!')){
            stream << currentChar;
            next();
        }
        auto str = stream.str();
        if (str == std::string("nothing")){
            return token(NOTHING, con);
        } else if (str == std::string("true")){
            return token(BOOLEAN, con, true);
        } else if (str == std::string("false")){
            return token(BOOLEAN, con, false);
        } else if (str == std::string("function")){
            return token(FUNCTION, con);
        } else if (str == std::string("var")){
            return token(VAR, con);
        }
        return token(ID, con, stream.str());
    }

    Token* parseString(char delimiter){
        Context con = context();
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
        return token(STRING, con, str);
    }

    bool isFirstToken(){
        return lastToken == 0;
    }

    Token* parseNumeric(){
        Context con = context();
        std::stringstream stream;
        while (isDigit()){
            stream << currentChar;
            next();
        }
        if (is('.')){
            stream << '.';
            next();
            while (isDigit()){
                stream << currentChar;
                next();
            }
            float_type f;
            stream >> f;
            return token(FLOAT, con, f);
        } else {
            int_type integer;
            stream >> integer;
            return token(INT, con, integer);
        }
    }

    Token* parseLineBreak(){
        Context con = context();
        next();
        if (is('\r')) next();
        return token(TokenType::LINE_BREAK, con);
    }

    Token* parseOperator(){
        Context con = context();
        std::ostringstream stream;
        while(isSpecialChar()){
            stream << currentChar;
            next();
        }
        return token(OPERATOR, con, stream.str());
    }

    Token* token(TokenType type, Context context){
        return new Token(type, context);
    }

    template<typename T>
    ArgumentedToken<T>* token(TokenType type, Context context, T argument){
        return new ArgumentedToken<T>(type, context, argument);
    }

    bool is(char expected){
        return expected == currentChar;
    }

    bool is(std::vector<char> expectable){
        return std::find(expectable.begin(), expectable.end(),
                         currentChar) != expectable.end();
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

    bool isSpecialChar(){
        return is({'<', '>', '|', '-', '+', '~', '*', '`',
                   '?', '=', '/', '&', '%', '$',
                   '!', '^'});
    }

    bool ended(){
        return currentChar == 0;
    }

    Context context(){
        return Context(lineNumber, columnNumber);
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
        //std::cerr << stream.str() << "\n";
        throw stream.str();
    }

};

}
