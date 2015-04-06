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

    std::vector<size_t> breakLabels;
    std::vector<size_t> continueLabels;

    virtual void PUSH_INT(int_type value){
        std::cout << "PUSH_INT " << value << "\n";
    }

    virtual void PUSH_FLOAT(float_type value){
        std::cout << "PUSH_FLOAT " << value << "\n";
    }

    virtual void PUSH_STRING(std::string value){
        std::cout << "PUSH_STRING " << escape(value) << "\n";
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
        std::cout << "PUSH_NOTHING\n";
    }

    virtual void PUSH_BOOLEAN(bool value){
        std::cout << "PUSH_BOOLEAN " << (value ? "true" : "false") << "\n";
    }

    virtual void PUSH_ARRAY(){
        std::cout << "PUSH_ARRAY\n";
    }

    virtual void PUSH_MAP(){
        std::cout << "PUSH_MAP\n";
    }

    virtual void PUSH_VAR(std::string variableName){
        std::cout << "PUSH_VAR " << variableName << "\n";
    }

    void CALL_N(std::string functionName, size_t numberOfArguments){
        PUSH_VAR(functionName);
        CALL_N(numberOfArguments);
    }

    virtual void CALL_N(size_t numberOfArguments){
        std::cout << "CALL_N " << numberOfArguments << "\n";
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
        std::cout << ":" << label << " NOP\n";
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
        std::cout << "JUMP " << label << "\n";
    }

    virtual void JUMP_IF(size_t label){
        std::cout << "JUMP_IF " << label << "\n";
    }

    virtual void JUMP_IF_NOT(size_t label){
        std::cout << "JUMP_IF_NOT " << label << "\n";
    }

    virtual void NOP(){
        std::cout << "NOP\n";
    }

    virtual void DUP(){
        std::cout << "DUP\n";
    }

    virtual void LINE_COLUMN_NUMBER(size_t lineNumber, size_t columnNumber){
        std::cout << "LINE_COLUMN_NUMBER " << lineNumber << " " << columnNumber << "\n";
    }

    virtual void SET_VAR(){
        std::cout << "SET_VAR\n";
    }

    virtual void INIT_VAR(std::string varName){
        std::cout << "INIT_VAR " << varName << "\n";
    }

    virtual void PUSH_SCOPE(){
        std::cout << "PUSH_SCOPE\n";
    }

    virtual void POP_SCOPE(){
        std::cout << "POP_SCOPE\n";
    }

    virtual void FUNCTION_HEADER(std::string name, std::vector<std::string> parameters){
        std::cout << "FUNCTION_HEADER " << name;
        for (auto &param : parameters){
            std::cout << " " << param;
        }
        std::cout << "\n";
    }

    virtual void FUNCTION_HEADER_WO_NAME(std::vector<std::string> parameters){
        std::cout << "CLOSURE_HEADER ";
        for (auto &param : parameters){
            std::cout << " " << param;
        }
        std::cout << "\n";
    }

    virtual void FUNCTION_END(){
        std::cout << "FUNCTION_END\n";
    }
};

}
