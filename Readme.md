## Quik

This repository contains the compiler for my Quik programming language. I've recently restarted work on the project after a long break to dive into some different things. At the time of writing, my most recent work was getting the lexical analyzer cleaned up, the pre-processor partly cleaned up, and laying the base for the new AST visualization system.

This is the third version of my Quik programming language. Quik is a simple general-purpose systems programming language. Its similar to C in that its basically portable assembler, but its different in that its intended to be somewhat easier to use and understand, which still being able to interface with the underlying system libraries. This project contains the compiler, standard library, and tests. The compiler in turn is split into two parts: the parser library, which loads a source file and generates an AST; and the compiler, which calls the parser and generates Assembly.

I wrote the entire thing from scratch. I don't intend for Quik to fill any special need or be the next big thing; I wrote this primarily as a learning experience, which is why I wrote the whole thing from scratch. Not to mention, its been a really fun hobby project.

Currently, the compiler is capable of generating 64-bit x86 Intel Assembly code and Arm v7 Assembly code. By default, it will generate Intel 64-bit.

### New features

Here are some of the new features since v0.5:   
* Function overloading   
* PIC code for the Intel x86-64 architecture   
* The (basic) standard library is now written in Quik and compiled as a shared library   
* I'm moving the AST viewer to generate DOT graphs instead of that hard-to-read system I formerly had   

As far as technical goes, here are some of the internal changes:   
* I've been working on the new LTAC system   
* The lexical analyzer has been mostly re-written   
* The pre-processor has some improvements   

If you wish to use the last working version, please visit the v0.5 branch.

### The Language
The specs and examples folder has some practical examples of most of the language parts. I'll eventually create a better language specs.

For some example programs and what the Assembly code looks like, see the examples folder.

### Optimizations
I have a small optimization area of the parser, which I plan to expand. I may make it a separate library down the line, but I haven't decided yet. Optimizations are an experimental feature; I originally wanted to make it a default feature, but when I ran my test suite, I realized that we are really a long way off from doing that, so if you wish to try the optimizations, you can run the compiler and qkast with the "--optimize" switch. 

The following optimizations are currently implemented:
 * Remove uncalled functions (including those designated with "extern")
 * Inline small functions (currently set to less than or equal to 5 lines)
 
### Vectorization (Intel SSE/AVX)
One of the new and unique features I'm currently adding to Quik is its vectorization support. I've recently started studying the Intel SSE/AVX extensions, and decided to make an easy-to-use, builtin way to access this.

The data for vectorization is declared using the int128, int256, float128, and float256 types. They are pretty self-explanatory. The SSE extensions handle 128 bits of data (or 4 ints), while the AVX extensions handle 256 bits of data (or 8 ints). The types are declared with the same syntax as arrays. Currently, only parallel math operations are supported (and only addition and multiplication are supported). See the examples folder for sample programs.

I'll expand the documentation on this a little later on my site. For now, keep an eye on the test and examples folder.

### Architectures

Quik primarily targets the Intel x86-64 architecture. The older versions (v0.5 and earlier) had some basic support for the Intel i386 architecture, but when I began re-working the midend and backend, most of that went away. I'm probably not going to bring that back honestly.

There is some basic support for the Arm v7 architecture, but I haven't worked on this portion in a while. Arm is a new architecture for me, and while its something I really want to learn and bring Quik over to, I'm probably going to wait until I can get a better Arm-based computer before I continue work in this area (not to mention the compiler itself needs a lot of work). Currently, the only Arm device other than my Androids I have is a Raspberry Pi 3. Yes, the Pi 3's do have AArch64, but have yet to find a Linux system that runs well enough on it to be usable. I'll probably just wait and get a Pi 4 at this point. 

### Building
To build, you simply need CMake, a C++ compiler, and Python 3 (for testing). Quik does not use any libraries other than the standard C++ library. A compiler supporting at least the C++11 standard is necessary.

### Running
To build, the only external dependencies you need are the GNU binutils (specifically, GAS and ld). Originally, I used NASM as the assembler for the x86 architecture, but I decided to switch to GAS since thats what I was using on the ARM architecture. Honestly, GAS doesn't really have any big advantages over NASM, but using it allows consistency across architectures. Eventually, I would like to incorporate my own assembler.

Three programs are generated: quikc, and qkast. Quikc is the actual compiler. Simply specify your input. By default, the executable will be named "out" and put in the current directory, but you can change this behavior with the -o switch. Multiple inputs are sort of supported, but there's problems with the standard library when you do this. You can still interface with the underlying system without including any part of the standard library. The qkast program generates a visual representation of the abstract syntax tree in DOT and PNG format (this is new and still under development).

Please note that you need to run the "install-stdlib.sh" script before trying to run or build anything.

The generated assembly files are stored in /tmp directory. They are not removed when compiling is complete, so if you wish to see what the assembly looks like, check these out.

### Testing
There are a few Python and Bash scripts for automated testing. Note that you must manually test each architecture. The "test-all.sh" script will run all tests in the /tests directory. To test 64-bit Intel architecture, run "./test-all.sh x64". To test 32-bit Intel, run "./test-all.sh i386". To test Arm, run "./test-all.sh armv7". The test script does not assume a default value; you must specify it. The test scripts will fail if one test fails. Upon success, all generated object and assembly files are removed.

### What works?
I originally had a features list here, but removed it after I began making changes to the backend and broke a lot of stuff (these are good changes :) ).

To get an idea of what works, check the test folders. The example folder functions as the language specs until I write one later on.
