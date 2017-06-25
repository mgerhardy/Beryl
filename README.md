# Beryl - A green thread library


![beryl icon](img/berylsvg.png)


## Coding with beryl

One of the main aims of the beryl library is it to bring the concurrency features of erlang and the go programming language to c++.
```c
#include <iostream>
#include "beryl.hpp"


int main (){
  beryl::create([] () {
    auto info = beryl::utils::getInfo();
    std::cout << "Hello from thread: " << info.name << std::endl;
  } );

  beryl::go();  
}
```


Beryl offers support for c++ closures or normal functions.

```c
#include <iostream>
#include "beryl.hpp"


void helloWorld() {
  std::cout << "Hello from beryl" << std::endl;
}

int main (){
  beryl::create([] () {
    auto info = beryl::utils::getInfo();
    std::cout << "Hello from thread: " << info.name << std::endl;
  }, "First_thread");

  beryl::create(&helloWorld, "helloWorld-thread");

  beryl::go();  
}
```
This outputs:

```
Hello from thread: First_thread
Hello from beryl
```



## Extended memory control over your threads

Beryl offers you to limit the stack size or the allocated memory on the heap
for each thread

### Stack memory
Still WIP :construction:

Each thread gets its own stack with a size of 10K.
You can provide a callback method which gets invoked when your thread
runs out of memory.

It is also possible to resize the actual stack.
This is not considered to be good practice because the
old stack data needs to be copied into the new stack.

### Heap memory
Still WIP :construction:

Beryl overwrites the default STL memory allocator so that it can
also monitor the heap consuming behaviour of your threads.


## TODOs :fire:

- [ ] save and restore all volatile registers
- [ ] allow return values and a better support for parameters :construction:
- [ ] implement IO functions
- [ ] implement event/messaging system for communication between threads
