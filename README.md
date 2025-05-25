This is a basic general purpose matrix/linear algebra library. It started off as an exercise, but now I have plan to 
actually use it in place of glm as I go off and learn Vulkan. 

So far, I have basic implementation of basic matrix operations, such as multiplication, inverse, finding determinants,
and addition/subtraction. I also have PLU factorization worked out for square matrices, alongside using it to solve systems.

It's a bit clunky to use for now. I wanted to support arbitrary matrix dimensions while still writing it in C, which means 
dynamic allocation is necessary. However, system calls like malloc are slow, and I wanted to avoid direct allocation anyways
because chaining matrix operations (ex (P^t)LUx) would cause memory leaks if you wanted to do it all in one line. To 
circumvent this, matrices are stored on a memory pool, which is allocated prior to declaring matrices, and freed after all
operations are complete. 
