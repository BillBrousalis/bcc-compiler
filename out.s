global main

section .text

main:
push rbp
mov rbp, rsp
sub rsp, 8
mov rax, 3
pop rbp
ret
 