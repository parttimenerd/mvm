#include "stack.hpp"
#include "heapobject.hpp"

HeapObject* Stack::pop(bool dereference){
    HeapObject* val = stack.at(stack.size() - 1);
    stack.pop_back();
    if (dereference){
        val->dereference();
    }
    return val;
}

void Stack::popAndDeref(){
    pop(true);
}

void Stack::push(HeapObject *obj, bool _reference){
	stack.push_back(obj);
	if (_reference){
        obj->reference();
	}
}

std::string Stack::str(){
    std::ostringstream stream;
    stream << "stack height=" << size() << "\n";
    size_t frameStackPos = frameStack.size();
    std::cout << __LINE__ << "\n";
    for (size_t i = size(); i > 0; i--){
        stream << " " << (i - 1) << ":" << "value=" << stack.at(i - 1)->str()
         << ", ref count=" << stack.at(i - 1)->reference_count << "\n";
         if (frameStackPos > 0 && frameStack.at(frameStackPos - 1) == i - 1){
            frameStackPos--;
            stream << "--------- frame " << frameStackPos << " ends ---------\n";
         }
    }
    return stream.str();
}
