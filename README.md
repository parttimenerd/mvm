Important note: This README is only a rough draft and is only conceptually what I'm going to implement.

How to build it: `./build.sh`

How to run the hello world program: run `./mvm < examples/hello_world.mvm`.

How to run the test suite: run `./test.sh` (it requires ruby and colordiff to be installed)

MVM
=================
My first little Virtual Machine written in C++ (or in Java if it fails).

It's a vm for a dynamic typed language. But actually its just an interpreter for a really trimmed down language. This type of interpreter is sometimes called VM.

Goal
-----------------
The goal of this project is provide a minimal working example of an usable VM. That can be extended later and be used to experiment. Therefore operators like plus and minus are just library functions.
The long term goal is to develop a language that compiles to MVM-Code, that is usable as a small scripting language (like bash).

Implementation notes
----------------
- C++11 with extensive use of the STL
- no additional libraries
- simplicity over performance
  - extensive usage of the algorithm header of the STL

Language
-----------------
The interpreted language is pretty simple and is defined in the following.

The statements are separated by new lines.

Valid variable (and function) names may not contain spaces or start with a number or contain a `|` or contain a new line.

Only ASCII literals a allowed in Strings or names.

Types
- Integer
- Float
- String
  - a string literal is a just the string with all line breaks (`\n`) and back slashes escaped by prepending a back slash
- Array (unbound array)
  - only array literal is `[]` for an empty array
- Map (a dictionary)
  - only map literal is `{}` for an empty map
- Boolean (true and false)
- null (used as default return value of functions)

Function statements
- call func_name N
  - Call a function with N values from the stack
  - the function creates a new scope, stores the current stack index
  - call by value
  - N >= number of arguments defined in function declaration
- call_var_args func_name
  - like above, but first pops the number of arguments from the stack
- push_val value
  Pushs a literal value on the stack
- push_var variable_name
  Pushs the content of the variable on the stack
- set var_name
  Set a variable to the top most item on the stack (pops it)
- dup
  Duplicates the top element of the stack

Control statement
- while
  - pops a value and if this values is equivalent to true, it executes the following code block
  - after executing the block, the while statement is executed again
- if
  - followed by a code block
  - pops a value from the stack and executes the block if it isn't `false` or `null`
- else
  begins an else block
- end
  end keyword for loops, if, ife, else, … blocks
- function arg1 … argN
  - followed by a code block, ended by `end`
  - if more than N arguments are passed by the caller, the additional arguments are stored as an array in the `arguments` variable
- ret
  - returns from the current function
  - pops all values (besides the last) from the stack that are pushed on it from the function
  - a value or null has to be pushed on the stack before
  - each function has to end with a `ret` statement

More shouldn't be neccessary to build a VM for a simple (to be designed) imperative language

Garbage collection
------------------------
The VM uses a simple reference counting garbage collection.
All values are located on the heap (modelled by a collection class). When a value is pushed initially on the stack, it's ref count is 1.

The ref count is decreased when
- the value is popped from the stack
- a variable that has this value
  - is removed because it's scope is dropped
  - is assigned a new value
- the value is removed from / overwritten in a collection

The ref count is increased
- the value is assigned to a (new) variable
  - adding this to a function call frame implies this
- the value is pushed on the stack
- the value is added to a collection

A value is removed from the heap (and cleaned up) if its ref count is zero. This is checked each time the ref count is decreased.
Collections: Just the collection itself is removed, not its elements. Their ref counts are decreased.

Architecture
------------------------
Classes
- Utils
  - methods: validate variable/function name
- Value
  - properties: data, reference count
  - method: abstract getType()
  - Types are subclasses
    - methods: getData(), decrRefCount(), incrRefCount(), deletable() (ref count == 0), getRefCount()
- TypeEnum
- Heap (contains all values), essentially an ArrayList (std::vector)
  - methods: addValue(Value), freeValue(Reference to Value), getValues()
- ScopeStack
  - properties: scope stack, current stack num
  - methods: push, pop, getCurrentStackNum, popTill(stack num)
- ValueStack
  - properties: value stack
  - methods: push, pop, …
- Scope
  A scope with variables, ..., there's also a root scope defining all functions
  - properties: dictionary of variables -> value reference, parent scope, root-scope
  - methods: isRoot(), getVariableValue(var_name), setVariable(var_name, value) (searches recursive), isSet(var_name), clean()
  - subclass RootScope (defines all functions and global variable)
    - properties: dictionary of functions
    - methods: addFunction(func_name, function), doesFunctionExist(func_name), getFunction(func_name)
- Function
  - properties: name, number of arguments
  - methods: call(Interpreter, N) (N is the number of arguments liying on the stack of the interpreter)
  - subclass RawFunction
    A function declared in the code
    - properties: array of CodeLines, array of the parameter names
    - call()
      - pushes a new scope on the stack and stores its number
      - calls Interpreter.interpret
      - gets the control back after the `ret` statement
      - then cleans up the ValueStack and the ScopeStack
