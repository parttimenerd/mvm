#pragma once

#include "utils.hpp"

/**
 * @brief The value stack of this vm.
 * It supports stack frames.
 *
 * Actually it stores SmartReference<Reference> objects.
 */
class Stack {

    std::vector<rref> stack;
    std::vector<size_t> frame_stack;

public:
    /**
     * @brief Pop a value from the stack
     * @throws runtime expcetion if the stack is empty
     * @return topmost value of the stack
     */
    rref pop();

    /**
     * @brief Pushes the passed reference on the stack.
     * @param value reference value
     */
    void push(rref value);

    /**
     * @brief Duplicates the topmost stack element on the stack.
     * @throws runtime expcetion if the stack is empty
     */
    void dup();

    /**
     * @brief Is the stack empty?
     * @return stack empty?
     */
    bool empty(){
        return stack.size() == 0;
    }

    /**
     * @brief Get the height of stack
     * @return stack height
     */
    size_t size(){
        return stack.size();
    }

    /**
     * @brief Push a frame on the frame stack
     */
    void pushFrame();

    /**
     * @brief Pop a frame from the frame stack.
     * And pop the associated stack elements.
     * @throws runtime expcetion if the stack is empty
     */
    void popFrame();

    /**
     * @brief Is the frame stack empty?
     * @return frame stack empty?
     */
    bool frameStackEmpty(){
        return frame_stack.size() > 0;
    }

    /**
     * Clean the stack so it has the passed size.
     */
    void cleanToSize(size_t _size);

    std::string str();
};
