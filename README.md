# BCC - Bad C Compiler

**Building a basic C compiler from scratch, for educational purposes and bragging rights.**

Outputs `x86 64-bit assembly`, nasm & gcc are used from there to create the binary file.

The goal is to compile basic programs, `#includes`, `typedefs` and other fancy things are forbidden.

This is a **non-optimizing compiler**, it doesn't make use of an intermediate representation phase.

Simplicity is valued more highly than support for more features. Will try to keep the entire thing under 3000 lines.

Taking inspiration from other (hobby) compilers, **no** memory management **is** the memory management scheme here.

**Work in progress**

An example of what `bcc` can do as of now:

```
$ GRAPH=1 ./bcc tests/test13.c && echo "\nGenerated assembly:" && cat out.s && ./build.sh

Source Code:
int add(int a, int b)
{
  return a + b;
}

int mul(int a, int b)
{
  return a * b;
}

int main()
{
  int x = 2;
  int y = 3;

  int z = add(x, y);
  int w = mul(z, 5);

  return w * 4;
}


Generated assembly:
global add
global mul
global main

section .text

add:
push rbp
mov rbp, rsp
mov ebx, edi
mov r8d, esi
add rbx, r8
mov eax, ebx
pop rbp
ret

mul:
push rbp
mov rbp, rsp
mov ebx, edi
mov r8d, esi
imul rbx, r8
mov eax, ebx
pop rbp
ret

main:
push rbp
mov rbp, rsp
sub rsp, 0x10
mov dword [rbp-0x4], 0x2
mov dword [rbp-0x8], 0x3
mov edi, dword [rbp-0x4]
mov esi, dword [rbp-0x8]
call add
mov dword [rbp-0xc], eax
mov edi, dword [rbp-0xc]
mov esi, 0x5
call mul
mov dword [rbp-0x10], eax
mov ebx, dword [rbp-0x10]
mov r8, 0x4
imul rbx, r8
mov eax, ebx
leave
ret

[*] Building asm into executable...
[*] Running it and checking the return value
100
```

**Generated AST graph:**

![](assets/example_AST_graph.png)

**Notes:**

```

The register use is just plain wrong. Deal with it.

Q: Why is there no "sub rsp, X" in all my functions, even though local variables are declared ?

A: The System V ABI for x86-64 specifies a red zone of 128 bytes below %rsp. These 128 bytes belong to the function as long as it doesn't call any other function (it is a leaf function).

Signal handlers (and functions called by a debugger) need to respect the red zone, since they are effectively involuntary function calls.
All of the local variables of your test_function, which is a leaf function, fit into the red zone, thus no adjustment of %rsp is needed. (Also, the function has no visible side-effects and would be optimized out on any reasonable optimization setting).

You can compile with -mno-red-zone to stop the compiler from using space below the stack pointer. Kernel code has to do this because hardware interrupts don't implement a red-zone.

--

TODO:

******* BUG - test12.c
When a function call is part of an expression,
if registers are already used by a first part of the expression
before the call is made, the called function might use the same registers again,
ruining the values they carry.

******* get puts working to print outside the range of [0, 255]
**** clean up lexer - repeating code
**** add unary operator support

labels
--
generate labels for every point we will be jumping to in the source code

keep track of general purpose registers - their names, and if they are in use.
pick the first not in use register to use


Parsing
--
implement conditionals, loops etc

Lexing
--
eventually implement more keywords to detect, but get basic asm generation working first

```
