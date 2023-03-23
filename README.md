# BF - Brainfuck Compiler and Interpreter
This is a toy project that provides a compiler for the Brainfuck (BF) language.
It includes a tree walk interpreter that bounds checks all memory access in
debug mode.
The BF can also be translated into LLVM bitcode, such that it can be compiled to 
native machine code.
This project was originally intended as an exercise in ranges and concepts, so
all code is written in C++20.

## Usage
The compiler currently only supports Linux (because of hard-coded file system
paths).
Make sure that LLVM is installed on your system in a location where CMake can
find it.
Run
```commandline
$ cmake -S . -B build
```

to generate the build directory. To compile and run BF program use the provided
`compileAndRun` bash script:
```commandline
$ ./compileAndRun example_programs/HelloWorld.bf
```
This command should compile the BF compiler, compile `HelloWorld.bf` as well
as the `libBf.c` to object code, link both, and execute the program. It should 
print `Hello World` to stdout.

## TODOs
I probably will not have the time to tend to any of these TODOs.
Still, these are the most important tasks left (in order most important to least
important).

### Testing
The entire code currently includes neither unit tests, nor end-to-end tests, so
expect the code to contain a ton bugs.

### Reproducible Build Environment
Probably using Docker.

### Commandline Interface
The compiler always prints the AST, runs the program using the
ASTWalker, then compiles it to LLVM bitcode and writes it to a pre-defined
(Unix) file system path.
The user should be able to choose what the compiler should do by using
corresponding command line arguments.

### Optimization
The elephant in the room.
Requires changing the AST to be mutable and adapting the code gen accordingly.
Simple optimization could detect certain pre-defined patterns in code (e.g.
adding the value of one memory cell to another).
More advanced optimization could e.g. infer which memory cells are reachable in 
any given loop to enable auto parallelization.