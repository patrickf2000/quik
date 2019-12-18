## Quik

### Introduction
This is the third version of my Quik programming language. Like the second version, its a compiler written from scratch. By why written from scratch again? The original compiler was really my first non-trival compiler, so because I was learning, it contained a lot of bad design choices, both in the language and the implementation (especially the implementation). I completely rewrote it from scratch, using better techniques, and I'm very proud of the result (especially the abstract syntax tree generator).

As of the time of writing, it is almost completely at the same point (as far as features go) as the original compiler.

Quik is a my custom programming language; its intended to be a general-purpose systems language like C, with a few extra features. Ultimately, my purpose with this program is to learn compiler development. Quik is basically portable assembler like C.

The backend generates Intel x86 assembly code. The code is intended to be assembled by NASM. The object code is linked with GCC to make everyone's life easier.

### What is working?

The integer and floating-point datatypes and their operations are pretty well supported. Other variables have varying degrees of support beyond lexical analysis, but not much.

Functions are working; you can implement them and push arguments, but you cannot currently retrieve anything.

Comparisons are working between integers. Full If/else if/else is supported, both linearly and nested.

Basic while loops seem to be working (only with integers though).

Some pretty base syntax checking is supported; there is an internal framework within the parser for further functions as the program matures.

The compiler is currently capable of calling some external C functions via the extern keyword. Additionally, you can compile multiple files into one executable.
