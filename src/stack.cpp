#include "utils.hpp"

#include "stack.hpp"
#include "reference.hpp"
#include "baseobject.hpp"

rref Stack::pop(){
    if (empty()){
        throw std::string("No element on stack, can't pop any more");
    }
    rref val = stack.at(stack.size() - 1);
    stack.pop_back();
    return val.copy();
}

void Stack::push(rref value){
    stack.push_back(value);
}

void Stack::dup(){
    rref obj = pop();
    push(obj.copy());
    push(obj.copy());
}

void Stack::pushFrame()
{
    frame_stack.push_back(size());
}

void Stack::popFrame(){
    cleanToSize(frame_stack.at(frame_stack.size() - 1));
    frame_stack.pop_back();
}

void Stack::cleanToSize(size_t _size){
    while (size() > _size){
        pop();
    }
}

std::string Stack::str(){
    std::ostringstream stream;
    stream << "stack height=" << size() << "\n";
    size_t frameStackPos = frame_stack.size();
    for (size_t i = size(); i > 0; i--){
        stream << " " << (i - 1) << ":" << "value=" << stack.at(i - 1)->escapedStr()
               << "\n";
        if (frameStackPos > 0 && frame_stack.at(frameStackPos - 1) == i - 1){
            frameStackPos--;
            stream << "--------- frame " << frameStackPos << " ends ---------\n";
        }
    }
    return stream.str();
}
