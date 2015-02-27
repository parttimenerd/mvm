#pragma once

#include "../utils.hpp"

namespace lang {
/**
 * @brief A compilation target
 *
 * It provides generic methods to construct lower level code, the default
 * implementation just print verbose assembler to standard out.
 */
struct Target {

    virtual void PUSH_INT(int_type value){
        std::cout << "PUSH_INT " << value << "\n";
    }

    virtual void PUSH_STRING(std::string escapedStr){
        std::cout << "PUSH_STRING " << escapedStr << "\n";
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

    virtual void PUSH_VAR2(){
        std::cout << "PUSH_VAR2\n";
    }
};

}
