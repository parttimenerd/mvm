#pragma once

#include "utils.hpp"

/*template<class T>
struct Reference;
template<class T>
struct SmartReference;*/
#include "reference.hpp"

struct Stack {

    std::vector<Reference<HeapObject>*> stack;
    std::vector<size_t> frameStack;

    Reference<HeapObject>* pop(bool dereference = false);

    void popAndDeref();

    template<class T>
    void push(Reference<T> *obj, bool reference = true){
        stack.push_back((Reference<HeapObject>*)obj);
        if (reference){
            obj->reference();
        }
    }

    template<class T>
    void push(SmartReference<T> &obj){
        push(obj.reference);
    }

    void dup();

	bool empty(){
		return stack.size() == 0;
	}

	size_t size(){
		return stack.size();
	}

    void pushFrame(){
        frameStack.push_back(size());
    }

    void popFrame(bool dereference = false){
        size_t newSize = 0;
        if (frameStack.size() > 0){
            newSize = frameStack.at(frameStack.size() - 1);
            frameStack.pop_back();
        }
        cleanToSize(newSize, dereference);
    }

    void popFrameAndAddReturn(Reference<HeapObject> *returnVal, bool dereference = false, bool reference = true){
        popFrame(dereference);
        push(returnVal, reference);
    }

    bool hasStackFrames(){
        return frameStack.size() > 0;
    }

    /**
     * Clean the stack so it has the passed size.
     */
    void cleanToSize(size_t _size, bool dereference = false){
        while (size() > _size){
            pop(dereference);
        }
    }

    std::string str();
};
