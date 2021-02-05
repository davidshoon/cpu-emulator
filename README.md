# cpu-emulator

This is a VERY SMALL stack-based virtual machine / cpu emulator.

Compile with: g++ test.cpp.

It should be noted this emulator/vm isn't secure. For example,
you could push onto the stack and exceed the buffer size (in the C
version) as it's statically allocated and doesn't expand.

In the C++ version, there is no opcode to access the heap yet, but
if there were, it's possible to exceed the limits with a direct access
to out of bounds of the heap, if the opcode handler doesn't check 
the boundaries.

So basically this code should be regarded as early-alpha.
