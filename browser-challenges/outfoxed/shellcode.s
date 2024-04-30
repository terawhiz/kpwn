.global _start
.intel_syntax noprefix

_start:

instruction_1:
  push rax
  push rax
  pop rdi
  pop r12
  jmp $+10
  nop
  nop



instruction_2:
  mov esi, 0x1000
  jmp $+9
  nop

instruction_3:
  mov edx, 7
  jmp $+9
  nop

instruction_4:
  mov eax, 10
  jmp $+9
  nop


instruction_5:
  mov ecx, 0xfff
  jmp $+9
  nop

instruction_6:
  not rcx
  jmp $+11
  nop
  nop
  nop

instruction_7:
  and rdi, rcx
  syscall
  jmp $+12
  nop

instruction_8:
  add r12, 8
  jmp r12