#include "mcall.h"

  .section ".text.init"
  .globl _start
_start:
  call main
  li a7, SBI_SHUTDOWN
  ecall
