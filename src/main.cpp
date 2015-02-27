/**
 * This is a work in progress draft of a simple virtual machine.
 * It's developed with the aim of conciseness and simplicity.
 * The style of this code might not be the best, but it's good enough to work with it.
 *
 * License: GPL v3
 */


#include "utils.hpp"

#include "heap.hpp"
#include "heapobjects.hpp"
#include "scope.hpp"
#include "stack.hpp"
#include "env.hpp"
#include "parser.hpp"
#include "stdlib.hpp"
#include "lang/lexer.hpp"
#include "lang/parser.hpp"

// TODO add other line types

void lex();
void parse();

int main(int argc, char *argv[]){
    if (argc == 2){
        if (std::string(argv[1]) == "lex"){
            lex();
        } else if (std::string(argv[1]) == "parse"){
            parse();
        }
        exit(0);
    }
	Env env;
    loadStdLib(env);
    try {
        env.interpret(new VerboseParser(&std::cin));
    } catch (std::string msg){
        std::cerr << msg << "\n";
        exit(1);
    }
}

void lex(){
    lang::Lexer lexer(&std::cin);
    while (!lexer.ended()){
        std::cout << lexer.nextToken()->str() << "\n";
    }
    std::cout << lexer.nextToken()->str() << "\n";
}

void parse(){
    lang::Lexer lexer(&std::cin);
    lang::Parser parser(&lexer);
    lang::Node* node = parser.parse();
    lang::Target target;
    node->compile(target);
}
