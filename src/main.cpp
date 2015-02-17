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


// TODO add other line types


int main(){
	Heap heap;
	Env env;
	auto print_env = [](Env* env, std::vector<HeapObject*>, std::vector<HeapObject*>){
        std::cout << env->heap->str();
        std::cout << env->stack->str();
        return env->createNothing()->transfer();
	};
	env.addFunction("print_env", 0, print_env);
	auto print = [](Env* env, std::vector<HeapObject*>, std::vector<HeapObject*> misc_args){
        for (size_t i = 0; i < misc_args.size(); i++){
            std::cout << misc_args[i]->str();
        }
        return env->createNothing()->transfer();
	};
    env.addFunction("print", 0, print);
    auto println = [](Env* env, std::vector<HeapObject*>, std::vector<HeapObject*> misc_args){
        for (size_t i = 0; i < misc_args.size(); i++){
            std::cout << misc_args[i]->str();
        }
        std::cout << "\n";
        return env->createNothing()->transfer();
    };
    env.addFunction("println", 0, println);
    auto binary_math_op = [](std::function<int_type(int_type, int_type)> math_op, int_type neutral_elem, Env* env, std::vector<HeapObject*> args, std::vector<HeapObject*> misc_args){
        int_type result = 0;
        auto func = [math_op, neutral_elem](std::vector<HeapObject*> arr){
            int_type res = neutral_elem;
            for (size_t i = 0; i < arr.size(); i++){
                if (arr[i]->type == Type::INT){
                    res = math_op(res, static_cast<Int*>(arr[i])->value);
                }
            }
            return res;
        };
        result = math_op(func(args), func(misc_args));
        return env->createInt(result)->transfer();
    };
    auto add = [binary_math_op](Env* env, std::vector<HeapObject*> args, std::vector<HeapObject*> misc_args){
        return binary_math_op([](int_type x, int_type y){ return x + y; }, 0, env, args, misc_args);
    };
    env.addFunction("add", 2, add);
    auto mul = [binary_math_op](Env* env, std::vector<HeapObject*> args, std::vector<HeapObject*> misc_args){
        return binary_math_op([](int_type x, int_type y){ return x * y; }, 1, env, args, misc_args);
    };
    env.addFunction("mul", 2, mul);
    auto sub = [binary_math_op](Env* env, std::vector<HeapObject*> args, std::vector<HeapObject*> misc_args){
        return binary_math_op([](int_type x, int_type y){ return x - y; }, 0, env, args, misc_args);
    };
    env.addFunction("sub", 2, sub);
    auto div = [binary_math_op](Env* env, std::vector<HeapObject*> args, std::vector<HeapObject*> misc_args){
        return binary_math_op([](int_type x, int_type y){ return x / y; }, 1, env, args, misc_args);
    };
    env.addFunction("div", 2, div);
    env.interpret(new VerboseParser(&std::cin));
	/*std::cout << "########-#-#########################\n";
	Int* integer = env.createInt(3);

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
	std::cout << env.heap->str();*/
}
