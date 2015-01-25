#include "statement.hpp" 
#include "dup_statement.hpp"
#include <iostream>

int main(){
	std::cout << "halo";
	DupStatement a(std::make_shared<LineInfo>(LineInfo(2)));
	a.toString(std::cout);
	return 0;
}
