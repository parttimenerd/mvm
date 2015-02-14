#ifndef MAP_HPP
#define MAP_HPP

#include "utils.hpp"
#include "heapobject.hpp"
#include "int.hpp"
#include "string.hpp"

/**
 * A map that has either Ints or Strings as keys.
 */
struct Map : HeapObject {
    std::map<HeapObject*, HeapObject*> map;
    Type content_type = Type::NOTHING;

	Map(Env *env) : HeapObject(Type::MAP, env) {}

	std::string str(){
        std::ostringstream stream;
        stream << "[ ";
        for (auto t : map){
            stream << t.first->str() << " => " << t.second->str() << " ";
        }
        stream << "]";
		return stream.str();
	}

	HeapObject* get(HeapObject *key){
        return has(key) ? map[key] : env->createNothing();
	}

	size_t size(){
        return map.size();
	}

	void set(HeapObject *key, HeapObject *value){
        if (content_type == Type::NOTHING){
            if (key->type == Type::STRING || key->type == Type::INT){
                content_type = key->type;
            } else {
                throw "Key has wrong type";
            }
        }
        if (key->type == content_type){
            if (has(key)){
                HeapObject* old = map[key];
                if (old != value){
                    old->dereference();
                    map[key] = value;
                }
            } else {
                map[key] = value;
            }
        }
	}

	bool has(HeapObject *key){
        return map.find(key) != map.end();
	}

	bool operator==(HeapObject*){
		return false;
	}

	bool operator>(HeapObject*){
		return false;
	}
};

#endif