- CodeLine (parsed line of code)
  - properties: type
  - static methods: parse()
  - methods: eval(Interpreter)
  - subclasses for each CodeLine type
- CodeLineType (enum of code line types)
- Interpreter
  - properties: ScopeStack, ValueStack, Array of CodeLine, RootScope
  - static methods: parse
  - methods: interpret(Array of CodeLines)
- StandardLibrary
  library of built in functions
- VirtualMachine
  - properties: Interpreter
  - methods: exec


Weird Ideas
-------------
Ideas that need some thought
- Add higer order functions
  - Place all declared functions on the heap
    - Garbage Collection of functions
    - function can be passed as a parameter, assigned to variables, added to collections, …
  - The defining scope has a variable referencing it
  - Allow defining a function in every scope
  - Needs a function type
  - Would remove need for additional root scope
  - Needs efficient function-look-up-caching
  - As a function is just an object, it has no name, but is pushed on the stack and then assigned to a variable
  - Function is a subclass of Value
  - Weird sub idea: Extend `function arg1 … argN` to `function arg1 … argN | var1 … varN`
    - all `vari` are set referenced as `vari` in the function
    - the function turns into some sort of implicite collection of values (increments their ref counts)
    - when a function object is removed from the stack, the clean up is equivalent to the dictionary type clean up
    - the values are also set in the scope of a function when it's called (increments their ref counts)
    - the cool part about this is: it allows some sort of scope capturing, i.e. the declared function can have access to a some variables of the scope it was declared in (see C++11, http://en.cppreference.com/w/cpp/language/lambda)
    - pro: variable capturing, …
    - con: Function.call() gets more complicated, see problems below
    - solution: just capture the whole scope
  - to call a function, just push N arguments and then itself on the stack and call `call N`
- Add prototype inheritance to map type (to be implemented later)
  - each map has a property `__proto__` that can reference another map
  - … and a property `__name__` to indicate the group of objects (aka class) it belongs to
  - if map `A` has the prototype map `B` then values are also searched in `B` when `A` doesn't contain the requested
  - needs clone function that creates a new map with reference to all values of another map
  - pro: inheritance almost for free, only small code changes in the map code
  - con: usefulness?
Usefulness approved (12.01) for the above two

- Make every type a subclass of Map (to be implemented later)
  - pro: would allow some sort of object orientation without much work
  - con: –
- Add some sort of exception
  - catch syntax
  ```
      try
        [block of code]
      catch
        [block of code, exception object is stored in variable __ex]
      end
  ```
  - throw syntax: just a `throw` statement that pops a value from the stack
    - actually places this value in the exception object as the `data` property
  - pro: improves usefulness of language dramatically
  - con: complex
  - Solution: Use exception mechanism of C++

No more features, as it contains enough features (when implementing the weird ideas) to be a VM for languages like JavaScript …

Some Problems
--------------------

Pseudocode snippet
  a = 3
  b = a
  b = 5
  => a = 3 or a = 5 ?
`a` is currently  `3`, in ruby it's the same
=> no problem with the current heap and variable model
Another snippet
  a = 3
  lambda { a = 5 }.call // see weird function idea
  => a = 3 or a = 5 ?
`a` is currently `3`, but it should be `5`
Problem: Just assigning references to values doesn't work for capturing variables.
Solution by lot's of languages: capture the whole scope, not just one single variable
Possible solutions:
  - set the parent scope of a functions call scope to the scope it was in when it got declared before
    - store a reference to the scope its declared in
    - Problem: what happens when this scope is removed? A helper function for creating functions could produce lots of functions with invalid parent pointers
    - Solution: Each scope has a ref count, initially at zero
      - the ref count is incremented when a function newly depends on it or it has a new child scope
      - the ref count is decremented when a function depending on it is deleted, or a child scope is deleted
      - if the scope is removed from the scope stack with a non zero ref count, it's stored in a special list in the stack class
      - if the ref count of a non stack scope gets to be zero, the scope is finally deleted
      - scope with a zero ref count are deleted when removed from the stack
      - it's some sort of reference counting GC for scopes
      - good solution?
Should the root scope behave like a function root scope?
- Yes. Use subclass RootScope of Scope, which is an parent for MainScope and FunctionScope.
- Delete the ScopeStack, double link the scopes instead and use a ScopeHeap for GC

TODO
---------------------
- Rewrite it to include thr weird ideas
  - con: complex
  - pro: improves usefulness of language dramatically
- Write some example code
- Don't implement prototype stuff
- Implement `class` statement that does nothing
```
	class
	[CODE]
	end
```


Weird ideas (when the above is implemented)
--------------------
- Implement multi threading via workers (see JavaScript)
