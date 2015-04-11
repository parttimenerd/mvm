#pragma once

#include "../utils.hpp"
#include "../parser.hpp"

namespace lang {
/**
 * @brief A compilation target
 *
 * It provides generic methods to construct lower level code, the default
 * implementation just print verbose assembler to standard out.
 */
struct Target {

    std::ostream *out;

    Target(std::ostream *out) : out(out) {}

    Target() : out(&std::cout) {}

    std::vector<size_t> breakLabels;
    std::vector<size_t> continueLabels;

    virtual void PUSH_INT(int_type value){
        *out << "PUSH_INT " << value << "\n";
    }

    virtual void PUSH_FLOAT(float_type value){
        *out << "PUSH_FLOAT " << value << "\n";
    }

    virtual void PUSH_STRING(std::string value){
        *out << "PUSH_STRING " << escape(value) << "\n";
    }

    std::string escape(std::string str){
        std::vector<std::string> replacements = {
            "\n", "\\n",
            "\t", "\\t"
        };
        for (size_t i = 0; i < replacements.size(); i += 2){
            replaceAll(str, replacements[i], replacements[i + 1]);
        }
        return str;
    }

    virtual void PUSH_NOTHING(){
        *out << "PUSH_NOTHING\n";
    }

    virtual void PUSH_BOOLEAN(bool value){
        *out << "PUSH_BOOLEAN " << (value ? "true" : "false") << "\n";
    }

    virtual void PUSH_ARRAY(){
        *out << "PUSH_ARRAY\n";
    }

    virtual void PUSH_MAP(){
        *out << "PUSH_MAP\n";
    }

    virtual void PUSH_VAR(std::string variableName){
        *out << "PUSH_VAR " << variableName << "\n";
    }

    void CALL_N(std::string functionName, size_t numberOfArguments){
        PUSH_VAR(functionName);
        CALL_N(numberOfArguments);
    }

    virtual void CALL_N(size_t numberOfArguments){
        *out << "CALL_N " << numberOfArguments << "\n";
    }

    size_t makeLabel(){
        size_t label = inventLabel();
        placeLabel(label);
        return label;
    }

    size_t inventLabel(){
        static size_t count = 0;
        return ++count;
    }

    virtual void placeLabel(size_t label){
        *out << ":" << label << " NOP\n";
    }

    void pushBreakLabel(size_t label){
        breakLabels.push_back(label);
    }

    void popBreakLabel(){
        breakLabels.pop_back();
    }

    size_t getBreakLabel(){
        return breakLabels.at(breakLabels.size() - 1);
    }

    bool hasBreakLabel(){
        return !breakLabels.empty();
    }

    void pushContinueLabel(size_t label){
        continueLabels.push_back(label);
    }

    void popContinueLabel(){
        continueLabels.pop_back();
    }

    size_t getContinueLabel(){
        return continueLabels.at(continueLabels.size() - 1);
    }

    bool hasContinueLabel(){
        return !continueLabels.empty();
    }

    virtual void JUMP(size_t label){
        *out << "JUMP " << label << "\n";
    }

    virtual void JUMP_IF(size_t label){
        *out << "JUMP_IF " << label << "\n";
    }

    virtual void JUMP_IF_NOT(size_t label){
        *out << "JUMP_IF_NOT " << label << "\n";
    }

    virtual void NOP(){
        *out << "NOP\n";
    }

    virtual void DUP(){
        *out << "DUP\n";
    }

    virtual void LINE_COLUMN_NUMBER(size_t lineNumber, size_t columnNumber){
        *out << "LINE_COLUMN_NUMBER " << lineNumber << " " << columnNumber << "\n";
    }

    virtual void SET_VAR(){
        *out << "SET_VAR\n";
    }

    virtual void INIT_VAR(std::string varName){
        *out << "INIT_VAR " << varName << "\n";
    }

    virtual void PUSH_SCOPE(){
        *out << "PUSH_SCOPE\n";
    }

    virtual void POP_SCOPE(){
        *out << "POP_SCOPE\n";
    }

    virtual void FUNCTION_HEADER(std::string name, std::vector<std::string> parameters){
        *out << "FUNCTION_HEADER " << name;
        for (auto &param : parameters){
            *out << " " << param;
        }
        *out << "\n";
    }

    virtual void FUNCTION_HEADER_WO_NAME(std::vector<std::string> parameters){
        *out << "CLOSURE_HEADER ";
        for (auto &param : parameters){
            *out << " " << param;
        }
        *out << "\n";
    }

    virtual void FUNCTION_END(){
        *out << "FUNCTION_END\n";
    }
};

}
