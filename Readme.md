## Quik

### Note
Check the dev branch for the latest progress; the master branch may not have all the latest commits.

### Introduction
This is the third version of my Quik programming language. Quik is a simple general-purpose systems programming language. Its similar to C in that its basically portable assembler, but its different in that its intended to be somewhat easier to use and understand, which still being able to interface with the underlying system libraries. This project contains the compiler, standard library, and tests. The compiler in turn is split into two parts: the parser library, which loads a source file and generates an AST; and the compiler, which calls the parser and generates Assembly.

I wrote the entire thing from scratch. I don't intend for Quik to fill any special need or be the next big thing; I wrote this primarily as a learning experience, which is why I wrote the whole thing from scratch. Not to mention, its been a really fun hobby project.

Currently, the compiler is capable of generating 32-bit and 64-bit Intel Assembly code. By default, it will generate 64-bit.

### The Language
I currently don't have a document or anything with the language specs. Ok, I actually do, but its for the original versions of Quik. The specs folder contains examples of pretty much everything supported by the implementations.

For some example programs and what the Assembly code looks like, see the examples folder.

### Optimizations
I have a small optimization area of the parser, which I plan to expand. I may make it a separate library down the line, but I haven't decided yet. While I'm doing my best to make sure the optimizations don't create problems, its more than likely I'll miss something. Should you have an issue, optimizations can be disabled in the compiler and the qkast program using the "--no-optimize" switch (Note: In the qkast program, this MUST be the last argument).

The following optimizations are currently implemented:
 * Remove uncalled functions (including those designated with "extern")
 * Inline small functions (currently set to less than or equal to 5 lines)

### Building
To build, you simply need CMake, a C++ compiler, and Python 3 (for testing). Quik does not use any libraries other than the standard C++ library. A compiler supporting at least the C++11 standard is necessary.

### Running
In order to use the compiler, you need NASM and GCC. NASM is used to build an object file from the generated assembly. GCC is used to link it (this makes everyone's life easier). 

Three programs are generated: quikc, qkast, and lextest. Quikc is the actual compiler. Simply specify your input. By default, the executable will be named "out" and put in the current directory, but you can change this behavior with the -o switch. Multiple inputs are sort of supported, but there's problems with the standard library when you do this. You can still interface with the underlying system without including any part of the standard library. The qkast program generates a visual representation of the abstract syntax tree (it just prints it to the console). I wrote this primarily for development purposes, but its interesting to understand how the frontend works. The lextest tests the lexical analyzer component. You should have no need to run this unless you plan to work on the language.

Please note that you need to run the "install-stdlib.sh" script before trying to run or build anything.

The generated assembly files are stored in /tmp directory. They are not removed when compiling is complete, so if you wish to see what the assembly looks like, check these out.

### Testing
There are a few Python and Bash scripts for automated testing. Note that you must manually test each architecture. The "test-all.sh" script will run all tests in the /tests directory. To test 64-bit architecture, run "./test-all.sh x64". To test 32-bit, run "./test-all.sh i386". The test script does not assume a default value; you must specify it. The test scripts will fail if one test fails. Upon success, all generated object and assembly files are removed.

### What works?
I originally had a features list here, but removed it after I began making changes to the backend and broke a lot of stuff (these are good changes :) ).

To get an idea of what works, check the test folders. The example folder functions as the language specs until I write one later on.
