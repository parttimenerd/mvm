#include "utils.hpp"

#include "scope.hpp"
#include "heap.hpp"
#include "heapobjects.hpp"

/*Scope::Scope(Heap *heap) : HeapObject(Type::SCOPE, heap) {
    isRoot = true;
}*/

Scope::Scope(Heap *heap, Scope *scope) : HeapObject(Type::SCOPE, heap) {
    parent = scope;
    if (scope == 0){
        isRoot = true;
    }
}

void Scope::set(std::string varname, HeapObject* obj, bool reference){
    bool hasSelf = variables.find(varname) != variables.end();
    bool hasWhole = has(varname);
    if (hasWhole && !hasSelf){
        parent->set(varname, obj, reference);
    }
    if (hasWhole && hasSelf){
        if (obj != variables[varname]){
            variables[varname]->dereference();
            if (reference){
                obj->reference();
            }
        }
        variables[varname] = obj;
    }
    if (!hasWhole){
        variables[varname] = obj;
        if (reference){
            obj->reference();
        }
    }
}

void Scope::setHere(std::string varname, HeapObject* obj, bool reference){
    bool hasSelf = variables.find(varname) != variables.end();
    if (hasSelf){
        if (obj != variables[varname]){
            variables[varname]->dereference();
            if (reference){
                obj->reference();
            }
        }
        variables[varname] = obj;
    } else {
        variables[varname] = obj;
        if (reference){
            obj->reference();
        }
    }
}

HeapObject* Scope::get(std::string varname, bool returnNothing){
    if (variables.find(varname) != variables.end()){
        return variables[varname];
    } else if (isRoot){
        return returnNothing ? heap->createNothing() : 0;
    } else {
        return parent->get(varname, returnNothing);
    }
}

bool Scope::has(std::string varname, bool recursive){
    if (variables.find(varname) != variables.end()){
        return true;
    }
    if (isRoot || !recursive){
        return false;
    } else {
        return parent->has(varname);
    }
}

Scope* Scope::createChild(){
    return new Scope(heap, this);
}

std::string Scope::str(){
    std::ostringstream stream;
    stream << "scope with " << variables.size() << " variables";
    return stream.str();
}

std::string Scope::str_large(){
    std::ostringstream stream;
    stream << str();
    for (auto t : variables){
        stream << " " << t.first
               << "=" << t.second->str()
               << ", ref count=" << t.second->reference_count << "\n";
    }
    if (!isRoot){
        stream << "-> " << parent->str_large();
    }
    return stream.str();
}
