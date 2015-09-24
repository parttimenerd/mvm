/**
 * This is a work in progress draft of a simple virtual machine.
 * It's developed with the aim of conciseness and simplicity.
 * The style of this code might not be the best, but it's good enough to work with it.
 *
 * License: GPL v3
 */

#define DO_LOG
#include "utils.hpp"

#include "heap.hpp"
#include "scope.hpp"
#include "stack.hpp"
#include "env.hpp"
#include "parser.hpp"
#include "lang/lexer.hpp"
#include "lang/parser.hpp"
#include "exception.hpp"

void lex();
void parse();
void compileAndInterpret();

int main(int argc, char *argv[]){
    L
    if (argc == 2){
        try {
            if (std::string(argv[1]) == "lex"){
                L
                lex();
            } else if (std::string(argv[1]) == "parse"){
                L
                parse();
            } else if (std::string(argv[1]) == "interpret"){
                L
                compileAndInterpret();
            }
        } catch (std::string str){
            std::cerr << str;
            exit(1);
        }
     L
        exit(0);
    }
    L
    Env env;
    try {
        env.interpret(new VerboseParser(&std::cin));
    } catch (Exception *ex){
        std::cerr << ex->str();
        exit(1);
    } catch (std::string str){
        std::cerr << str;
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

void compileAndInterpret(){
    lang::Lexer lexer(&std::cin);
    lang::Parser parser(&lexer);
    L
    lang::Node* node = parser.parse();
    L
    std::ostringstream ostream;
    L
    lang::Target target(&ostream);
    L
    node->compile(target);
    L
    Env env;
    L
    std::istringstream istream(ostream.str());
    try {
        env.interpret(new VerboseParser(&istream));
    } catch (Exception *ex){
        std::cerr << ex->str();
        exit(1);
    } catch (std::string str){
        std::cerr << str;
        exit(1);
    }
}
