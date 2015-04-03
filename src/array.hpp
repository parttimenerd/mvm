#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "utils.hpp"
#include "heapobject.hpp"
#include "env.hpp"

/*
 * Resizable array
 */
struct Array : HeapObject {
    std::vector<Reference<HeapObject>*> value;

    Array(Env *env, std::vector<Reference<HeapObject>*> value, bool reference = true) : HeapObject(Type::ARRAY, env) {
		this->value = value;
        if (reference) {
            for (auto& element : value) {
                element->reference();
            }
        }
	}

	std::string str(){
        std::ostringstream stream;
        stream << "[ ";
        for (size_t i = 0; i < value.size(); i++){
            stream << value.at(i)->str() << " ";
        }
        stream << "]";
		return stream.str();
	}

	HeapObject* at(size_t i){
        return value.at(i);
	}

	size_t size(){
        return value.size();
	}

    void set(size_t i, Reference<HeapObject> *obj, bool reference = true){
        value[i] = obj;
        if (reference){
            obj->reference();
        }
	}

    void set(size_t i, HeapObject *obj, bool reference = true){
        value[i] = new Reference<HeapObject>(env, obj, reference);
    }

    void add(Reference<HeapObject> *obj){
        value.push_back(obj);
	}

    void add(HeapObject *obj){
        value.push_back(new Reference<HeapObject>(env, obj));
    }

    bool operator==(HeapObject&){
		return false;
	}

    bool operator>(HeapObject&){
		return false;
	}

    virtual Reference<HeapObject>* copy(){
        return (Reference<HeapObject>*)new Reference<Array>(env, new Array(env, value));
    }

    virtual void _set(HeapObject *other){
        auto new_value = ((Array*)other)->value;
        for (auto &val : new_value){
            val->reference();
        }
        for (auto &val : value){
            val->dereference();
        }
        value = new_value;
    }
};

#endif
