#include "map.hpp"
#include "utils.hpp"
#include "heapobject.hpp"
#include "reference.hpp"
#include "int.hpp"
#include "string.hpp"
#include "env.hpp"

Map::Map(Env *env,
    std::map<HeapObject*, Reference<HeapObject>*> value, bool reference)
    : HeapObject(Type::MAP, env) {
    if (reference){
        for (auto &val : value){
            val.second->reference();
        }
    }
    this->map = value;
}

HeapObject* Map::get(HeapObject *key){
    return has(key) ? map[key] : (Reference<HeapObject>*) env->createNothing();
}

void Map::set(HeapObject *key, Reference<HeapObject>* value){
    if (content_type == Type::NOTHING){
        if (key->type == Type::STRING || key->type == Type::INT){
            content_type = key->type;
        } else {
            throw std::string("Key has wrong type");
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

Reference<HeapObject>* Map::copy(){
    return (Reference<HeapObject>*) new Reference<Map>(env, new Map(env, map));
}

void Map::_set(HeapObject *other){
    auto new_value = ((Map*)other)->map;
    for (auto &val : map){
        if (new_value.find(val.first) == new_value.end()){
            val.second->dereference();
            map.erase(val.first);
        }
    }
}
