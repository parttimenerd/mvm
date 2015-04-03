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
        return value;
	}

	std::string escapedStr(){
        std::string str = value + "";
        std::vector<std::string> replacements = {
            "\n", "\\n",
            "\t", "\\t"
        };
        for (size_t i = 0; i < replacements.size(); i += 2){
            replaceAll(str, replacements[i], replacements[i + 1]);
        }
        return str;
	}

	void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if(from.empty()){
            return;
        }
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    bool operator==(HeapObject &obj){
        return obj.type == type && value.compare(((String*)&obj)->value) == 0;
	}

	bool operator<(HeapObject* obj){
        return obj->type == type && value.compare(((String*)&obj)->value) < 0;
	}

    virtual Reference<HeapObject>* copy(){
        return (Reference<HeapObject>*)new Reference<String>(env, new String(env, value));
    }

    virtual void _set(HeapObject *other){
        value = ((String*)other)->value;
    }
};

#endif
