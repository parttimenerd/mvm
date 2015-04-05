#pragma once

#include "utils.hpp"
#include <istream>

#define L std::cout << __LINE__ << "\n";

enum class LineType : uint8_t {
    CALL,
    CALL_N,
    CALL_N2,
    PUSH_NOTHING,
    PUSH_BOOLEAN,
    PUSH_INT,
    PUSH_STRING,
    PUSH_ARRAY,
    PUSH_MAP,
    PUSH_VAR,
    SET_VAR,
    INIT_VAR,
    POP,
    DUP,
    JUMP_IF,
    JUMP,
    PUSH_SCOPE,
    POP_SCOPE,
    RETURN,
    ERROR,
    COMMENT,
    NOP,
    LINE_COLUMN_NUMBER,
    PRINT_STACK,
    ASSERT_STACK_HEIGHT
};

static std::vector<std::string> type_names = {
    "CALL",
    "CALL_N",
    "CALL_N2",
    "PUSH_NOTHING",
    "PUSH_BOOLEAN",
    "PUSH_INT",
    "PUSH_STRING",
    "PUSH_ARRAY",
    "PUSH_MAP",
    "PUSH_VAR",
    "SET_VAR",
    "INIT_VAR",
    "POP",
    "DUP",
    "JUMP_IF",
    "JUMP",
    "PUSH_SCOPE",
    "POP_SCOPE",
    "RETURN",
    "ERROR",
    "COMMENT",
    "NOP",
    "LINE_COLUMN_NUMBER",
    "PRINT_STACK",
    "ASSERT_STACK_HEIGHT"
};

static std::string lineTypeToString(LineType type){
    return type_names[(uint8_t)type];
}

static LineType uintToLineType(uint8_t type){
    return static_cast<LineType>(type);
}

static LineType stringToLineType(std::string type){
    auto iter = std::find(type_names.begin(), type_names.end(), type);
    size_t index = std::distance(type_names.begin(), iter);
    if (index == type_names.size()){
        return LineType::ERROR;
    } else {
        return uintToLineType((uint8_t)index);
    }
}

static void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()){
        return;
    }
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

static std::vector<std::string> files;

static size_t getIDofFile(std::string fileName){
    for (size_t i = 0; i < files.size(); i++){
        if (fileName == files[i]){
            return i;
        }
    }
    size_t id = files.size();
    files.push_back(fileName);
    return id;
}

struct LangContext {
    size_t fileID;
    size_t lineNumber;
    size_t columnNumber;

    LangContext(size_t fileID, size_t lineNumber, size_t columnNumber)
        : fileID(fileID), lineNumber(lineNumber), columnNumber(columnNumber) {}

    std::string str(){
        std::ostringstream stream;
        stream << files[fileID] << "[" << lineNumber << ":" << columnNumber << "]";
        return stream.str();
    }
};

/**
 * A line of code
 */
struct Line {
    LangContext context;
	LineType type;

    Line(LangContext context, LineType type) : context(context), type(type) {}

    virtual std::string str(){
        return lineTypeToString(type) + "["s + context.str() + "]"s;
    }

    std::string typeString(){
        return lineTypeToString(type);
    }

    virtual ~Line() = default;
};

/**
 * A line of code with one additional argument, e.g. CALL_N
 */
template<typename T>
struct ArgumentedLine : Line {
    T argument;

    ArgumentedLine(LangContext context, LineType type, T argument)
        : Line(context, type), argument(argument) {}

    std::string str(){
        std::ostringstream stream;
        stream << lineTypeToString(type) << "[" << context.str() << "] " << argument;
        return stream.str();
    }
};

/**
 * A parser for parsing a stream as a sequence of lines.
 */
struct Parser {

    std::istream *stream;
    size_t file_id;
    std::string current_line;
    size_t line_number = 0;
    size_t lang_line_number = 0;
    size_t lang_column_number = 0;
    bool ended = false;

    Parser(std::istream *stream, std::string file_name = "std::cin") {
        this->stream = stream;
        this->file_id = getIDofFile(file_name);
    }


    void error(std::string msg, std::string msg1 = ""){
        std::ostringstream stream;
        stream << "Error in line " << line_number << "[" << context().str() << "]"
               << ": " << msg << msg1;
        std::cerr << stream.str() << "\n";
        throw stream.str();
    }

    LangContext context(){
        return LangContext(file_id, lang_line_number, lang_column_number);
    }

    virtual std::vector<Line*> lines(){
        std::vector<Line*> vec;
        while (!ended){
            Line *line = nextCodeLine();
            if (line != 0){
                vec.push_back(line);
            }
        }

        return vec;
    }

    virtual Line* nextCodeLine() = 0;
};

/**
 * A parser for parsing verbose code (line format: `COMMAND\n` or `COMMAND ARGUMENT\n`).
 * Use this parser for development purposes.
 */
struct VerboseParser : Parser {

