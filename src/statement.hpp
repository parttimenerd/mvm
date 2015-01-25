#ifndef STATEMENT_H
#define STATEMENT_H

#include "utils.hpp"
#include "lineinfo.hpp"
#include <ostream>



class Statement {
		
	std::shared_ptr<LineInfo> info;
		
	public:
	Statement(std::shared_ptr<LineInfo> info) {
		this->info = info;
	}
	
	void toString(std::ostream& os, int indent = 0){}

};



# endif
