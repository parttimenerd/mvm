#pragma once

#include "utils.hpp"

class HeapObject;
class Env;

class Heap {

    Env *env;
    std::unordered_map<id_type, std::unique_ptr<HeapObject>> heap;
    id_type max_id = 0;

public:

    Heap(Env *env) : env(env) { }

    std::string str();

    /**
     * @brief Delete the passed object
     *
     * @param object passed object
     */
    void del(HeapObject *obj);

    void dereference(HeapObject *obj);

    /**
     * @brief Add the passed object
     *
     * @param object passed object
     */
    void add(HeapObject *obj);

    /**
     * @brief Creates a new ID
     * @return new ID
     */
    id_type newId(){
        return max_id++;
    }

    ~Heap() = default;
};
