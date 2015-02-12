#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "utils.hpp"
#include "heapobject.hpp"

struct Heap;

struct Scope : HeapObject {
    bool isRoot = false;
    Scope* parent = nullptr;
    std::unordered_map<std::string, HeapObject*> variables;

   // Scope(Heap *heap);

    Scope(Heap *heap, Scope *scope = 0);

    Scope* root(){
        if (isRoot){
            return this;
        }
        return parent->root();
    }

    void set(std::string varname, HeapObject* obj, bool reference = true);

    void setHere(std::string varname, HeapObject* obj, bool reference = false);

    HeapObject* get(std::string varname, bool returnNothing = true);

    bool has(std::string varname, bool recursive = true);

    Scope* createChild();

    std::string str();

    std::string str_large();
};

#endif
