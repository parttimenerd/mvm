#ifndef MAP_HPP
#define MAP_HPP

#include "utils.hpp"
#include "heapobject.hpp"
#include "reference.hpp"

/**
 * A map that has either Ints or Strings as keys.
 */
struct Map : HeapObject {
    std::map<HeapObject*, Reference<HeapObject>*> map;
    Type content_type = Type::NOTHING;

    Map(Env *env,
        std::map<HeapObject*, Reference<HeapObject>*> value = std::map<HeapObject*, Reference<HeapObject>*>(),
        bool reference = true);

	std::string str(){
        std::ostringstream stream;
        stream << "[ ";
        for (auto t : map){
            stream << t.first->str() << " => " << t.second->str() << " ";
        }
        stream << "]";
		return stream.str();
	}

    HeapObject* get(HeapObject *key);

	size_t size(){
        return map.size();
	}

    void set(HeapObject *key, Reference<HeapObject>* value);

	bool has(HeapObject *key){
        return map.find(key) != map.end();
	}

    bool operator==(HeapObject&){
		return false;
	}

    bool operator>(HeapObject&){
		return false;
	}

    virtual Reference<HeapObject>* copy();

    virtual void _set(HeapObject *other);
};

#endif
