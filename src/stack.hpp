#ifndef STACK
#define STACK

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

struct HeapObject;

struct Stack {

	std::vector<HeapObject*> stack;
    std::vector<size_t> frameStack;

	HeapObject* pop(bool dereference = false);

    void popAndDeref();

	void push(HeapObject *obj, bool reference = true);

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

#endif
