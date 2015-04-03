#include "utils.hpp"

#include "stack.hpp"
#include "reference.hpp"

Reference<HeapObject>* Stack::pop(bool dereference){
    if (stack.empty()){
        throw std::string("No element on stack, can't pop any more");
    }
    Reference<HeapObject>* val = stack.at(stack.size() - 1);
    stack.pop_back();
    if (dereference){
        val->dereference();
    }
    return val;
}

void Stack::popAndDeref(){
    pop(true);
}

void Stack::dup(){
    Reference<HeapObject> *obj = stack.at(stack.size() - 1);
    push(obj);
}

std::string Stack::str(){
    std::ostringstream stream;
    stream << "stack height=" << size() << "\n";
    size_t frameStackPos = frameStack.size();
    for (size_t i = size(); i > 0; i--){
        stream << " " << (i - 1) << ":" << "value=" << stack.at(i - 1)->escapedStr()
         << ", ref count=" << stack.at(i - 1)->reference_count << "\n";
         if (frameStackPos > 0 && frameStack.at(frameStackPos - 1) == i - 1){
            frameStackPos--;
            stream << "--------- frame " << frameStackPos << " ends ---------\n";
         }
    }
    return stream.str();
}
