global main

section .text

main:
push rbp
mov rbp, rsp
mov rax, 23
pop rbp
ret
