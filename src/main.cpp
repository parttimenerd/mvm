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


// TODO add other line types


int main(){
	Heap heap;
	Env env;
	Int* integer = env.createInt(3);
	std::cout << env.heap->str();

	Stack stack;
	stack.pushFrame();
	stack.push(integer);
	stack.push(integer);
	std::cout << stack.str();
	stack.popFrame(true);
	std::cout << stack.str();
	std::cout << env.heap->str();
	std::cout << "----------\n";
	Scope *scope = new Scope(&env);
	//std::cout << scope->str();
	scope->set("abc", env.createInt(3));
	//std::cout << scope->str_large();
	scope->set("abc2", env.createNothing());
	//std::cout << scope->str_large();
	scope->set("abc", env.createInt(7));
	std::cout << env.heap->str();
	//std::cout << scope->str_large();
	Scope *childScope = scope->createChild();
	//childScope->set("abc", heap.createInt(5));
	//heap.str();
	//childScope->set("abc3", heap.createNothing());
	std::cout << childScope->str_large();
	std::cout << "#############\n";
	std::cout << env.heap->str();
}