    std::unordered_map<std::string, size_t> labels;

    VerboseParser(std::istream *stream, std::string context = "")
        : Parser(stream, context) {}

    std::vector<Line*> lines(){
        std::vector<Line*> vec;
        while (!ended){
            Line *line = nextCodeLine();
            if (line != 0){
                vec.push_back(line);
            }
        }
        replaceLabels(&vec);
        return vec;
    }

    void replaceLabels(std::vector<Line*> *vec){
        for (size_t i = 0; i < vec->size(); i++){
            Line *elem = (*vec)[i];
            if (elem->type == LineType::JUMP_IF || elem->type == LineType::JUMP){
                ArgumentedLine<std::string>* line = (ArgumentedLine<std::string>*)elem;
                size_t target = getLabel(line->argument);
                delete line;
                (*vec)[i] = new ArgumentedLine<size_t>(context(), elem->type, target);
            }
        }
    }

    std::string line(){
        return current_line;
    }

    std::string nextLine(){
        std::getline(*stream, current_line);
        ended = !stream->good() || current_line == "eof";
        if (!ended){
            line_number++;
        }
        if (current_line.size() == 0 && !ended){
            return nextLine();
        }
        return current_line;
    }

    Line* nextCodeLine(){
        while (nextLine() == "" && !ended);
        if (ended){
            return 0;
        }

        std::vector<std::string> tokens = tokenize(line());
        if (tokens[0][0] == '#'){
            return new Line(context(), LineType::COMMENT);
        }
        if (isLabeled(line())){
            storeLabel(tokens[0]);
            tokens = tokenize(tokens[1]);
        }
        //std::cout << tokens[1] << "\n";
        LineType type = stringToLineType(tokens[0]);
        //std::cout << "##" << tokens[0] << "#" << tokens[1] << "#";
        switch (type){
            case LineType::CALL_N:
            case LineType::ASSERT_STACK_HEIGHT:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<size_t>(context(), type, strToNum<size_t>(tokens[1]));
            case LineType::PUSH_INT:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<int_type>(context(), type, strToNum<int_type>(tokens[1]));
            case LineType::JUMP:
            case LineType::JUMP_IF:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<std::string>(context(), type, tokens[1]);
            case LineType::PUSH_STRING:
            case LineType::PUSH_VAR:
            case LineType::INIT_VAR:
            case LineType::SET_VAR:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<std::string>(context(), type, strToString(tokens[1]));
            case LineType::PUSH_BOOLEAN:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<bool>(context(), type, tokens[1] == "true");
            case LineType::PUSH_NOTHING:
            case LineType::PUSH_ARRAY:
            case LineType::PUSH_MAP:
            case LineType::RETURN:
            case LineType::DUP:
            case LineType::POP:
            case LineType::CALL:
            case LineType::CALL_N2:
            case LineType::PUSH_SCOPE:
            case LineType::POP_SCOPE:
            case LineType::NOP:
            case LineType::PRINT_STACK:
                return new Line(context(), type);
            case LineType::LINE_COLUMN_NUMBER:
                if (tokens.size() != 3){
                    std::ostringstream stream;
                    stream << "Expected two arguments, not " << tokens.size() - 1;
                    error(stream.str());
                }
                lang_line_number = strToNum<size_t>(tokens[1]);
                lang_column_number = strToNum<size_t>(tokens[2]);
                return new Line(context(), type);
            case LineType::ERROR:
            default:
                error("Don't know what to do with ", line());
        }
        return 0;
    }

    template<typename T>
    T strToNum(std::string str){
        T number;
        if (!(std::istringstream(str) >> number)) {
            error("Expected Number, got ", str);
        }
        return number;
    }

    std::string strToString(std::string str){
        std::vector<std::string> replacements = {
            "\\n", "\n",
            "\\t", "\t"
        };
        for (size_t i = 0; i < replacements.size(); i += 2){
            replaceAll(str, replacements[i], replacements[i + 1]);
        }
        return str;
    }

    std::vector<std::string> tokenize(std::string str){
        std::vector<std::string> tokens;
        size_t split_index = str.find(" ");
        if (split_index >= str.size()){
            tokens.push_back(str);
        } else {
            tokens.push_back(str.substr(0, split_index));
            tokens.push_back(str.substr(split_index + 1));
        }
        return tokens;
    }

    bool isLabeled(std::string line){
        return !line.empty() && line.at(0) == ':';
    }

    size_t getLabel(std::string str){
        //std::cout << "get label " << str << "\n";
        if (labels.find(str) != labels.end()){
            return labels[str];
        }
        error(std::string("Unknown label ") + str);
        return 0;
    }

    void storeLabel(std::string label){
        //std::cout << "set label " << label << "\n";
        std::string str = std::string(label.begin() + 1, label.end());
        if (labels.find(str) != labels.end()){
            error("Redefine label", str);
        }
        labels[str] = line_number - 1;
    }
};
