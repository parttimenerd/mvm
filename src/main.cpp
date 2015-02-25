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


// TODO add other line types


int main(){
	Env env;
    loadStdLib(env);
    try {
        env.interpret(new VerboseParser(&std::cin));
    } catch (std::string msg){
        std::cerr << msg << "\n";
        exit(1);
    }
}
