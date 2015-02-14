#ifndef STRING_HPP
#define STRING_HPP

#include "utils.hpp"
#include "heapobject.hpp"

struct String : HeapObject {
	std::string value;

	String(Env *env, std::string value) : HeapObject(Type::STRING, env) {
		this->value = value;
	}

	std::string str(){
        std::ostringstream stream;
        stream << "'" << value << "'";
		return stream.str();
	}

	bool operator==(HeapObject* obj){
		return obj->type == type && value.compare(((String*)obj)->value) == 0;
	}

	bool operator<(HeapObject* obj){
		return obj->type == type && value.compare(((String*)obj)->value) < 0;
	}
};

#endif
