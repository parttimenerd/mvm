#include "utils.hpp"

#include "env.hpp"
#include "heapobject.hpp"
#include "scope.hpp"
#include "heapobjects.hpp"

Env::Env(){
    heap = new Heap();
    stack = new Stack();
}
