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
    PUSH_VAR2,
    SET_VAR,
    SET_VAR2,
    POP,
    DUP,
    JUMP_IF,
    RETURN,
    ERROR,
    COMMENT,
    PRINT_STACK
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
    "PUSH_VAR2",
    "SET_VAR",
    "SET_VAR2",
    "POP",
    "DUP",
    "JUMP_IF",
    "RETURN",
    "ERROR",
    "COMMENT",
    "PRINT_STACK"
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

/**
 * A line of code
 */
struct Line {
	LineType type;

	Line(LineType type){
		this->type = type;
	}

    virtual std::string str(){
        return lineTypeToString(type);
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

    ArgumentedLine(LineType type, T argument) : Line(type){
        this->argument = argument;
    }

    std::string str(){
        std::ostringstream stream;
        stream << lineTypeToString(type) << " " << argument;
        return stream.str();
    }
};

/**
 * A parser for parsing a stream as a sequence of lines.
 */
struct Parser {

    std::istream *stream;
    std::string context;
    std::string current_line;
    size_t line_number = 0;
    bool ended = false;

    Parser(std::istream *stream, std::string context = "") {
        this->stream = stream;
        this->context = context;
    }


    void error(std::string msg, std::string msg1 = ""){
        std::ostringstream stream;
        stream << "Error in line " << line_number << ": " << msg << msg1;
        std::cerr << stream.str() << "\n";
        throw stream.str();
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

    VerboseParser(std::istream *stream, std::string context = "") : Parser(stream, context) {}

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
            if (elem->type == LineType::JUMP_IF){
                ArgumentedLine<std::string>* line = (ArgumentedLine<std::string>*)elem;
                size_t target = getLabel(line->argument);
                delete line;
                (*vec)[i] = new ArgumentedLine<size_t>(LineType::JUMP_IF, target);
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
            return new Line(LineType::COMMENT);
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
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<size_t>(type, strToNum<size_t>(tokens[1]));
            case LineType::PUSH_INT:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<int_type>(type, strToNum<int_type>(tokens[1]));
            case LineType::JUMP_IF:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<std::string>(type, tokens[1]);
            case LineType::PUSH_STRING:
            case LineType::PUSH_VAR:
            case LineType::SET_VAR:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<std::string>(type, strToString(tokens[1]));
            case LineType::PUSH_BOOLEAN:
                if (tokens.size() != 2){
                    error("Expected one argument, got more");
                }
                return new ArgumentedLine<bool>(type, tokens[1] == "true");
            case LineType::PUSH_NOTHING:
            case LineType::PUSH_ARRAY:
            case LineType::PUSH_MAP:
            case LineType::RETURN:
            case LineType::PUSH_VAR2:
            case LineType::SET_VAR2:
            case LineType::DUP:
            case LineType::POP:
            case LineType::CALL:
            case LineType::CALL_N2:
            case LineType::PRINT_STACK:
                return new Line(type);
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
