#include "utils.hpp"

#include "scope.hpp"
#include "heap.hpp"
#include "heapobject.hpp"
#include "reference.hpp"
#include "env.hpp"

Scope::Scope(Env *env, SmartReference<Scope> parent_scope) : HeapObject(env, Type::SCOPE) {
    _parent = Optional<SmartReference<Scope>>(parent_scope);
}

Scope::Scope(Env *env) : HeapObject(env, Type::SCOPE) {
    L
    _parent = Optional<SmartReference<Scope>>();
    L
}

SmartReference<Scope> Scope::root(){
    if (isRoot()){
        return SmartReference<Scope>(this);
    }
    return parent()->root();
}

bool Scope::isRoot(){
    return !_parent.has();
}


SmartReference<Scope> Scope::parent(){
    if (!_parent.has()){
        throw std::string("Scope has no parent");
    }
    return _parent.value().copy();
}

void Scope::set(std::string varname, bref obj){
    set(varname, to_rref(obj));
}

void Scope::set(std::string varname, rref obj){
    bool hasSelf = variables.find(varname) != variables.end();
    bool hasWhole = has(varname);
    if (hasWhole && !hasSelf){
        parent()->set(varname, obj.copy());
    } else {
        variables.emplace(varname, obj.copy());
    }
}

void Scope::initVar(std::string varname){
    bool hasSelf = variables.find(varname) != variables.end();
    if (!hasSelf){
        variables.emplace(varname, env->rnothing());
    }
}

void Scope::setHere(std::string varname, rref obj){
    bool hasSelf = variables.find(varname) != variables.end();
    if (hasSelf){
        variables.emplace(varname, obj.copy());
    } else {
        variables.emplace(varname, obj.copy());
    }
}

rref Scope::get(std::string varname){
    if (variables.find(varname) != variables.end()){
        variables[varname]->reference();
        return variables[varname].copy();
    } else if (isRoot()){
        return env->rnothing();
    } else {
        return parent()->get(varname).copy();
    }
}

bool Scope::has(std::string varname, bool recursive){
    if (variables.find(varname) != variables.end()){
        return true;
    }
    if (isRoot() || !recursive){
        return false;
    } else {
        return parent()->has(varname);
    }
}

SmartReference<Scope> Scope::createChild(){
    return make_sref<Scope>(env, SmartReference<Scope>(this));
}

std::string Scope::str(){
    std::ostringstream stream;
    stream << "scope with " << variables.size() << " variables";
    return stream.str();
}

std::string Scope::str_large(){
    std::ostringstream stream;
    stream << str() << "\n";
    for (auto t : variables){
        stream << " " << t.first
               << "=" << t.second->escapedStr()
               << "\n";
    }
    if (!isRoot()){
        stream << "-> " << parent()->str_large();
    }
    return stream.str();
}
