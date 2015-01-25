#ifndef DUP_STATEMENT_HPP
#define DUP_STATEMENT_HPP

#include "utils.hpp"
#include "statement.hpp"

class DupStatement : public Statement {

public:
	using Statement::Statement;	

	void toString(std::ostream& os, int indent = 0){
		print_ws(os, indent);
		os << "dup";
	}
};

#endif
